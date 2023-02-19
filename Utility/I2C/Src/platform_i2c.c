/**
 * @file platform_i2c.c
 * @brief Platform unified I2C driver for mutual access to the bus
 * @version 0.1
 * @date 2022-12-29
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "platform_i2c.h"
#define LOG_DEFAULT_MODULE LOG_M_I2C
#include "loglib.h"
#include <malloc.h>
#include <stdlib.h>
#include <sys/queue.h>

#define TRANSMIT_COMPLETE  0x0001
#define RECEIVE_COMPLETE   0x0002
#define MEM_WRITE_COMPLETE 0x0004
#define MEM_READ_COMPLETE  0x0008
#define WAIT_TIMEOUT       100

struct entry {
    i2c_bus_t *i2c_bus;
    SLIST_ENTRY(entry) entries;
};

static const char *error_code[] = {
    "No error",
    "BERR error",
    "ARLO error",
    "ACKF error",
    "OVR error",
    "DMA transfer error",
    "Timeout error",
    "Size Management error",
    "DMA Parameter error",
};

static struct entry *i2cs;
static SLIST_HEAD(slisthead, entry) head;

i2c_status_t I2C_Init(i2c_bus_t *i2c, I2C_HandleTypeDef *hi2c, TX_MUTEX *mutex, TX_EVENT_FLAGS_GROUP *ef) {
    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (i2c->inited) {
        return I2C_ERR_ALLREADY_INITED;
    }

    i2c->hi2c = hi2c;

    if (tx_mutex_create(mutex, "mx", TX_NO_INHERIT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }
    i2c->mutex = mutex;

    if (tx_event_flags_create(ef, "ef") != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }
    i2c->ef = ef;

    struct entry *new_i2c = malloc(sizeof(struct entry));
    SLIST_INSERT_HEAD(&head, new_i2c, entries);
    new_i2c->i2c_bus = i2c;

    i2c->inited = 1;

    return I2C_OK;
}

i2c_status_t I2C_DeInit(i2c_bus_t *i2c) {
    /**
     * @todo Implement deinitialization
     */
    return I2C_OK;
}

i2c_status_t I2C_GetI2CBus(I2C_HandleTypeDef *hi2c, i2c_bus_t **i2c) {
    SLIST_FOREACH(i2cs, &head, entries) {
        if (i2cs->i2c_bus->hi2c == hi2c) {
            *i2c = i2cs->i2c_bus;
            return I2C_OK;
        }
    }
    *i2c = NULL;
    return I2C_ERR_MEMORY;
}

