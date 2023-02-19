/**
 ******************************************************************************
 * @file           : LED.h
 * @date           : 2022-09-16
 * @brief          : This file contains all the function prototypes for
 *                  the LED.c file
 ******************************************************************************
 */

#ifndef CORE_INC_LED_H_
#define CORE_INC_LED_H_

#include "main.h"

/** @brief Structure which contains color parameters */
typedef struct {
    uint16_t R;
    uint16_t G;
    uint16_t B;
} LED_RGB_t;

/** @brief Structure which contains parameters for one task element. Use it as an array */
struct LED_task_t {
    LED_RGB_t RGB;         // Array of color codes from 0 to 100
    uint8_t   br_beg;      // Begin brightness(from 0 to 100)
    uint8_t   br_end;      // End brightness(from 0 to 100)
    uint16_t  dur_on;      // Duration on: Time until the light is on
    uint16_t  dur_off;     // Duration off: Time until the light is off
    uint8_t   switches;    // Number of repeating combinations of color scheme pattern
};

/** @brief Data type for validating indication */
typedef enum {
    LED_S_OK,
    LED_S_ERROR
} led_state_t;

/** @brief Default available color palette to simplify the use of colors */

#define COLOR_RED      ((LED_RGB_t){ 100, 0, 0 })
#define COLOR_ORANGE   ((LED_RGB_t){ 100, 10, 0 })
#define COLOR_YELLOW   ((LED_RGB_t){ 100, 40, 0 })
#define COLOR_GREEN    ((LED_RGB_t){ 0, 100, 0 })
#define COLOR_BLUE     ((LED_RGB_t){ 0, 100, 100 })
#define COLOR_INDIGO   ((LED_RGB_t){ 0, 0, 100 })
#define COLOR_LAVANDER ((LED_RGB_t){ 90, 20, 60 })
#define COLOR_VIOLET   ((LED_RGB_t){ 30, 0, 35 })

/**
 * @brief Function for for initializing the indication pins
 * @param memoryPoolPtr Pointer to the existing byte pool
 */
led_state_t LED_Init(void *memoryPoolPtr);

/** @brief Function for for deinitializing the indication pins */
void LED_Deinit();

/**
 * @brief Main function in a LED module for control indication
 * @param indicArray Array with color combinations and settings
 * @param arraySize Size of indicArray
 * @param priority Priority of task. Do not set higher than 9(I guess it will be changed in future)
 *
 * @retval Handler to queue element. NULL if queue if full or if NULL passed as the first parameter
 */
void *LED_TurnOn(const struct LED_task_t *indicArray, uint8_t arraySize, uint8_t priority);

/**
 * @brief function to terminate previously activated task
 * @param indicQueueElem Handler to queue element
 */
void LED_TurnOff(void *indicQueueElem);

/**
 * @brief Function for setting parameters for one task element. You can still do it manually without using this
 * function
 *
 *  @param array Task array
 *  @param elementIndex Array index to fill or change
 *  @param RGB Default or previously set by developer array of color codes from 0 to 100
 *  @param br_beg Begin brightness: Defines the initial degree of brightness of the glow (from 0 to 100)
 *  @param br_end br_end End brightness: Defines the final degree of brightness of the glow (from 0 to 100)
 *  @param dur_on Duration on: Time until the light is on
 *  @param dur_off Duration off: Time until the light is off
 *  @param switches Number of repeating combinations of color scheme pattern
 */
void LED_FillElement(struct LED_task_t *array, uint8_t elementIndex, LED_RGB_t RGB, uint8_t br_beg, uint8_t br_end,
                     uint16_t dur_on, uint16_t dur_off, uint8_t switches);

#endif /* CORE_INC_LED_H_ */
