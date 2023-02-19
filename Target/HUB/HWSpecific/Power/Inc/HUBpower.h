/**
 * @file HUBpower.h
 * @brief Power management
 * @version 0.1
 * @date 2022-12-22
 *
 *   (c) 2022
 */

#ifndef HUBPOWER_H
#define HUBPOWER_H

#include <stdint.h>

// Array of voltages on components
extern volatile uint32_t VoltLevel[3];

// Battery charge percentage
extern uint32_t charge_level_percent;

// Data type for indices of global array with voltages
typedef enum {
    IDX_VBAT,
    IDX_VPSU,
    IDX_GSM,
    IDX_VREF
} voltageLevel_idx_t;

// Data type for validating power management
typedef enum {
    POWER_S_OK,
    POWER_S_ERROR
} power_state_t;

/**
 * @brief Initialize power system
 *
 * @return power_state_t
 */
power_state_t POWER_Init(void *);

/**
 * @brief Deinitialize power system
 *
 * @return power_state_t
 */
power_state_t POWER_Deinit(void);

/**
 * @brief Waiting for battery charge measurement flag
 *
 * @return void
 */
void POWER_ChargeFlagWait(void);

/**
 * @brief Callback is triggered when the external power supply disappears
 *
 * @return void
 */
void POWER_PsuOffClbk(void);

/**
 * @brief Сallback is triggered when external power is applied
 *
 * @return void
 */
void POWER_PsuOnClbk(void);

/**
 * @brief Сallback is triggered in the event of a battery failure (VBAT < 2.0 V)
 *
 * @return void
 */
void POWER_VbatFailClbk(void);

/**
 * @brief Callback is triggered when the battery charge level is 100%
 *
 * @return void
 */
void POWER_FullChargClbk(void);

/**
 * @brief Callback is triggered when battery is not fully charged
 *
 * @return void
 */
void POWER_PartlyChargClbk(void);

/**
 * @brief Callback is triggered when battery charging is on
 *
 * @return void
 */
void POWER_ChargDetOnClbk(void);

/**
 * @brief Callback is triggered when battery charging is off
 *
 * @return void
 */
void POWER_ChargDetOffClbk(void);

/**
 * @brief Callback is triggered when the battery charge level is 10%
 *
 * @return void
 */
void POWER_TenPercentClbk(void);

/**
 * @brief Callback is triggered when the battery charge level is 5%
 *
 * @return void
 */
void POWER_FivePercentClbk(void);

/**
 * @brief Callback is triggered when power button is pressed
 *
 * @return void
 */
void POWER_ButtonOnClbk(void);

/**
 * @brief Callback is triggered when the interrupt is triggered on pin CHG_STBY
 *
 * @return void
 */
void POWER_GPIO_STBY_Callback(void);

/**
 * @brief Callback is triggered when the interrupt is triggered on pin CHG_DET
 *
 * @return void
 */
void POWER_GPIO_DET_Callback(void);

#endif
