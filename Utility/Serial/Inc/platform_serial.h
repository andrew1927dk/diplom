/**
 * @file platform_serial.h
 * @brief
 * @version 0.1
 * @date 2023-01-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef PLATFORM_SERIAL_H
#define PLATFORM_SERIAL_H

#include "main.h"
#include "usart.h"

/**
 * @brief
 */
typedef enum {
    SERIAL_OK,
    SERIAL_ERR_GENERAL,
    SERIAL_ERR_BUSY,
    SERIAL_ERR_TIMEOUT,
    SERIAL_ERR_MEMORY,
    SERIAL_ERR_ALLREADY_INITED,
    SERIAL_ERR_NOT_INITED,
} serial_status_t;

#endif    // PLATFORM_SERIAL_H
