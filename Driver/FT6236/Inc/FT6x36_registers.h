/**
 ***********************************************************************************
 * @file           : FT6x36_registers.h
 * @date           : 2022-16-12
 * @brief          : This file contains the working mode register map.
 ***********************************************************************************
 */

#ifndef CORE_INC_FT6x36_REGISTERS_H_
#define CORE_INC_FT6x36_REGISTERS_H_

#include <stdint.h>
#include "main.h"

#define FT6x36_I2C_ADDR             (0x38 << 1)    //
#define FT6x36_MAX_DETECTABLE_TOUCH 1              //
#define FT6x36_REG_DEV_MODE         0x00           //
#define FT6x36_DEV_MODE_FACTORY     0x40           //
#define FT6x36_DEV_MODE_WORKING     0x00           //

/* Touch Data Status register : gives number of active touch points */
#define FT6x36_REG_TD_STATUS 0x02    //
#define FT6x36_TD_STAT_MASK  0x0F    //
#define FT6x36_TD_STAT_SHIFT 0x00    //

/* Values for reading coords */
#define FT6x36_REG_P1_XH 0x03    //
#define FT6x36_MSB_MASK  0x0F    //
#define FT6x36_MSB_SHIFT 0       //
#define FT6x36_LSB_MASK  0xFF    //
#define FT6x36_LSB_SHIFT 0       //

#endif