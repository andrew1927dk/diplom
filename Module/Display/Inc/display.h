/**
 ******************************************************************************
 * @file           : display.h
 * @date           : 2022-12-21
 * @brief          :
 ******************************************************************************
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "ILI9341.h"
#include "Touch.h"
#include "lvgl.h"
#include "demos/lv_demos.h"

/**
 * @enum DISPLAY_state_t
 * @brief Status enum to be used as return values.
 */
typedef enum {
    DISPLAY_OK,
    DISPLAY_DRIVER_ERR,
    DISPLAY_TX_ERR,
} DISPLAY_state_t;

/**
 * @brief This function initializes ILI9341 and LVGL
 * @param memoryPoolPtr : ThreadX pool memory pointer
 * @param  i2c_read: Pointer to I2C driver struct.
 */
DISPLAY_state_t DISPLAY_Init(void *memoryPoolPtr, i2c_bus_t *i2c_read);

/**
 * @brief Lock Display mutex
 */
void DISPLAY_Lock(void);

/**
 * @brief Unlock Display mutex
 */
void DISPLAY_Unlock(void);

#endif /* DISPLAY_H_ */