/**
 * @file indication.c
 * @brief Indication
 * @version 0.1
 * @date 2022-11-23
 *
 *  (c) 2022
 */

#include "FreeRTOS.h"
// #include "cmsis_os.h"
#include "power.h"
#include "LED.h"
#include "indication.h"

#define INDICATION_PSU_OFF   0x00000001U
#define INDICATION_FIVE_PERC 0x00000002U

// Data type for selecting a specific indication pattern
typedef enum {
    PATTERN_BAT_DEFAULT,
    PATTERN_BAT_FULL,
    PATTERN_BAT_GREEN,
    PATTERN_BAT_YELLOW,
    PATTERN_BAT_RED,
    PATTERN_BAT_VIOLET
} pattern_indication_t;

// Thread powerIndication initialization
static osThreadId_t         powerIndicationHandle;
static const osThreadAttr_t powerIndication_attributes = {
    .name = "powerIndication",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};
static void StartPowerIndication(void *argument);

static osEventFlagsId_t evfIndication;

static void FillPatternArray(pattern_indication_t);
static void IndicationPatternOn(pattern_indication_t);

// Indication patterns
static struct LED_task_t patternBatGreenArray[4];
static struct LED_task_t patternBatFullArray[4];
static struct LED_task_t patternBatYellowArray[7];
static struct LED_task_t patternBatRedArray[13];
static struct LED_task_t patternBatVioletArray[13];

static pattern_indication_t currPattern;
static void                *elHandler;

indication_state_t INDICATION_Init() {

    if ((powerIndicationHandle = osThreadNew(StartPowerIndication, NULL, &powerIndication_attributes)) == NULL) {
        return INDICATION_S_ERROR;
    }
    if ((evfIndication = osEventFlagsNew(NULL)) == NULL) {
        return INDICATION_S_ERROR;
    }

    FillPatternArray(PATTERN_BAT_GREEN);
    FillPatternArray(PATTERN_BAT_YELLOW);
    FillPatternArray(PATTERN_BAT_RED);
    FillPatternArray(PATTERN_BAT_VIOLET);
    FillPatternArray(PATTERN_BAT_FULL);

    return INDICATION_S_OK;
}

// General thread indication
static void StartPowerIndication(void *argument) {

    // Waiting for battery measurement flag from POWER
    POWER_ChargeFlagWait();

    // Resetting the current pattern
    currPattern = PATTERN_BAT_DEFAULT;

    while (1) {
        // Waiting if PSU is ON
        if (VoltLevel[IDX_VPSU] > 3500) {
            osEventFlagsWait(evfIndication, INDICATION_PSU_OFF, osFlagsWaitAny, osWaitForever);
        }

        if (charge_level_percent >= 50) {
            if (currPattern != PATTERN_BAT_GREEN) {
                currPattern = PATTERN_BAT_GREEN;
                IndicationPatternOn(PATTERN_BAT_GREEN);
            }
        } else if (charge_level_percent > 10) {
            if (currPattern != PATTERN_BAT_YELLOW) {
                if (currPattern != PATTERN_BAT_DEFAULT) {
                    LED_TurnOff(elHandler);
                }
                currPattern = PATTERN_BAT_YELLOW;
                IndicationPatternOn(PATTERN_BAT_YELLOW);
            }
        } else if (charge_level_percent > 5) {
            if (currPattern != PATTERN_BAT_RED) {
                if (currPattern != PATTERN_BAT_DEFAULT) {
                    LED_TurnOff(elHandler);
                }
                currPattern = PATTERN_BAT_RED;
                IndicationPatternOn(PATTERN_BAT_RED);
            }
        } else {
            if (currPattern != PATTERN_BAT_DEFAULT) {
                LED_TurnOff(elHandler);
            }
            IndicationPatternOn(PATTERN_BAT_VIOLET);
        }
        osDelay(5);
    }
}

