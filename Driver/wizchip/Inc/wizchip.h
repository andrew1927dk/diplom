/**
 * @file wizchip.h
 * @brief A wiznet chip header
 * @version 0.1
 * @date 2022-10-12
 *
 *  (c) 2022
 */
#ifndef _WIZCHIP_H_
#define _WIZCHIP_H_

#include "main.h"
#include "wizchip_conf.h"

typedef enum {
    WIZ_OK,
    WIZ_ERROR,
} wiz_status_t;

/**
 * @brief SPI transmit/receive callback
 * @return None
 */
void wizchipTxRxCpltCallback(void);

/**
 * @brief wiznet adapter chip initialization
 * @return None
 */
wiz_status_t wizChipInit(void *memoryPoolPtr);

#endif    // _WIZCHIP_H_
