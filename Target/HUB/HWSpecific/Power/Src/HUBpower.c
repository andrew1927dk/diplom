/**
 * @file HUBpower.c
 * @brief Power management
 * @version 0.1
 * @date 2022-12-22
 *
 *  (c) 2022
 */

#include <stm32f4xx_hal.h>
#include <string.h>
#include "tx_api.h"
#include "HUBpower.h"
#include "main.h"
#include "spi.h"
#include "adc.h"
#include "parser.h"
#include "indication.h"

#define VBAT_CHARGE_MIN       2900
#define VBAT_CHARGE_MAX       4200
#define ADC_REFERENCE_VOLTAGE 1210
#define ADC_MAX               0xFFF
#define NUM_SAMPLES_AVERAGE   20
#define TIM1_100              2
#define TIM2_1100             22
#define TIM3_2500             50
#define TIM4_20000            400
#define FLAG_MEAS_POWER       0x00000001U
#define FLAG_VBAT_1           0x00000002U
#define FLAG_VBAT_2           0x00000004U
#define FLAG_MEAS_CHARGE      0x00000008U

typedef enum {
    VPSU_S_ON,
    VPSU_S_OFF
} vpsu_state_t;

typedef enum {
    CLBK_COMPLETED,
    CLBK_EXPECTED
} clbk_state_t;

static void TimerCmpltClbk(ULONG arg);
static void modeSTBY(uint8_t argc, void **argv);
static void rebootSystem(uint8_t argc, void **argv);

// Array of voltages on components
volatile uint32_t VoltLevel[3];
// Battery charge percentage
uint32_t charge_level_percent;

static uint32_t adcData[4];
static uint32_t average_buf[3];
static uint32_t timerCount;

static ULONG                actual_flags;
static TX_EVENT_FLAGS_GROUP evfPower;
static TX_TIMER             txTim;

// Thread powerMeasurement initialization
#define POWER_MEAS_THREAD_STACK_SIZE 128 * 4
static TX_THREAD powerMeasurementHandle;
static void      StartPowerMeasurement(ULONG argument);

// Thread powerButton initialization
#define POWER_BUTTON_THREAD_STACK_SIZE 256 * 4
static TX_THREAD powerButtonHandle;
static void      StartPowerButton(ULONG argument);