// Activation of a specific pattern indication
static void IndicationPatternOn(pattern_indication_t PATTERN) {

    switch (PATTERN) {
        case PATTERN_BAT_GREEN:
            elHandler = LED_TurnOn(patternBatGreenArray, 4, 10);
            break;
        case PATTERN_BAT_YELLOW:
            elHandler = LED_TurnOn(patternBatYellowArray, 7, 10);
            break;
        case PATTERN_BAT_RED:
            elHandler = LED_TurnOn(patternBatRedArray, 13, 10);
            break;
        case PATTERN_BAT_VIOLET:
            elHandler = LED_TurnOn(patternBatVioletArray, 13, 10);
            // Time delay for this pattern to work (Time ON - 30 ms)
            osDelay(3470);
            // This is a one-time pattern, so it must be turned off
            LED_TurnOff(elHandler);
            osEventFlagsSet(evfIndication, INDICATION_FIVE_PERC);
            break;
        case PATTERN_BAT_FULL:
            elHandler = LED_TurnOn(patternBatFullArray, 4, 10);
            // Time delay for this pattern to work (Time ON - 30 ms)
            osDelay(2270);
            // This is a one-time pattern, so it must be turned off
            LED_TurnOff(elHandler);
            break;
        default:
            break;
    }
}

// Waiting for indication 5% flag
void INDICATION_FivePercFlagWait(void) {
    osEventFlagsWait(evfIndication, INDICATION_FIVE_PERC, osFlagsWaitAny, osWaitForever);
}

// Callback will be called when the battery is fully charged
void POWER_FullChargClbk() {
    IndicationPatternOn(PATTERN_BAT_FULL);
}

// Callback will be called when the external power supply disappears
void POWER_PsuOffClbk() {
    osEventFlagsSet(evfIndication, INDICATION_PSU_OFF);
}

// Сallback will be called when external power is applied
void POWER_PsuOnClbk() {
    LED_TurnOff(elHandler);
    // Resetting the current pattern
    currPattern = PATTERN_BAT_DEFAULT;
}

