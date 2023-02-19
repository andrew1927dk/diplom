/**
 ***********************************************************************************
 * @file           : Touch.c
 * @date           : 2022-27-12
 * @brief          : This file provides code for the configuration
 *                 : of the touch instances.
 ***********************************************************************************
 */

#include "Touch.h"
#define LOG_DEFAULT_MODULE LOG_M_TOUCH
#include "loglib.h"

#define TOUCH_MIN_PIXEL_DIFF 5

/* Touch init function */
touch_status_t TOUCH_Init(i2c_bus_t *i2c_read) {
    uint8_t        data = FT6x36_DEV_MODE_WORKING;
    touch_status_t status = TOUCH_OK;
    i2c_status_t   i2c_status = I2C_OK;
    i2c_status = I2C_Write_IT(i2c_read, TOUCH_DEVICE_ADDRESS, FT6x36_REG_DEV_MODE, &data, 1);
    switch (i2c_status) {
        case I2C_ERR_GENERAL:
            status = TOUCH_DEVICE_NOT_FOUND;
            break;
        case I2C_ERR_TIMEOUT:
            status = TOUCH_TIMEOUT;
            break;
        case I2C_ERR_NOT_INITED:
            status = TOUCH_ERROR;
            break;
        case I2C_ERR_MEMORY:
            status = TOUCH_ERROR;
            break;
        default:
            break;
    }
    return status;
}

/* Returns status and positions of the touch screen */
uint8_t TOUCH_GetState(i2c_bus_t *i2c_read, touch_state_t *TOUCH_State) {
    static uint32_t _x = 0;
    static uint32_t _y = 0;
    uint8_t         TOUCH_status = TOUCH_OK;
    uint16_t        Raw_x;
    uint16_t        Raw_y;
    uint16_t        xDiff;
    uint16_t        yDiff;

    TOUCH_State->touchDetected = FT6x36_DetectTouch(i2c_read);
    if (TOUCH_State->touchDetected) {

        FT6x36_GetXY(i2c_read, &(Raw_x), &(Raw_y));

        xDiff = Raw_x > _x ? (Raw_x - _x) : (_x - Raw_x);
        yDiff = Raw_y > _y ? (Raw_y - _y) : (_y - Raw_y);

        if ((xDiff + yDiff) > TOUCH_MIN_PIXEL_DIFF) {
            _x = Raw_x;
            _y = Raw_y;
        }

        TOUCH_State->touchX = _x;
        TOUCH_State->touchY = _y;
#if 0
        LOG_DEBUG("x1: %d", TOUCH_State->touchX);
        LOG_DEBUG("y1: %d", TOUCH_State->touchY);
#endif
    }
    return TOUCH_status;
}

/* Function used to reset all touch data before a new acquisition of touch information */
uint8_t TOUCH_ResetTouchData(touch_state_t *TOUCH_State) {
    uint8_t TOUCH_status = TOUCH_ERROR;

    if (TOUCH_State != (touch_state_t *) NULL) {
        TOUCH_State->touchDetected = 0;

        TOUCH_State->touchX = 0;
        TOUCH_State->touchY = 0;

        TOUCH_status = TOUCH_OK;
    }

    return TOUCH_status;
}