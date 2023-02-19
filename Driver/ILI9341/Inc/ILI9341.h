/**
 ******************************************************************************
 * @file           : ILI9341.h
 * @date           : 2022-12-21
 * @brief          : This file is a header for ILI9341.c file
 ******************************************************************************
 */

#ifndef ILI9341_H_
#define ILI9341_H_

#include <ILI9341_comlist_lvl1.h>
#include <ILI9341_comlist_lvl2.h>
#include "main.h"

#define ILI9341_SCREEN_WIDTH  240
#define ILI9341_SCREEN_HEIGHT 320

#define ILI9341_START_DATA    PIN_SET(DISP_DCX)
#define ILI9341_START_COMMAND PIN_RESET(DISP_DCX)
#define ILI9341_RST_SET       PIN_SET(DISP_RESET)
#define ILI9341_RST_RESET     PIN_RESET(DISP_RESET)
#define ILI9341_END_WRITE     PIN_SET(DISP_NSS)
#define ILI9341_START_WRITE   PIN_RESET(DISP_NSS)

/**
 * @enum ILI9341_state_t
 * @brief Status enum to be used as return values.
 */
typedef enum {
    ILI9341_OK,
    ILI9341_ERR,
    ILI9341_PARAM_ERR,
    ILI9341_TX_ERR
} ILI9341_state_t;

/**
 * @brief This function initializes ILI9341
 */
ILI9341_state_t ILI9341_init(void);

/**
 * @brief This function sets the area what is going to be redrawn
 * @param x : x cord
 * @param y : y cord
 * @param w : width
 * @param h : height
 */
void ILI9341_setWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

/**
 * @brief This function send data via SPI for ILI9341
 * @param data : Data that is going to be sent
 * @param size : Size of data
 */
void ILI9341_sendData(uint8_t *data, size_t size);

/**
 * @brief Tx Callback for ILI9341. Sets SEND flag.
 */
void ILI9341_TxCpltCallback(void);
/**
 * @brief Rx Callback for ILI9341. Sets READ flag.
 */
void ILI9341_RxCpltCallback(void);

#endif /* ILI9341_H_ */
