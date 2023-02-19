/**
 ******************************************************************************
 * @file           : is25l.c
 * @date           : 2022-09-27
 * @brief          : This file provides code for the configuration
 *                   of the IS25L instances.
 ******************************************************************************
 */

/* Includes */
#include <stdint.h>
#include "main.h"
#include "is25l.h"
#include "loglib.h"
#include "parser.h"
#include "octospi.h"

/* Private variables */
static TX_MUTEX             muxIS25L;
static TX_EVENT_FLAGS_GROUP flagIS25L;
static ULONG                actual_events;

/* Private functions*/

static is25l_state_t IS25L_WriteEnable(void) {
    is25l_state_t state = IS25L_SPI_ERR;

    tx_mutex_get(&muxIS25L, TX_WAIT_FOREVER);

    OSPI_RegularCmdTypeDef command = {
        .OperationType = HAL_OSPI_OPTYPE_COMMON_CFG,
        .Instruction = IS25L_WRITE_ENABLE_CMD,
        .InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE,
        .InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS,
        .InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE,
        .Address = 0x0U,
        .AddressMode = HAL_OSPI_ADDRESS_NONE,
        .AddressSize = HAL_OSPI_ADDRESS_NONE,
        .AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE,
        .AlternateBytes = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE,
        .DataMode = HAL_OSPI_DATA_NONE,
        .DummyCycles = 0,
        .NbData = 0,
        .SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD,
        .DQSMode = HAL_OSPI_DQS_DISABLE,
        .DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE,
    };

    if (HAL_OSPI_Command_IT(&FLASH_QSPI, &command) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    tx_event_flags_get(&flagIS25L, IS25L_FLAG_CMD_CPLT, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);

    state = IS25L_OK;

quit:
    tx_mutex_put(&muxIS25L);
    return state;
}

static is25l_state_t IS25L_WaitForStatus(uint32_t mask, uint32_t match) {
    is25l_state_t state = IS25L_SPI_ERR;

    tx_mutex_get(&muxIS25L, TX_WAIT_FOREVER);

    OSPI_RegularCmdTypeDef command = {
        .OperationType = HAL_OSPI_OPTYPE_COMMON_CFG,
        .Instruction = IS25L_READ_STATUS_REG_CMD,
        .InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE,
        .InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS,
        .InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE,
        .Address = 0x00000000,
        .AddressMode = HAL_OSPI_ADDRESS_NONE,
        .AddressSize = HAL_OSPI_ADDRESS_NONE,
        .AddressDtrMode = HAL_OSPI_DATA_DTR_DISABLE,
        .AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .DataMode = HAL_OSPI_DATA_1_LINE,
        .DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE,
        .NbData = 1,
        .DummyCycles = 0,
        .DQSMode = HAL_OSPI_DQS_DISABLE,
        .SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD,
    };

    if (HAL_OSPI_Command(&FLASH_QSPI, &command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    OSPI_AutoPollingTypeDef poll_config = {
        .MatchMode = HAL_OSPI_MATCH_MODE_AND,
        .AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE,
        .Interval = 0x10,
        .Match = match,
        .Mask = mask,
    };

    if (HAL_OSPI_AutoPolling_IT(&FLASH_QSPI, &poll_config) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    tx_event_flags_get(&flagIS25L, IS25L_FLAG_STATUS_CPLT, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);

    state = IS25L_OK;

quit:
    tx_mutex_put(&muxIS25L);
    return state;
}

/* Global functions */

void IS25L_StatusMatchCallback(void) {
    tx_event_flags_set(&flagIS25L, IS25L_FLAG_STATUS_CPLT, TX_OR);
}

void IS25L_TxCompleteCallback(void) {
    tx_event_flags_set(&flagIS25L, IS25L_FLAG_TX_CPLT, TX_OR);
}

void IS25L_RxCompleteCallback(void) {
    tx_event_flags_set(&flagIS25L, IS25L_FLAG_RX_CPLT, TX_OR);
}

void IS25L_CmdCompleteCallback(void) {
    tx_event_flags_set(&flagIS25L, IS25L_FLAG_CMD_CPLT, TX_OR);
}

is25l_state_t IS25L_Init(void) {
    if (tx_mutex_create(&muxIS25L, "IS25L Mutex", TX_NO_INHERIT) != TX_SUCCESS) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        return IS25L_TX_ERR;
    }

    if (tx_event_flags_create(&flagIS25L, "IS25L Flag") != TX_SUCCESS) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        return IS25L_TX_ERR;
    }

    // Enable Quad operations if disabled
    uint8_t status_reg;
    if (IS25L_ReadStatusReg(&status_reg) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        return IS25L_SPI_ERR;
    }
    if (!(status_reg & IS25L_STATUS_QE)) {
        status_reg |= IS25L_STATUS_QE;
        if (IS25L_WriteStatusReg(status_reg) != IS25L_OK) {
            LOG_DEBUG("%s, %d", __FILE__, __LINE__);
            return IS25L_SPI_ERR;
        }
    }

    return IS25L_OK;
}

is25l_state_t IS25L_Deinit(void) {
    if (tx_mutex_delete(&muxIS25L) != TX_SUCCESS) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        return IS25L_TX_ERR;
    }

    if (tx_event_flags_delete(&flagIS25L) != TX_SUCCESS) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        return IS25L_TX_ERR;
    }

    return IS25L_OK;
}

