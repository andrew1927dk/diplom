/**
 ******************************************************************************
 * @file           : flash.c
 * @date           : 2022-10-06
 * @brief          : This file contains all the function prototypes for
 *                 : flash.c file.
 ******************************************************************************
 */

#ifndef CORE_INC_FLASH_H_
#define CORE_INC_FLASH_H_

/* Includes */
#include "is25l.h"
#include <stdbool.h>

/**
 * @defgroup FLASH blocks
 * @brief Block bits defines for block protection
 * @{
 */

#define FLASH_BLNONE         0
#define FLASH_BL63           (1U << 2)
#define FLASH_BL62to63       (2U << 2)
#define FLASH_BL60to63       (3U << 2)
#define FLASH_BL56to63       (4U << 2)
#define FLASH_BL48to63       (5U << 2)
#define FLASH_BL32to63       (6U << 2)
#define FLASH_BLALL          (8U << 2)
#define FLASH_BL0to31        (9U << 2)
#define FLASH_BL0to15        (10U << 2)
#define FLASH_BL0to7         (11U << 2)
#define FLASH_BL0to3         (12U << 2)
#define FLASH_BL0to2         (13U << 2)
#define FLASH_BL0            (14U << 2)
#define FLASH_SAVEBITS       0x3C
#define FLASH_CLEANBLPROTECT 0x43

/**@}*/

/**
 * @enum flash_state_t
 * @brief Status enum to be used as return values.
 */
typedef enum {
    FLASH_OK,           //< Chip OK - Execution fine
    FLASH_PARAM_ERR,    //< Function parameters error
    FLASH_BUSY,         //< Chip busy
    FLASH_CHIP_ERR,     //< Chip error
    FLASH_SPI_ERR,      //< SPI Bus err
    FLASH_TX_ERR
} flash_state_t;    // Usless items will be deleted after writing all the functions

/**
 * @struct flash_IDinfo_t
 * @brief Struct for remember info from Read_ID
 * @attention Will be static if only flash.c file need it
 */
typedef struct {
    uint8_t Manufacturer;
    uint8_t Type;
    uint8_t Capacity;
} flash_IDinfo_t;

/**
 * @brief Function for initializing flash
 **/
flash_state_t FLASH_Init(void);

/**
 * @brief Function for deinitializing flash
 **/
flash_state_t FLASH_Deinit(void);

/**
 * @brief Function for write smth on chip
 * @param wrBuffer : Buffer for write data
 * @param startAddr : Read start address
 * @param datLen : Length of data to be written
 */
flash_state_t FLASH_Write(uint8_t *wrBuffer, uint32_t startAddr, uint16_t datLen);

/**
 * @brief Function for read smth from chip
 * @param rdBuffer : Buffer for read data
 * @param startAddr : Read start address
 * @param datLen : Length of data to be read
 */
flash_state_t FLASH_Read(uint8_t *rdBuffer, uint32_t startAddr, uint16_t datLen);

/**
 * @brief Function for erase smth from chip
 * @param startAddr : Read start address
 * @param datLen : Length of data to be erased
 */
flash_state_t FLASH_Erase(uint32_t startAddr, uint16_t datLen);

/**
 * @brief Function for erase all from chip
 */
flash_state_t FLASH_Erase_Chip(void);

/**
 * @brief Function for check block status
 */
uint8_t FLASH_CheckProtectionStatus(void);

/**
 * @brief Function for change block protection
 * @param rdBuffer : Buffer for write blocks to change protection status
 */
flash_state_t FLASH_ChangeProtectionStatus(uint8_t blockBuff);

#endif /* CORE_INC_FLASH_H_ */
