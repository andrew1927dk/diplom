/**
 ***********************************************************************************
 * @file           : FT6x36.h
 * @date           : 2022-26-12
 * @brief          : This file contains all the description of the FT6x36 touch screen.
 ***********************************************************************************
 */

#ifndef CORE_INC_FT6x36_H_
#define CORE_INC_FT6x36_H_

#include <stdint.h>
#include "FT6x36_registers.h"
#include "platform_i2c.h"

#define TOUCH_DEVICE_ADDRESS FT6x36_I2C_ADDR

typedef struct {
    uint8_t touchesBeforeNumb;
    uint8_t touchesCurrentNumb;

} ft6x36_touches_t;

/**
 * @brief  Return if there is touches detected or not.
 *         Try to detect new touches and forget the old ones (reset internal global
 *         variables).
 * @param  i2c_read: Pointer to I2C driver struct.
 * @retval : Number of active touches detected (can be 0 or 1).
 */
uint8_t FT6x36_DetectTouch(i2c_bus_t *i2c_read);

/**
 * @brief  Get the touch screen X and Y positions values
 *         Manage multi touch thanks to touch Index global
 *         variable 'FT6x36_handle.currActiveTouchIdx'.
 * @param  DeviceAddr: Device address on communication Bus.
 * @param  X: Pointer to X position value
 * @param  Y: Pointer to Y position value
 */
void FT6x36_GetXY(i2c_bus_t *i2c_read, uint16_t *X, uint16_t *Y);

#endif