/**
 ******************************************************************************
 * @file           : flash.c
 * @date           : 2022-10-12
 * @brief          : This file provides code for the configuration
 *                   of the flash instances.
 ******************************************************************************
 */

/* Includes */
#include "flash.h"
#define LOG_DEFAULT_MODULE LOG_M_FLASH
#include "loglib.h"

static flash_IDinfo_t ID = { 0x00, 0x00, 0x00 };

/* Mutex */
static TX_MUTEX muxFLASH;

/* Function for init flash */
flash_state_t FLASH_Init(void) {
    flash_state_t state = FLASH_OK;

    if (tx_mutex_create(&muxFLASH, "FLASH Common Mutex", TX_NO_INHERIT) != TX_SUCCESS) {
        return IS25L_TX_ERR;
    }

    tx_mutex_get(&muxFLASH, TX_WAIT_FOREVER);

    if (IS25L_Init() != IS25L_OK) {
        state = FLASH_CHIP_ERR;
        goto quit;
    }

    uint8_t Buf_ID[3];

    if (IS25L_ReadID(Buf_ID) != IS25L_OK) {
        state = FLASH_CHIP_ERR;
        goto quit;
    }

    ID.Manufacturer = Buf_ID[0];
    ID.Type = Buf_ID[1];

    if (Buf_ID[2] == 0x16) {
        ID.Capacity = IS25L_MEMORY_SIZE;
    } else {
        state = FLASH_CHIP_ERR;
    }
quit:
    tx_mutex_put(&muxFLASH);
    return state;
}

/* Function for Deinit flash */
flash_state_t FLASH_Deinit(void) {
    tx_mutex_get(&muxFLASH, TX_WAIT_FOREVER);

    if (IS25L_Deinit() != IS25L_OK) {
        return FLASH_CHIP_ERR;
    }

    tx_mutex_put(&muxFLASH);

    if (tx_mutex_delete(&muxFLASH) != TX_SUCCESS) {
        return FLASH_TX_ERR;
    }

    return FLASH_OK;
}

/* Function for information read */
flash_state_t FLASH_Read(uint8_t *rdBuffer, uint32_t startAddr, uint16_t datLen) {

    tx_mutex_get(&muxFLASH, TX_WAIT_FOREVER);
    if (IS25L_FastReadQUADOperation(rdBuffer, startAddr, datLen) != IS25L_OK) {
        return FLASH_CHIP_ERR;
    }
    tx_mutex_put(&muxFLASH);
    return FLASH_OK;
}

/* Function for information write */
flash_state_t FLASH_Write(uint8_t *wrBuffer, uint32_t startAddr, uint16_t datLen) {
    flash_state_t state = FLASH_SPI_ERR;

    tx_mutex_get(&muxFLASH, TX_WAIT_FOREVER);

    /* Counter: how many bytes there is to write */
    uint16_t bytesCounter = datLen;
    /* Current address of the flash memory */
    uint32_t currentAddr = startAddr;
    /* Counter: queue for how many bytes will be written during iteration */
    uint16_t currentDataLenght = 0;

    while (1) {
        /* Calculate shift on page */
        uint8_t comByteShift = currentAddr % IS25L_MEMORY_PAGE_SIZE;

        /* Check how much space is left on page */
        if (((uint16_t) comByteShift + bytesCounter) > IS25L_MEMORY_PAGE_SIZE) {
            /* If info to be written exceeds page limit, then info is trimmed */
            currentDataLenght = IS25L_MEMORY_PAGE_SIZE - comByteShift;
        } else {
            /* If page limit is not exceded, then write all bytes */
            currentDataLenght = bytesCounter;
        }

        /* Perform write to flash memory. */
        if (IS25L_WritePPQ(wrBuffer, currentAddr, currentDataLenght) != IS25L_OK) {
            LOG_DEBUG("%s, %d", __FILE__, __LINE__);
            goto quit;
        }

        /* Move the pointer for buffer after write */
        wrBuffer = wrBuffer + currentDataLenght;
        /* Memorize flash address after write */
        currentAddr = currentAddr + currentDataLenght;
        /* Memorize how many bytes left to write */
        bytesCounter = bytesCounter - currentDataLenght;

        if (bytesCounter == 0) {
            break;
        }
    }

    state = FLASH_OK;

quit:
    tx_mutex_put(&muxFLASH);
    return state;
}

/* Function for full Flash erase */
flash_state_t FLASH_Erase_Chip(void) {
    tx_mutex_get(&muxFLASH, TX_WAIT_FOREVER);
    if (IS25L_EraseChip() != IS25L_OK) {
        return FLASH_CHIP_ERR;
    }
    tx_mutex_put(&muxFLASH);
    return FLASH_OK;
}

