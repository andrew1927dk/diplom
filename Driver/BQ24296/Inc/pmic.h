/**
 * @file pmic.h
 * @brief header file for interaction with PMIC
 * @date 2022-12-27
 *
 *   (c) 2022
 */

#ifndef PMIC_H
#define PMIC_H

#include <stdbool.h>
#include "platform_i2c.h"

#define BQ24296_I2C_ADD (0x6B << 1)

#define BQ24296_INPUT_SRC_CTRL_REG 0x00 /*Input Source Control Register*/
#define BQ24296_CONFIG_REG         0x01 /*Power-On Configuration Register*/
#define BQ24296_ICHG_CTRL_REG      0x02 /*Charge Current Control Register*/
#define BQ24296_PRECHG_CTRL_REG    0x03 /*Pre-Charge/Termination Current Control Register*/
#define BQ24296_VREG_CTRL_REG      0x04 /*Charge Voltage Control Register*/
#define BQ24296_TIMER_CTRL_REG     0x05 /*Charge Termination/Timer Control Register*/
#define BQ24296_BOOSTV_CTRL_REG    0x06 /*Boost Voltage/Thermal Regulation Control Register*/
#define BQ24296_MISC_CTRL_REG      0x07 /*Misc Operation Control Register*/
#define BQ24296_SYSTEM_STAT_REG    0x08 /*System Status Register*/
#define BQ24296_FAULT_STAT_REG     0x09 /*Fault Status Register*/
#define BQ24296_VENDOR_STAT_REG    0x0A /*Vender / Part / Revision Status Register*/

typedef struct {
    uint8_t iinlim : 3;
    uint8_t vindpm : 4;
    uint8_t en_hiz : 1;
} pmic_inputSrcCtrl_t;

typedef struct {
    uint8_t boost_lim : 1;
    uint8_t sys_min : 3;
    uint8_t chg_config : 1;
    uint8_t otg_config : 1;
    uint8_t wdg_reset : 1;
    uint8_t reg_reset : 1;
} pmic_powerOnCfg_t;

typedef struct {
    uint8_t force_20pct : 1;
    uint8_t bcold : 1;
    uint8_t ichg : 6;
} pmic_chgCurrentCtrl_t;

typedef struct {
    uint8_t iterm : 4;
    uint8_t iprechg : 4;
} pmic_preChgTermCurrentCtrl_t;

typedef struct {
    uint8_t vrechg : 1;
    uint8_t batlowv : 1;
    uint8_t vreg : 6;
} pmic_chgVoltageVtrl_t;

typedef struct {
    uint8_t : 1;
    uint8_t chg_timer : 2;
    uint8_t en_timer : 1;
    uint8_t watchdog : 2;
    uint8_t : 1;
    uint8_t en_term : 1;
} pmic_chgTermTimerCtrl_t;

typedef struct {
    uint8_t treg : 2;
    uint8_t bhot : 2;
    uint8_t boostv : 4;
} pmic_boostvThermalCtrl_t;

typedef struct {
    uint8_t int_mask0 : 1;
    uint8_t int_mask1 : 1;
    uint8_t res : 3;
    uint8_t batfet_disable : 1;
    uint8_t tmr2x_en : 1;
    uint8_t iindet_en : 1;
} pmic_miscCtrl_t;

typedef struct {
    uint8_t vsys_stat : 1;
    uint8_t therm_stat : 1;
    uint8_t pg_stat : 1;
    uint8_t dpm_stat : 1;
    uint8_t chrg_stat : 2;
    uint8_t vbus_stat : 2;
} pmic_sysStat_t;

typedef struct {
    uint8_t ntc_fault : 2;
    uint8_t : 1;
    uint8_t bat_fault : 1;
    uint8_t chrg_fault : 2;
    uint8_t otg_fault : 1;
    uint8_t wdg_fault : 1;
} pmic_faultStat_t;

typedef struct {
    uint8_t rev : 3;
    uint8_t : 2;
    uint8_t part_number : 3;
} pmic_vendorStat_t;