i2c_status_t I2C_WriteBytes(i2c_bus_t *i2c, uint8_t addr, uint8_t *bytes, size_t size) {
    i2c_status_t      st = I2C_OK;
    HAL_StatusTypeDef hal_st;

    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (!i2c->inited) {
        LOG_WARN("Not inited");
        return I2C_ERR_NOT_INITED;
    }

    if (tx_mutex_get(i2c->mutex, WAIT_TIMEOUT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }

    hal_st = HAL_I2C_Master_Transmit(i2c->hi2c, addr, bytes, size, WAIT_TIMEOUT);
    if (hal_st != HAL_OK) {
        LOG_WARN("Error transmitting %d bytes (addr 0x%02X)", size, addr);
        switch (hal_st) {
            case HAL_TIMEOUT:
                st = I2C_ERR_TIMEOUT;
                break;
            default:
                st = I2C_ERR_GENERAL;
                break;
        }
    }

    if (tx_mutex_put(i2c->mutex) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }
    return st;
}

i2c_status_t I2C_WriteBytes_IT(i2c_bus_t *i2c, uint8_t addr, uint8_t *bytes, size_t size) {
    i2c_status_t      st = I2C_OK;
    HAL_StatusTypeDef hal_st;

    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (!i2c->inited) {
        LOG_WARN("Not inited");
        return I2C_ERR_NOT_INITED;
    }

    if (tx_mutex_get(i2c->mutex, WAIT_TIMEOUT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }

    hal_st = HAL_I2C_Master_Transmit_IT(i2c->hi2c, addr, bytes, size);
    if (hal_st != HAL_OK) {
        LOG_WARN("Error IT transmitting %d bytes (addr 0x%02X)", size, addr);
        switch (hal_st) {
            case HAL_TIMEOUT:
                st = I2C_ERR_TIMEOUT;
                break;
            default:
                st = I2C_ERR_GENERAL;
                break;
        }
    }

    if (tx_event_flags_get(i2c->ef, TRANSMIT_COMPLETE, TX_OR_CLEAR, &i2c->flags, WAIT_TIMEOUT) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }

    if (tx_mutex_put(i2c->mutex) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }
    return st;
}

i2c_status_t I2C_WriteBytes_DMA(i2c_bus_t *i2c, uint8_t addr, uint8_t *bytes, size_t size) {
    i2c_status_t      st = I2C_OK;
    HAL_StatusTypeDef hal_st;

    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (!i2c->inited) {
        LOG_WARN("Not inited");
        return I2C_ERR_NOT_INITED;
    }

    if (tx_mutex_get(i2c->mutex, WAIT_TIMEOUT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }

    hal_st = HAL_I2C_Master_Transmit_DMA(i2c->hi2c, addr, bytes, size);
    if (hal_st != HAL_OK) {
        LOG_WARN("Error IT transmitting %d bytes (addr 0x%02X)", size, addr);
        switch (hal_st) {
            case HAL_TIMEOUT:
                st = I2C_ERR_TIMEOUT;
                break;
            default:
                st = I2C_ERR_GENERAL;
                break;
        }
    }

    if (tx_event_flags_get(i2c->ef, TRANSMIT_COMPLETE, TX_OR_CLEAR, &i2c->flags, WAIT_TIMEOUT) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }

    if (tx_mutex_put(i2c->mutex) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }
    return st;
}
i2c_status_t I2C_ReadBytes(i2c_bus_t *i2c, uint8_t addr, uint8_t *bytes, size_t size) {
    i2c_status_t      st = I2C_OK;
    HAL_StatusTypeDef hal_st;

    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (!i2c->inited) {
        LOG_WARN("Not inited");
        return I2C_ERR_NOT_INITED;
    }

    if (tx_mutex_get(i2c->mutex, WAIT_TIMEOUT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }

    hal_st = HAL_I2C_Master_Receive(i2c->hi2c, addr, bytes, size, WAIT_TIMEOUT);
    if (hal_st != HAL_OK) {
        LOG_WARN("Error transmitting %d bytes (addr 0x%02X)", size, addr);
        switch (hal_st) {
            case HAL_TIMEOUT:
                st = I2C_ERR_TIMEOUT;
                break;
            default:
                st = I2C_ERR_GENERAL;
                break;
        }
    }

    if (tx_mutex_put(i2c->mutex) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }
    return st;
}

i2c_status_t I2C_ReadBytes_IT(i2c_bus_t *i2c, uint8_t addr, uint8_t *bytes, size_t size) {
    i2c_status_t      st = I2C_OK;
    HAL_StatusTypeDef hal_st;

    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (!i2c->inited) {
        LOG_WARN("Not inited");
        return I2C_ERR_NOT_INITED;
    }

    if (tx_mutex_get(i2c->mutex, WAIT_TIMEOUT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }

    hal_st = HAL_I2C_Master_Receive_IT(i2c->hi2c, addr, bytes, size);
    if (hal_st != HAL_OK) {
        LOG_WARN("Error IT receiving %d bytes (addr 0x%02X)", size, addr);
        switch (hal_st) {
            case HAL_TIMEOUT:
                st = I2C_ERR_TIMEOUT;
                break;
            default:
                st = I2C_ERR_GENERAL;
                break;
        }
    }

    if (tx_event_flags_get(i2c->ef, RECEIVE_COMPLETE, TX_OR_CLEAR, &i2c->flags, WAIT_TIMEOUT) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }

    if (tx_mutex_put(i2c->mutex) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }
    return st;
}

i2c_status_t I2C_ReadBytes_DMA(i2c_bus_t *i2c, uint8_t addr, uint8_t *bytes, size_t size) {
    i2c_status_t      st = I2C_OK;
    HAL_StatusTypeDef hal_st;

    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (!i2c->inited) {
        LOG_WARN("Not inited");
        return I2C_ERR_NOT_INITED;
    }

    if (tx_mutex_get(i2c->mutex, WAIT_TIMEOUT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }

    hal_st = HAL_I2C_Master_Receive_DMA(i2c->hi2c, addr, bytes, size);
    if (hal_st != HAL_OK) {
        LOG_WARN("Error IT receiving %d bytes (addr 0x%02X)", size, addr);
        switch (hal_st) {
            case HAL_TIMEOUT:
                st = I2C_ERR_TIMEOUT;
                break;
            default:
                st = I2C_ERR_GENERAL;
                break;
        }
    }

    if (tx_event_flags_get(i2c->ef, RECEIVE_COMPLETE, TX_OR_CLEAR, &i2c->flags, WAIT_TIMEOUT) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }

    if (tx_mutex_put(i2c->mutex) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }
    return st;
}

i2c_status_t I2C_Write(i2c_bus_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t size) {
    i2c_status_t      st = I2C_OK;
    HAL_StatusTypeDef hal_st;

    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (!i2c->inited) {
        LOG_WARN("Not inited");
        return I2C_ERR_NOT_INITED;
    }

    if (tx_mutex_get(i2c->mutex, WAIT_TIMEOUT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }

    hal_st = HAL_I2C_Mem_Write(i2c->hi2c, addr, reg, I2C_MEMADD_SIZE_8BIT, data, size, WAIT_TIMEOUT);
    if (hal_st != HAL_OK) {
        LOG_WARN("Error transmitting %d bytes (addr 0x%02X)", size, addr);
        switch (hal_st) {
            case HAL_TIMEOUT:
                st = I2C_ERR_TIMEOUT;
                break;
            default:
                st = I2C_ERR_GENERAL;
                break;
        }
    }

    if (tx_mutex_put(i2c->mutex) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }
    return st;
}

i2c_status_t I2C_Write_IT(i2c_bus_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t size) {
    i2c_status_t      st = I2C_OK;
    HAL_StatusTypeDef hal_st;

    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (!i2c->inited) {
        LOG_WARN("Not inited");
        return I2C_ERR_NOT_INITED;
    }

    if (tx_mutex_get(i2c->mutex, WAIT_TIMEOUT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }

    hal_st = HAL_I2C_Mem_Write_IT(i2c->hi2c, addr, reg, I2C_MEMADD_SIZE_8BIT, data, size);
    if (hal_st != HAL_OK) {
        LOG_WARN("Error IT transmitting %d bytes (addr 0x%02X)", size, addr);
        switch (hal_st) {
            case HAL_TIMEOUT:
                st = I2C_ERR_TIMEOUT;
                break;
            default:
                st = I2C_ERR_GENERAL;
                break;
        }
    }

    if (tx_event_flags_get(i2c->ef, MEM_WRITE_COMPLETE, TX_OR_CLEAR, &i2c->flags, WAIT_TIMEOUT) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }

    if (tx_mutex_put(i2c->mutex) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }
    return st;
}

i2c_status_t I2C_Write_DMA(i2c_bus_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t size) {
    i2c_status_t      st = I2C_OK;
    HAL_StatusTypeDef hal_st;

    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (!i2c->inited) {
        LOG_WARN("Not inited");
        return I2C_ERR_NOT_INITED;
    }

    if (tx_mutex_get(i2c->mutex, WAIT_TIMEOUT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }

    hal_st = HAL_I2C_Mem_Write_DMA(i2c->hi2c, addr, reg, I2C_MEMADD_SIZE_8BIT, data, size);
    if (hal_st != HAL_OK) {
        LOG_WARN("Error DMA transmitting %d bytes (addr 0x%02X)", size, addr);
        switch (hal_st) {
            case HAL_TIMEOUT:
                st = I2C_ERR_TIMEOUT;
                break;
            default:
                st = I2C_ERR_GENERAL;
                break;
        }
    }

    if (tx_event_flags_get(i2c->ef, MEM_WRITE_COMPLETE, TX_OR_CLEAR, &i2c->flags, WAIT_TIMEOUT) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }

    if (tx_mutex_put(i2c->mutex) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }
    return st;
}

i2c_status_t I2C_Read(i2c_bus_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t size) {
    i2c_status_t      st = I2C_OK;
    HAL_StatusTypeDef hal_st;

    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (!i2c->inited) {
        LOG_WARN("Not inited");
        return I2C_ERR_NOT_INITED;
    }

    if (tx_mutex_get(i2c->mutex, WAIT_TIMEOUT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }

    hal_st = HAL_I2C_Mem_Read(i2c->hi2c, addr, reg, I2C_MEMADD_SIZE_8BIT, data, size, WAIT_TIMEOUT);
    if (hal_st != HAL_OK) {
        LOG_WARN("Error receiving %d bytes (addr 0x%02X)", size, addr);
        switch (hal_st) {
            case HAL_TIMEOUT:
                st = I2C_ERR_TIMEOUT;
                break;
            default:
                st = I2C_ERR_GENERAL;
                break;
        }
    }

    if (tx_mutex_put(i2c->mutex) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }
    return st;
}

i2c_status_t I2C_Read_IT(i2c_bus_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t size) {
    i2c_status_t      st = I2C_OK;
    HAL_StatusTypeDef hal_st;

    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (!i2c->inited) {
        LOG_WARN("Not inited");
        return I2C_ERR_NOT_INITED;
    }

    if (tx_mutex_get(i2c->mutex, WAIT_TIMEOUT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }

    hal_st = HAL_I2C_Mem_Read_IT(i2c->hi2c, addr, reg, I2C_MEMADD_SIZE_8BIT, data, size);
    if (hal_st != HAL_OK) {
        LOG_WARN("Error IT receiving %d bytes (addr 0x%02X)", size, addr);
        switch (hal_st) {
            case HAL_TIMEOUT:
                st = I2C_ERR_TIMEOUT;
                break;
            default:
                st = I2C_ERR_GENERAL;
                break;
        }
    }

    if (tx_event_flags_get(i2c->ef, MEM_READ_COMPLETE, TX_OR_CLEAR, &i2c->flags, WAIT_TIMEOUT) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }

    if (tx_mutex_put(i2c->mutex) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }
    return st;
}

i2c_status_t I2C_Read_DMA(i2c_bus_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t size) {
    i2c_status_t      st = I2C_OK;
    HAL_StatusTypeDef hal_st;

    if (!i2c) {
        LOG_ERROR("Wrong handle");
        return I2C_ERR_MEMORY;
    }

    if (!i2c->inited) {
        LOG_WARN("Not inited");
        return I2C_ERR_NOT_INITED;
    }

    if (tx_mutex_get(i2c->mutex, WAIT_TIMEOUT) != TX_SUCCESS) {
        return I2C_ERR_GENERAL;
    }

    hal_st = HAL_I2C_Mem_Read_DMA(i2c->hi2c, addr, reg, I2C_MEMADD_SIZE_8BIT, data, size);
    if (hal_st != HAL_OK) {
        LOG_WARN("Error DMA receiving %d bytes (addr 0x%02X)", size, addr);
        switch (hal_st) {
            case HAL_TIMEOUT:
                st = I2C_ERR_TIMEOUT;
                break;
            default:
                st = I2C_ERR_GENERAL;
                break;
        }
    }

    if (tx_event_flags_get(i2c->ef, MEM_READ_COMPLETE, TX_OR_CLEAR, &i2c->flags, WAIT_TIMEOUT) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }

    if (tx_mutex_put(i2c->mutex) != TX_SUCCESS) {
        st = I2C_ERR_GENERAL;
    }
    return st;
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    SLIST_FOREACH(i2cs, &head, entries) {
        if (i2cs->i2c_bus->hi2c == hi2c) {
            tx_event_flags_set(i2cs->i2c_bus->ef, TRANSMIT_COMPLETE, TX_OR);
            return;
        }
    }
    LOG_WARN("I2C handle not found. Is it initialized?");
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    SLIST_FOREACH(i2cs, &head, entries) {
        if (i2cs->i2c_bus->hi2c == &I2C) {
            tx_event_flags_set(i2cs->i2c_bus->ef, RECEIVE_COMPLETE, TX_OR);
            return;
        }
    }
    LOG_WARN("I2C handle not found. Is it initialized?");
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    SLIST_FOREACH(i2cs, &head, entries) {
        if (i2cs->i2c_bus->hi2c == hi2c) {
            tx_event_flags_set(i2cs->i2c_bus->ef, MEM_WRITE_COMPLETE, TX_OR);
            return;
        }
    }
    LOG_WARN("I2C handle not found. Is it initialized?");
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    SLIST_FOREACH(i2cs, &head, entries) {
        if (i2cs->i2c_bus->hi2c == hi2c) {
            tx_event_flags_set(i2cs->i2c_bus->ef, MEM_READ_COMPLETE, TX_OR);
            return;
        }
    }
    LOG_WARN("I2C handle not found. Is it initialized?");
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    /**
     * @todo Handle I2C errors
     */
    LOG_WARN("Error callback, code %d (%s)", hi2c->ErrorCode, error_code[__builtin_ctz(hi2c->ErrorCode) + 1]);
}

void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c) {
    /**
     * @todo Handle I2C abort transaction
     */
    LOG_WARN("Abort callback");
}