// Function with indication patterns
static void FillPatternArray(pattern_indication_t PATTERN) {
    switch (PATTERN) {
        case PATTERN_BAT_GREEN:
            // 1 pulsation
            LED_FillElement(patternBatGreenArray, 0, COLOR_GREEN, 0, 20, 100, 0, 1);
            LED_FillElement(patternBatGreenArray, 1, COLOR_GREEN, 20, 100, 20, 0, 1);
            LED_FillElement(patternBatGreenArray, 2, COLOR_GREEN, 100, 100, 10, 0, 1);
            LED_FillElement(patternBatGreenArray, 3, COLOR_GREEN, 20, 0, 100, 2770, 1);
            // dur_off = 30s - TIME_ON
            break;
        case PATTERN_BAT_YELLOW:
            // 1-st pulsation
            LED_FillElement(patternBatYellowArray, 0, COLOR_YELLOW, 0, 20, 100, 0, 1);
            LED_FillElement(patternBatYellowArray, 1, COLOR_YELLOW, 20, 100, 20, 0, 1);
            LED_FillElement(patternBatYellowArray, 2, COLOR_YELLOW, 100, 100, 10, 0, 1);
            LED_FillElement(patternBatYellowArray, 3, COLOR_YELLOW, 20, 20, 10, 0, 1);
            // 2-nd pulsation
            LED_FillElement(patternBatYellowArray, 4, COLOR_YELLOW, 20, 100, 20, 0, 1);
            LED_FillElement(patternBatYellowArray, 5, COLOR_YELLOW, 100, 100, 10, 0, 1);
            LED_FillElement(patternBatYellowArray, 6, COLOR_YELLOW, 20, 0, 100, 2730, 1);
            // dur_off = 30s - TIME_ON
            break;
        case PATTERN_BAT_RED:
            // 1-st pulstaion
            LED_FillElement(patternBatRedArray, 0, COLOR_RED, 0, 20, 100, 0, 1);
            LED_FillElement(patternBatRedArray, 1, COLOR_RED, 20, 100, 20, 0, 1);
            LED_FillElement(patternBatRedArray, 2, COLOR_RED, 100, 100, 10, 0, 1);
            LED_FillElement(patternBatRedArray, 3, COLOR_RED, 20, 20, 10, 0, 1);
            // 2-nd pulsation
            LED_FillElement(patternBatRedArray, 4, COLOR_RED, 20, 100, 20, 0, 1);
            LED_FillElement(patternBatRedArray, 5, COLOR_RED, 100, 100, 10, 0, 1);
            LED_FillElement(patternBatRedArray, 6, COLOR_RED, 20, 20, 70, 0, 1);
            // 3-rd pulsation
            LED_FillElement(patternBatRedArray, 7, COLOR_RED, 20, 100, 20, 0, 1);
            LED_FillElement(patternBatRedArray, 8, COLOR_RED, 100, 100, 10, 0, 1);
            LED_FillElement(patternBatRedArray, 9, COLOR_RED, 20, 20, 10, 0, 1);
            // 4-th pulsation
            LED_FillElement(patternBatRedArray, 10, COLOR_RED, 20, 100, 20, 0, 1);
            LED_FillElement(patternBatRedArray, 11, COLOR_RED, 100, 100, 10, 0, 1);
            LED_FillElement(patternBatRedArray, 12, COLOR_RED, 20, 0, 100, 2590, 1);
            // dur_off = 30s - TIME_ON
            break;
        case PATTERN_BAT_VIOLET:
            // 1-st pulstaion
            LED_FillElement(patternBatVioletArray, 0, COLOR_VIOLET, 0, 20, 100, 0, 1);
            LED_FillElement(patternBatVioletArray, 1, COLOR_VIOLET, 20, 100, 20, 0, 1);
            LED_FillElement(patternBatVioletArray, 2, COLOR_VIOLET, 100, 100, 10, 0, 1);
            LED_FillElement(patternBatVioletArray, 3, COLOR_VIOLET, 20, 20, 10, 0, 1);
            // 2-nd pulsation
            LED_FillElement(patternBatVioletArray, 4, COLOR_VIOLET, 20, 100, 20, 0, 1);
            LED_FillElement(patternBatVioletArray, 5, COLOR_VIOLET, 100, 100, 10, 0, 1);
            LED_FillElement(patternBatVioletArray, 6, COLOR_VIOLET, 20, 20, 10, 0, 1);
            // 3-rd pulsation
            LED_FillElement(patternBatVioletArray, 7, COLOR_VIOLET, 20, 100, 20, 0, 1);
            LED_FillElement(patternBatVioletArray, 8, COLOR_VIOLET, 100, 100, 10, 0, 1);
            LED_FillElement(patternBatVioletArray, 9, COLOR_VIOLET, 20, 20, 10, 0, 1);
            // 4-th pulsation
            LED_FillElement(patternBatVioletArray, 10, COLOR_VIOLET, 20, 100, 20, 0, 1);
            LED_FillElement(patternBatVioletArray, 11, COLOR_VIOLET, 100, 100, 10, 0, 1);
            LED_FillElement(patternBatVioletArray, 12, COLOR_VIOLET, 20, 0, 100, 0, 1);
            // dur_off = 30s - TIME_ON
            break;
        case PATTERN_BAT_FULL:
            // 1 pulsation
            LED_FillElement(patternBatFullArray, 0, COLOR_GREEN, 0, 20, 100, 0, 1);
            LED_FillElement(patternBatFullArray, 1, COLOR_GREEN, 20, 100, 20, 0, 1);
            LED_FillElement(patternBatFullArray, 2, COLOR_GREEN, 100, 100, 10, 0, 1);
            LED_FillElement(patternBatFullArray, 3, COLOR_GREEN, 20, 0, 100, 0, 1);
        default:
            break;
    }
}