is25l_state_t IS25L_ReadID(uint8_t *ID_buffer) {
    is25l_state_t state = IS25L_SPI_ERR;

    if (IS25L_WaitForStatus(IS25L_STATUS_WIP, 0) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    tx_mutex_get(&muxIS25L, TX_WAIT_FOREVER);
    OSPI_RegularCmdTypeDef command = {
        .OperationType = HAL_OSPI_OPTYPE_COMMON_CFG,
        .InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE,
        .Instruction = IS25L_READ_ID_CMD,
        .InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS,
        .InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE,
        .AddressMode = HAL_OSPI_ADDRESS_1_LINE,
        .AddressSize = HAL_OSPI_ADDRESS_24_BITS,
        .Address = 0x0U,
        .AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE,
        .AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytes = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE,
        .DataMode = HAL_OSPI_DATA_1_LINE,
        .DummyCycles = 0,
        .NbData = 3,
        .SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD,
        .DQSMode = HAL_OSPI_DQS_DISABLE,
        .DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE,
    };

    if (HAL_OSPI_Command(&FLASH_QSPI, &command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (HAL_OSPI_Receive_IT(&FLASH_QSPI, ID_buffer) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    tx_event_flags_get(&flagIS25L, IS25L_FLAG_RX_CPLT, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);

    state = IS25L_OK;

quit:
    tx_mutex_put(&muxIS25L);
    return state;
}

is25l_state_t IS25L_ReadStatusReg(uint8_t *reg_buffer) {
    is25l_state_t state = IS25L_SPI_ERR;

    tx_mutex_get(&muxIS25L, TX_WAIT_FOREVER);

    if (IS25L_WaitForStatus(IS25L_STATUS_WIP, 0) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    OSPI_RegularCmdTypeDef command = {
        .Instruction = IS25L_READ_STATUS_REG_CMD,
        .OperationType = HAL_OSPI_OPTYPE_COMMON_CFG,
        .InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE,
        .InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS,
        .InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE,
        .AddressMode = HAL_OSPI_ADDRESS_NONE,
        .AddressSize = HAL_OSPI_ADDRESS_NONE,
        .Address = 0x0U,
        .AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE,
        .AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytes = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE,
        .DataMode = HAL_OSPI_DATA_1_LINE,
        .DummyCycles = 0,
        .NbData = 1,
        .SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD,
        .DQSMode = HAL_OSPI_DQS_DISABLE,
        .DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE,
    };

    if (HAL_OSPI_Command(&FLASH_QSPI, &command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (HAL_OSPI_Receive_IT(&FLASH_QSPI, reg_buffer) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    tx_event_flags_get(&flagIS25L, IS25L_FLAG_RX_CPLT, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);

    state = IS25L_OK;

quit:
    tx_mutex_put(&muxIS25L);
    return state;
}

is25l_state_t IS25L_WriteStatusReg(uint8_t reg_buffer) {
    is25l_state_t state = IS25L_SPI_ERR;

    if (0x80 & reg_buffer) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        return IS25L_SPI_ERR;
    }

    tx_mutex_get(&muxIS25L, TX_WAIT_FOREVER);

    if (IS25L_WaitForStatus(IS25L_STATUS_WIP, 0) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (IS25L_WriteEnable() != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (IS25L_WaitForStatus(IS25L_STATUS_WEL, IS25L_STATUS_WEL) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    OSPI_RegularCmdTypeDef command = {
        .OperationType = HAL_OSPI_OPTYPE_COMMON_CFG,
        .Instruction = IS25L_WRITE_STATUS_REG_CMD,
        .InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE,
        .InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS,
        .InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE,
        .AddressMode = HAL_OSPI_ADDRESS_NONE,
        .AddressSize = HAL_OSPI_ADDRESS_NONE,
        .Address = 0x0U,
        .AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE,
        .AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytes = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE,
        .DataMode = HAL_OSPI_DATA_1_LINE,
        .DummyCycles = 0,
        .NbData = 1,
        .SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD,
        .DQSMode = HAL_OSPI_DQS_DISABLE,
        .DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE,
    };

    if (HAL_OSPI_Command(&FLASH_QSPI, &command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (HAL_OSPI_Transmit_IT(&FLASH_QSPI, &reg_buffer) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    tx_event_flags_get(&flagIS25L, IS25L_FLAG_TX_CPLT, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);

    if (IS25L_WaitForStatus(IS25L_STATUS_WIP, 0) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

quit:
    tx_mutex_put(&muxIS25L);
    return state;
}

/* Function for erasing the whole chip */
is25l_state_t IS25L_EraseChip(void) {
    is25l_state_t state = IS25L_SPI_ERR;

    tx_mutex_get(&muxIS25L, TX_WAIT_FOREVER);

    if (IS25L_WaitForStatus(IS25L_STATUS_WIP, 0) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (IS25L_WriteEnable() != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (IS25L_WaitForStatus(IS25L_STATUS_WEL, IS25L_STATUS_WEL) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    OSPI_RegularCmdTypeDef command = {
        .OperationType = HAL_OSPI_OPTYPE_COMMON_CFG,
        .Instruction = IS25L_ERASE_CHIP_CMD,
        .InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE,
        .InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS,
        .InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE,
        .AddressMode = HAL_OSPI_ADDRESS_NONE,
        .AddressSize = HAL_OSPI_ADDRESS_NONE,
        .Address = 0,
        .AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE,
        .AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytes = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE,
        .DataMode = HAL_OSPI_DATA_NONE,
        .DummyCycles = 0,
        .NbData = 0,
        .SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD,
        .DQSMode = HAL_OSPI_DQS_DISABLE,
        .DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE,
    };

    if (HAL_OSPI_Command_IT(&FLASH_QSPI, &command) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    tx_event_flags_get(&flagIS25L, IS25L_FLAG_CMD_CPLT, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);

    if (IS25L_WaitForStatus(IS25L_STATUS_WIP, 0) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    state = IS25L_OK;

quit:
    tx_mutex_put(&muxIS25L);
    return state;
}

/* Function for erasing selected sector */
is25l_state_t IS25L_EraseSector(uint32_t RAW_SectorAddress) {
    is25l_state_t state = IS25L_SPI_ERR;

    if (RAW_SectorAddress > 0x3FFFFF) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        return IS25L_PARAM_ERR;
    }

    tx_mutex_get(&muxIS25L, TX_WAIT_FOREVER);

    if (IS25L_WaitForStatus(IS25L_STATUS_WIP, 0) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (IS25L_WriteEnable() != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (IS25L_WaitForStatus(IS25L_STATUS_WEL, IS25L_STATUS_WEL) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    OSPI_RegularCmdTypeDef command = {
        .InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE,
        .Instruction = IS25L_ERASE_SECTOR_CMD,    // Sector Erase Operation (0x20)
        .AddressSize = HAL_OSPI_ADDRESS_24_BITS,
        .AddressMode = HAL_OSPI_ADDRESS_1_LINE,
        .Address = RAW_SectorAddress,
        .AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytes = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .DummyCycles = 0,
        .DataMode = HAL_OSPI_DATA_NONE,
        .NbData = 0,
        .SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD,
    };

    if (HAL_OSPI_Command_IT(&FLASH_QSPI, &command) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    tx_event_flags_get(&flagIS25L, IS25L_FLAG_CMD_CPLT, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);

    if (IS25L_WaitForStatus(IS25L_STATUS_WIP, 0) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    state = IS25L_OK;

quit:
    tx_mutex_put(&muxIS25L);
    return state;
}

is25l_state_t IS25L_FastReadQUADOperation(uint8_t *FR_Dat_Ptr, uint32_t Addr_start, uint16_t data_size) {
    is25l_state_t state = IS25L_SPI_ERR;

    tx_mutex_get(&muxIS25L, TX_WAIT_FOREVER);

    if (IS25L_WaitForStatus(IS25L_STATUS_WIP, 0) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    OSPI_RegularCmdTypeDef command = {
        .OperationType = HAL_OSPI_OPTYPE_COMMON_CFG,
        .InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE,
        .Instruction = IS25L_INOUT_FAST_READ_CMD,
        .InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS,
        .InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE,
        .Address = Addr_start,
        .AddressMode = HAL_OSPI_ADDRESS_4_LINES,
        .AddressSize = HAL_OSPI_ADDRESS_24_BITS,
        .AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE,
        .AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytes = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE,
        .DataMode = HAL_OSPI_DATA_4_LINES,
        .DummyCycles = 6,
        .NbData = data_size,
        .SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD,
        .DQSMode = HAL_OSPI_DQS_DISABLE,
        .DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE,
    };

    if (HAL_OSPI_Command(&FLASH_QSPI, &command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (HAL_OSPI_Receive_IT(&FLASH_QSPI, FR_Dat_Ptr) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    tx_event_flags_get(&flagIS25L, IS25L_FLAG_RX_CPLT, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);

    state = IS25L_OK;

quit:
    tx_mutex_put(&muxIS25L);
    return state;
}

is25l_state_t IS25L_WritePPQ(uint8_t *buffer, uint32_t RAW_PageAddress, uint16_t data_lenght) {
    is25l_state_t state = IS25L_SPI_ERR;

    if (data_lenght > 256 || data_lenght == 0) {
        return IS25L_PARAM_ERR;
    }

    tx_mutex_get(&muxIS25L, TX_WAIT_FOREVER);

    if (IS25L_WaitForStatus(IS25L_STATUS_WIP, 0) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (IS25L_WriteEnable() != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (IS25L_WaitForStatus(IS25L_STATUS_WEL, IS25L_STATUS_WEL) != IS25L_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    OSPI_RegularCmdTypeDef command = {
        .OperationType = HAL_OSPI_OPTYPE_COMMON_CFG,
        .InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE,
        .Instruction = IS25L_WRITE_PPQ_INP_CMD,
        .InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS,
        .InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE,
        .AddressMode = HAL_OSPI_ADDRESS_1_LINE,
        .AddressSize = HAL_OSPI_ADDRESS_24_BITS,
        .Address = RAW_PageAddress,
        .AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE,
        .AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytes = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE,
        .DataMode = HAL_OSPI_DATA_4_LINES,
        .DummyCycles = 0,
        .NbData = data_lenght,
        .SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD,
        .DQSMode = HAL_OSPI_DQS_DISABLE,
        .DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE,
    };

    if (HAL_OSPI_Command(&FLASH_QSPI, &command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    if (HAL_OSPI_Transmit_IT(&FLASH_QSPI, buffer) != HAL_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    tx_event_flags_get(&flagIS25L, IS25L_FLAG_TX_CPLT, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);

    state = IS25L_OK;

quit:
    tx_mutex_put(&muxIS25L);
    return state;
}

uint32_t PageToAddress(uint32_t PageNumber, uint8_t PageShift) {
    return PageNumber * IS25L_MEMORY_PAGE_SIZE + PageShift;
}

uint32_t SectorToAddress(uint32_t SectorNumber) {
    return SectorNumber * IS25L_MEMORY_SECTOR_SIZE * 1024U;
}
