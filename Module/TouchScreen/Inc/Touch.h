/**
 ***********************************************************************************
 * @file           : Touch.h
 * @date           : 2022-27-12
 * @brief          : This file contains all tall the function prototypes for
 *                 : Touch.c file.
 ***********************************************************************************
 */

#ifndef CORE_INC_TOUCH_H_
#define CORE_INC_TOUCH_H_

#include "FT6x36.h"

#define FT6x36_MAX_WIDTH  ((uint16_t) 320) /* Touchscreen pad max width   */
#define FT6x36_MAX_HEIGHT ((uint16_t) 240) /* Touchscreen pad max height  */

#define TOUCH_NO_IRQ_PENDING ((uint8_t) 0)
#define TOUCH_IRQ_PENDING    ((uint8_t) 1)

#define TOUCH_SWAP_NONE ((uint8_t) 0x01)
#define TOUCH_SWAP_X    ((uint8_t) 0x02)
#define TOUCH_SWAP_Y    ((uint8_t) 0x04)
#define TOUCH_SWAP_XY   ((uint8_t) 0x08)

/**
 *  @brief touch_state_t
 *  Define TOUCH State structure
 */
typedef struct {
    uint8_t  touchDetected; /*!< Total number of active touches detected at last scan */
    uint16_t touchX;        /*!< Touch X[0], X[1] coordinates on 12 bits */
    uint16_t touchY;        /*!< Touch Y[0], Y[1] coordinates on 12 bits */
} touch_state_t;

/**
 *  @brief TOUCH_StatusTypeDef
 *  Define TOUCH_xxx() functions possible return value,
 *  when status is returned by those functions.
 */
typedef enum {
    TOUCH_OK = 0x00,              /*!< Touch Ok */
    TOUCH_ERROR = 0x01,           /*!< Touch Error */
    TOUCH_TIMEOUT = 0x02,         /*!< Touch Timeout */
    TOUCH_DEVICE_NOT_FOUND = 0x03 /*!< Touchscreen device not found */
} touch_status_t;

/**
 * @brief  Touch init function.
 * @param  i2c_read: Pointer to I2C driver struct.
 * */
touch_status_t TOUCH_Init(i2c_bus_t *i2c_read);

/**
 * @brief  Returns status and positions of the touch screen.
 * @param  i2c_read: Pointer to I2C driver struct.
 * @param  TOUCH_State: Pointer to touch screen current state structure
 * @retval TOUCH_OK if all initializations are OK. Other value if error.
 */
uint8_t TOUCH_GetState(i2c_bus_t *i2, touch_state_t *TOUCH_State);

/**
 * @brief  Function used to reset all touch data before a new acquisition
 *         of touch information.
 * @param  TOUCH_State: Pointer to touch screen current state structure
 * @retval TOUCH_OK if OK, TE_ERROR if problem found.
 */
uint8_t TOUCH_ResetTouchData(touch_state_t *TOUCH_State);
#endif    // CORE_INC_TOUCH_H_