/* Buffer to store the sector data */
static uint8_t EraseBUF[4096];
/* Function for selective Flash erase */
flash_state_t FLASH_Erase(uint32_t startAddr, uint16_t datLen) {
    /* Return error in case user erases data by address beyond flash's capacity */
    if ((startAddr + (uint32_t) datLen) > IS25L_EDGE) {
        return FLASH_PARAM_ERR;
    }
    tx_mutex_get(&muxFLASH, TX_WAIT_FOREVER);
    /* Get current sector number */
    uint32_t sectorNumber = startAddr / (IS25L_MEMORY_SECTOR_SIZE * 1024U);
    /* Get current sector address */
    uint32_t sectorAddress = SectorToAddress(sectorNumber);
    /* Current address of the flash memory */
    uint32_t currentAddr = startAddr;

    /* Get buffer element number relative to sector address */
    uint16_t bufElementNumber = startAddr % (IS25L_MEMORY_SECTOR_SIZE * 1024U);
    /* Counter: how many bytes there is to erase */
    uint16_t bytesCounter = datLen;
    /* Counter: queue for how many bytes will be erased during iteration */
    uint16_t currentDataLenght = 0;
    /* Flag to indicate if area for erase crosses sector's border */
    uint8_t flagSequence = 0;

    while (1) {
        /* Check if area of erase crosses sector's border */
        if (((currentAddr % (IS25L_MEMORY_SECTOR_SIZE * 1024U)) + bytesCounter) > (IS25L_MEMORY_SECTOR_SIZE * 1024U)) {
            flagSequence = 1;
            currentDataLenght = (IS25L_MEMORY_SECTOR_SIZE * 1024U) - bufElementNumber;
        } else {
            currentDataLenght = bytesCounter;
        }

        /* Read sector to buffer */
        FLASH_Read(EraseBUF, sectorAddress, sizeof(EraseBUF));

        /* Erase (255 = empty by flash logic) chosen data */
        for (int i = 0; i < currentDataLenght; i++) {
            EraseBUF[bufElementNumber] = 255;
            bufElementNumber++;
        }

        /* Erase sector in Flash memory */
        IS25L_EraseSector(sectorAddress);
        /* Perform write to flash memory. */
        FLASH_Write(EraseBUF, sectorAddress, sizeof(EraseBUF));

        if (flagSequence == 1) {
            /* Memorize flash address after operation */
            currentAddr = currentAddr + currentDataLenght;
            /* Memorize how many bytes is left to erase */
            bytesCounter = bytesCounter - currentDataLenght;
            /* Change current sector address to the next one */
            sectorAddress = sectorAddress + (IS25L_MEMORY_SECTOR_SIZE * 1024U);
            /* Reset this counter because next data will be erased at the start of the sector */
            bufElementNumber = 0;
            /* Reset flag at the end of every iteration (if) in case if next iter will be final */
            flagSequence = 0;
        } else {
            break;
        }
    }

    tx_mutex_put(&muxFLASH);
    return FLASH_OK;
}

/* Check protection status based on registry state */
uint8_t FLASH_CheckProtectionStatus(void) {
    uint8_t regist_buf_r;

    IS25L_ReadStatusReg(&regist_buf_r);
    regist_buf_r &= FLASH_SAVEBITS;

    switch (regist_buf_r) {
        case FLASH_BL63:
            LOG_INFO("%s \r", "Block 63 protected");
            break;
        case FLASH_BL62to63:
            LOG_INFO("%s \r", "Blocks 62-63 protected");
            break;
        case FLASH_BL60to63:
            LOG_INFO("%s \r", "Blocks 60-63 protected");
            break;
        case FLASH_BL56to63:
            LOG_INFO("%s \r", "Blocks 56-63 protected");
            break;
        case FLASH_BL48to63:
            LOG_INFO("%s \r", "Blocks 48-63 protected");
            break;
        case FLASH_BL32to63:
            LOG_INFO("%s \r", "Blocks 32-63 protected");
            break;
        case FLASH_BLALL:
            LOG_INFO("%s \r", "All blocks protected");
            break;
        case FLASH_BL0to31:
            LOG_INFO("%s \r", "Blocks 0-31 protected");
            break;
        case FLASH_BL0to15:
            LOG_INFO("%s \r", "Blocks 0-15 protected");
            break;
        case FLASH_BL0to7:
            LOG_INFO("%s \r", "Blocks 0-7 protected");
            break;
        case FLASH_BL0to3:
            LOG_INFO("%s \r", "Blocks 0-3 protected");
            break;
        case FLASH_BL0to2:
            LOG_INFO("%s \r", "Blocks 0-2 protected");
            break;
        case FLASH_BL0:
            LOG_INFO("%s \r", "Block 0 protected");
            break;
        default:
            LOG_INFO("%s \r", "No blocks protected");
            break;
    }
    return regist_buf_r;
}

/* Change block protection status */
flash_state_t FLASH_ChangeProtectionStatus(uint8_t blockBuff) {

    uint8_t regist_buf;

    IS25L_ReadStatusReg(&regist_buf);

    regist_buf &= FLASH_CLEANBLPROTECT;
    regist_buf = regist_buf | blockBuff;

    if (IS25L_WriteStatusReg(regist_buf) != IS25L_OK) {
        return FLASH_CHIP_ERR;
    };
    return FLASH_OK;
}