/**
 * @brief DPM Input Voltage Limit Set.
 *
 * @param i2c Pointer to device handle
 * @param val Voltage Limit(mV) = 3880mV + (val * 80mV)
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetInputVoltageLimit(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief DPM Input Current Limit Set.
 *
 * @param i2c Pointer to device handle
 * @param val Current Limit = 000 – 100 mA, 001 – 150 mA,
 *                            010 – 500 mA, 011 – 900 mA,
 *                            100 – 1 A,    101 – 1.5 A,
 *                            110 – 2 A,    111 – 3A
 *
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetInputCurrentLimitSet(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Charge Enable.
 *
 * @param i2c Pointer to device handle.
 * @param val true = enable, false = disable
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_EnableCharge(i2c_bus_t *i2c, bool val);

/**
 * @brief Reset Watchdog Timer.
 *
 * @param i2c Pointer to device handle.
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_ResetWatchdog(i2c_bus_t *i2c);

/**
 * @brief Minimum System Voltage Set.
 *
 * @param i2c Pointer to device handle.
 * @param val VSYS_Min(mV) = 3000mV + (val * 100mV)
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetMinimumSystemVoltage(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Boost Current Limit Set.
 *
 * @param i2c Pointer to device handle.
 * @param val 0 = 1A, 1 = 1.5A
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetBoostCurrentLimit(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Charge Current Set.
 *
 * @param i2c Pointer to device handle.
 * @param val ICharge(mA) = 512mA + (val * 64mA)
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetChargeCurrent(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Charge Current Set.
 *
 * @param i2c Pointer to device handle.
 * @param val ICharge(mA) = 512mA + (val * 64mA)
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetPrechargeCurrent(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Termination Current Set.
 *
 * @param i2c Pointer to device handle.
 * @param val ITermination(mA) = 128mA + (val * 128mA)
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetTerminationCurrent(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Charge Voltage Set.
 *
 * @param i2c Pointer to device handle.
 * @param val VREG(mV) = 3504mV + (val * 16mV)
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetChargeVoltage(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Battery low threshold set.
 *
 * @param i2c Pointer to device handle.
 * @param val 0: 2.8V, 1: 3.0V
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetBattLowThreshold(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Recharge Threshold Set.
 *
 * @param i2c Pointer to device handle.
 * @param val 0: 100mV, 1: 300mV
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetRechargeVoltage(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Enable Battery Termination.
 *
 * @param i2c Pointer to device handle.
 * @param val true = enable, false = disable
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_EnableTermination(i2c_bus_t *i2c, bool val);

/**
 * @brief Watchdog Timer Period Set
 *
 * @param i2c Pointer to device handle.
 * @param val 00 – Disable timer, 01 – 40 s, 10 – 80 s, 11 – 160 s
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetWatchdogTimer(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Enable Safety Timer.
 *
 * @param i2c Pointer to device handle.
 * @param val true = enable, false = disable
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_EnableSafetyTimer(i2c_bus_t *i2c, bool val);

/**
 * @brief Safety Timer Period Set
 *
 * @param i2c Pointer to device handle.
 * @param val 00 – 5 hrs, 01 – 8 hrs, 10 – 12 hrs, 11 – 20 hrs
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetChargeTimer(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Boost Voltage Set.
 *
 * @param i2c Pointer to device handle.
 * @param val Vboost(mV) = 4550mV + (val * 64mV)
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetBoostVoltage(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Boost Temperature Monitor Set.
 *
 * @param i2c Pointer to device handle.
 * @param val 00 – Vbhot1 (33% of REGN or 55°C w/ 103AT thermistor)
              01 – Vbhot0 (36% of REGN or 60°C w/ 103AT thermistor)
              10 – Vbhot2 (30% of REGN or 65°C w/ 103AT thermistor)
              11 – Disable boost mode thermal protection
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetBoostTempMonitor(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Thermal Regulation Threshold Set.
 *
 * @param i2c Pointer to device handle.
 * @param val 00 – 60°C, 01 – 80°C, 10 – 100°C, 11 – 120°C
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_SetThermalRegulationThreshold(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Enable Interrupt on Battery Fault.
 *
 * @param i2c Pointer to device handle.
 * @param val true = enable, false = disable
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_EnableBattFaultInterrupt(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Enable Interrupt on Charge Fault.
 *
 * @param i2c Pointer to device handle.
 * @param val true = enable, false = disable
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_EnableChgFaultInterrupt(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief BATFET turn off.
 *
 * @param i2c Pointer to device handle.
 * @param val true = disable, false = enable
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_DisableBatfet(i2c_bus_t *i2c, uint8_t val);

/**
 * @brief Read a register into a variable
 *
 * @param i2c Pointer to device handle.
 * @param reg Register to read (what we read)
 * @param val Variable to write (where we write)
 * @return i2c_status_t (0) if OK, otherwise error
 */
i2c_status_t PMIC_GetRegister(i2c_bus_t *i2c, uint8_t reg, void *val);

#endif /* PMIC_H */