/**
 ***********************************************************************************
 * @file           : is25l.h
 * @date           : 2022-09-27
 * @brief          : This file contains all the function prototypes for
 *                 : IS25L.c file
 ***********************************************************************************
 */

#ifndef CORE_INC_IS25L_H_
#define CORE_INC_IS25L_H_

#include <stdint.h>
#include "is25l_registers.h"
#include "tx_api.h"

/* Edge of the flash memory */
#define IS25L_EDGE 0x3FFFFF

/**
 * @defgroup IS25L Chip Specifications
 * @brief User's chip parameters
 * @{
 */

/* Memory IS25L size (MB-bit) */
#define IS25L_MEMORY_SIZE 32U
/* Memory big block size (KB) */
#define IS25L_MEMORY_BBLOCK_SIZE 64U
/* Memory small block size (KB) */
#define IS25L_MEMORY_SBLOCK_SIZE 32U
/* Memory sector size (KB) */
#define IS25L_MEMORY_SECTOR_SIZE 4U
/* Memory page size (Bytes) */
#define IS25L_MEMORY_PAGE_SIZE 256U
/* Blocks count (64 Blocks) */
#define IS25L_BLOCK_COUNT 64U
/* Sector count (1024 Sectors) */
#define IS25L_SECTOR_COUNT (IS25L_BLOCK_COUNT * 16)
/* Pages count (16384 Pages) */
#define IS25L_PAGE_COUNT (IS25L_SECTOR_COUNT * 16)

/**@}*/

/**
 * @defgroup IS25L Flags
 * @brief Flag definitions
 * @{
 */

/* Flag for command completion */
#define IS25L_FLAG_CMD_CPLT 1U
/* Flag for TX completion */
#define IS25L_FLAG_TX_CPLT 2U
/* Flag for RX completion */
#define IS25L_FLAG_RX_CPLT 4U
/* Flag for operation comlpetion */
#define IS25L_FLAG_STATUS_CPLT 8U

/**@}*/

/**
 * @defgroup IS25L Register bits
 * @brief Status Register Bit Definitions
 * @{
 */

/* Write in progress bit */
#define IS25L_STATUS_WIP 0x01
/* Write enable latch bit */
#define IS25L_STATUS_WEL 0x02
/* Quad enable bit */
#define IS25L_STATUS_QE 0x40

/**@}*/

/**
 *
 * @enum is25l_state_t
 * @brief Status enum to be used as return values.
 */
typedef enum {
    IS25L_OK,             // Chip OK - Execution fine
    IS25L_BUSY,           // Chip busy
    IS25L_PARAM_ERR,      // Function parameters error
    IS25L_CHIP_ERR,       // Chip error
    IS25L_SPI_ERR,        // SPI Bus err
    IS25L_WREN_ERR,       // WREN Operation Error
    IS25L_CHIP_IGNORE,    // Chip ignore state
    IS25L_TX_ERR
} is25l_state_t;

/**
 * @brief Function for initializing IS25L
 **/
is25l_state_t IS25L_Init(void);

/**
 * @brief Function for initializing IS25L
 **/
is25l_state_t IS25L_Deinit(void);

/**
 * @brief Function for converting page number into page address
 * @param PageNumber : Number of the preffered page
 * @param PageShift : Byte shift on page (0-255)
 **/
uint32_t PageToAddress(uint32_t PageNumber, uint8_t PageShift);

/**
 * @brief Function for converting page number into page address
 * @param SectorNumber : Number of the preffered sector
 **/
uint32_t SectorToAddress(uint32_t SectorNumber);

/**
 * @brief Function for reading a product identification by JEDEC ID operation RDJDID, 9Fh
 * @param ID_buffer : Buffer for recieved address
 **/
is25l_state_t IS25L_ReadID(uint8_t *ID_buffer);

/**
 * @brief Function for reading status register
 * @param reg_buffer : Buffer for receiving a 1 byte
 **/
is25l_state_t IS25L_ReadStatusReg(uint8_t *reg_buffer);

/**
 * @brief Function for writing status register
 * @param reg_buffer : Buffer for receiving a 1 byte
 **/
is25l_state_t IS25L_WriteStatusReg(uint8_t reg_buffer);

/**
 * @brief Function for erasing sector
 * @param RAW_SectorAddress : Sector address (hex)
 * @note This function requires the start address of selected sector
 **/
is25l_state_t IS25L_EraseSector(uint32_t RAW_SectorAddress);

/**
 * @brief Function for erasing the whole chip
 **/
is25l_state_t IS25L_EraseChip(void);

/**
 * @brief Implementation of the FRQIO instruction which allows the address bits to be input four bits at a time
 * @param FR_Dat_Ptr : Pointer to data to be read
 * @param Addr_start : Read start address
 * @param data_size : Size of data to read
 **/
is25l_state_t IS25L_FastReadQUADOperation(uint8_t *FR_Dat_Ptr, uint32_t Addr_start, uint16_t data_size);

/**
 * @brief Write 8-bit data (1 byte per 2 clocks, 4 bits per 1 clock) to address
 * @note Address is in [byte] size
 * @param buffer: Contains data that is going to be written
 * @param RAW_PageAddress : Raw address of the page
 * @param data_lenght : Lenght of data that is going to be written (0 - 256)
 **/
is25l_state_t IS25L_WritePPQ(uint8_t *buffer, uint32_t RAW_PageAddress, uint16_t data_lenght);

/**
 * @brief IS25L Status Callback
 */
void IS25L_StatusMatchCallback(void);

/**
 * @brief IS25L Command Callback
 */
void IS25L_CmdCompleteCallback(void);

/**
 * @brief IS25L TX Callback
 */
void IS25L_TxCompleteCallback(void);

/**
 * @brief IS25L RX Callback
 */
void IS25L_RxCompleteCallback(void);

#endif /* CORE_INC_IS25L_H_ */