power_state_t POWER_Init(void *memoryPoolPtr) {

    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *) memoryPoolPtr;
    CHAR         *pointer = NULL;

    if (tx_byte_allocate(byte_pool, (void **) &pointer, POWER_MEAS_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS) {
        return POWER_S_ERROR;
    }
    if (tx_thread_create(&powerMeasurementHandle, "StartPowerMeasurement", (void *) StartPowerMeasurement, 1, pointer,
                         POWER_MEAS_THREAD_STACK_SIZE, 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS) {
        return POWER_S_ERROR;
    }

    if (tx_byte_allocate(byte_pool, (void **) &pointer, POWER_BUTTON_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS) {
        return POWER_S_ERROR;
    }
    if (tx_thread_create(&powerButtonHandle, "StartPowerButton", (void *) StartPowerButton, 1, pointer,
                         POWER_BUTTON_THREAD_STACK_SIZE, 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS) {
        return POWER_S_ERROR;
    }

    if (tx_event_flags_create(&evfPower, "PowerEventFlags") != TX_SUCCESS) {
        return POWER_S_ERROR;
    }

    // Add terminal command to turn off the power ("power" - MODULE + "off" - COMMAND = "power off")
    PARSER_AddCommand(modeSTBY, "power off");
    // Add terminal command for system reboot ("power" - MODULE + "reboot" - COMMAND = "power reboot")
    PARSER_AddCommand(rebootSystem, "power reboot");

    // Turn on Ethernet
    PIN_SET(ETH_ON);

    return POWER_S_OK;
}

power_state_t POWER_Deinit(void) {
    return POWER_S_OK;
}

// Conversion of volts to percentage of battery charge ( LINEAR DEPENDENCE )
static uint32_t ChargeToPercent(uint32_t volt_value) {
    if (volt_value <= VBAT_CHARGE_MIN) {
        return 0;
    } else if (volt_value >= VBAT_CHARGE_MAX) {
        return 100;
    }

    return (volt_value - VBAT_CHARGE_MIN) * 100 / (VBAT_CHARGE_MAX - VBAT_CHARGE_MIN);
}

static void modeSTBY(uint8_t argc, void **argv) {
    __disable_irq();
    PIN_RESET(ON_LATCH);
    __WFI();
}

static void rebootSystem(uint8_t argc, void **argv) {
    NVIC_SystemReset();
}

// Button click detection
static void StartPowerButton(ULONG argument) {

    PIN_SET(ON_LATCH);

    while (PIN_READ(ON_DET) == GPIO_PIN_RESET) {
        tx_thread_sleep(10);
    }

    while (1) {
        if (PIN_READ(ON_DET) == GPIO_PIN_RESET) {
            tx_thread_sleep(50);    // Debounce delay

            if (PIN_READ(ON_DET) == GPIO_PIN_RESET) {
                while (PIN_READ(ON_DET) == GPIO_PIN_RESET) {
                    tx_thread_sleep(10);
                }
                POWER_ButtonOnClbk();
                modeSTBY(0, NULL);
            }
        }
        tx_thread_sleep(100);
    }
}

// Voltage measurement and callback call scenarios
static void StartPowerMeasurement(ULONG argument) {

    tx_timer_create(&txTim, "PowerMeasTimer", (void *) TimerCmpltClbk, 0, 50, 50, TX_NO_ACTIVATE);
    tx_event_flags_create(&evfPower, "POWER event flags");

    // Flags for correct calling of callbacks
    vpsu_state_t vpsu_flag = VPSU_S_OFF;
    clbk_state_t clbk_five_flag = CLBK_EXPECTED;
    clbk_state_t clbk_ten_flag = CLBK_EXPECTED;
    clbk_state_t clbk_batt_failure_flag = CLBK_EXPECTED;

    HAL_ADC_Start_DMA(&hadc1, adcData, 4);

    // Starting a cyclic timer (period -- 50 ms)
    tx_timer_activate(&txTim);

    while (1) {
        // Resetting the timer counter
        timerCount = 0;

        // Buffer clearing
        memset(average_buf, 0, sizeof(average_buf));

        // VBAT_LOAD = 1
        PIN_SET(VBAT_LOAD);

        // Waiting for the end of calculations flag
        tx_event_flags_get(&evfPower, FLAG_MEAS_POWER, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);

        // Measuring the battery charge level in percent
        charge_level_percent = ChargeToPercent(VoltLevel[IDX_VBAT]);

        // Global flag for battery measurement check
        tx_event_flags_set(&evfPower, FLAG_MEAS_CHARGE, TX_OR);

        // Detection of 10% battery charge
        if (charge_level_percent > 10 && (clbk_ten_flag == CLBK_COMPLETED)) {
            clbk_ten_flag = CLBK_EXPECTED;
        } else if (charge_level_percent == 10 && (clbk_ten_flag == CLBK_EXPECTED)) {
            POWER_TenPercentClbk();
            clbk_ten_flag = CLBK_COMPLETED;
        }

        // External power recovery detection
        if (vpsu_flag == VPSU_S_OFF && VoltLevel[IDX_VPSU] > 4600) {
            vpsu_flag = VPSU_S_ON;
            POWER_PsuOnClbk();

            // turn on battery charger
            PIN_SET(CHG_ON);
        } else if (vpsu_flag == VPSU_S_ON &&
                   VoltLevel[IDX_VPSU] < 4500) {    // Detection of the disappearance of external power
            vpsu_flag = VPSU_S_OFF;
            POWER_PsuOffClbk();

            // turn off battery charger
            PIN_RESET(CHG_ON);
        }

        // Detection of 5% battery charge
        if (charge_level_percent > 5 && (clbk_five_flag == CLBK_COMPLETED)) {
            clbk_five_flag = CLBK_EXPECTED;
        } else if (charge_level_percent <= 5) {
            if (clbk_five_flag == CLBK_EXPECTED) {
                POWER_FivePercentClbk();
                clbk_five_flag = CLBK_COMPLETED;
            }

            // Emergency power off
            if (PIN_READ(CHG_DET) == GPIO_PIN_SET) {
                // INDICATION_FivePercFlagWait();
                modeSTBY(0, NULL);
            }
        }

        if (VoltLevel[IDX_VBAT] > 3700) {
            tx_event_flags_get(&evfPower, FLAG_VBAT_1, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
        } else {
            // Battery failure detection (VBAT < 2V)
            if (VoltLevel[IDX_VBAT] < 2000 && clbk_batt_failure_flag == CLBK_EXPECTED) {
                POWER_VbatFailClbk();
                clbk_batt_failure_flag = CLBK_COMPLETED;
            }
            tx_event_flags_get(&evfPower, FLAG_VBAT_2, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
        }
    }
}

static void TimerCmpltClbk(ULONG argument) {
    timerCount++;

    if (timerCount >= TIM1_100 && timerCount < TIM2_1100) {
        // Summing the sample for averaging
        average_buf[IDX_VBAT] += adcData[IDX_VBAT];
        average_buf[IDX_VPSU] += adcData[IDX_VPSU];
        average_buf[IDX_GSM] += adcData[IDX_GSM];

    } else if (timerCount == TIM2_1100) {
        // VBAT_LOAD = 0
        PIN_RESET(VBAT_LOAD);

        float mcuVoltage = ADC_MAX * ADC_REFERENCE_VOLTAGE / adcData[3];

        for (int i = 0; i < 3; ++i) {
            average_buf[i] = average_buf[i] / NUM_SAMPLES_AVERAGE;
            VoltLevel[i] = 2 * (average_buf[i] * mcuVoltage) / ADC_MAX;
        }
        tx_event_flags_set(&evfPower, FLAG_MEAS_POWER, TX_OR);

    } else if (timerCount == TIM3_2500) {
        tx_event_flags_set(&evfPower, FLAG_VBAT_1, TX_OR);
    } else if (timerCount == TIM4_20000) {
        tx_event_flags_set(&evfPower, FLAG_VBAT_2, TX_OR);
    }
}

// Callback is triggered when the interrupt is triggered on pin CHG_STBY
void POWER_GPIO_STBY_Callback(void) {
    // Full battery detection
    if (PIN_READ(CHG_STBY) == GPIO_PIN_SET) {
        POWER_PartlyChargClbk();
    } else if (PIN_READ(CHG_STBY) == GPIO_PIN_RESET) {
        POWER_FullChargClbk();
    }
}

// Callback is triggered when the interrupt is triggered on pin CHG_DET
void POWER_GPIO_DET_Callback(void) {
    // Charging detection
    if (PIN_READ(CHG_DET) == GPIO_PIN_SET) {
        POWER_ChargDetOffClbk();
    } else if (PIN_READ(CHG_DET) == GPIO_PIN_RESET) {
        POWER_ChargDetOnClbk();
    }
}

// Waiting for battery charge measurement flag
void POWER_ChargeFlagWait(void) {
    tx_event_flags_get(&evfPower, FLAG_MEAS_CHARGE, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
}

// Stubs for callback functions
__weak void POWER_PsuOffClbk(void) {
    /*
     * Do smth
     */
}

__weak void POWER_PsuOnClbk(void) {
    /*
     * Do smth
     */
}

__weak void POWER_VbatFailClbk(void) {
    /*
     * Do smth
     */
}

__weak void POWER_FullChargClbk(void) {
    /*
     * Do smth
     */
}

__weak void POWER_PartlyChargClbk(void) {
    /*
     * Do smth
     */
}

__weak void POWER_ChargDetOnClbk(void) {
    /*
     * Do smth
     */
}

__weak void POWER_ChargDetOffClbk(void) {
    /*
     * Do smth
     */
}

__weak void POWER_TenPercentClbk(void) {
    /*
     * Do smth
     */
}

__weak void POWER_FivePercentClbk(void) {
    /*
     * Do smth
     */
}

__weak void POWER_ButtonOnClbk(void) {
    /*
     * Do smth
     */
}