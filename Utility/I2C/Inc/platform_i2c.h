/**
 * @file platform_i2c.h
 * @brief Platform unified I2C driver for mutual access to the bus
 * @version 0.1
 * @date 2022-12-29
 *
 *
 */

#ifndef PLATFORM_I2C_H_
#define PLATFORM_I2C_H_

#include "main.h"
#include "i2c.h"
#include "tx_api.h"

/**
 * @brief I2C routines return status
 *
 */
typedef enum {
    I2C_OK = 0,
    I2C_ERR_GENERAL,
    I2C_ERR_BUSY,
    I2C_ERR_TIMEOUT,
    I2C_ERR_MEMORY,
    I2C_ERR_ALLREADY_INITED,
    I2C_ERR_NOT_INITED,
} i2c_status_t;

/**
 * @brief I2C bus specific structure
 *
 */
typedef struct {
    I2C_HandleTypeDef    *hi2c;
    TX_MUTEX             *mutex;
    TX_EVENT_FLAGS_GROUP *ef;
    uint32_t              flags;
    uint32_t              inited : 1;
} i2c_bus_t;

/**
 * @brief I2C initialization
 *
 * @param i2c I2C bus
 * @param hi2c HAL I2C handle
 * @param mutex Dedicated mutex
 * @param ef Dedicated event flags group
 * @return i2c_status_t Status
 */
i2c_status_t I2C_Init(i2c_bus_t *i2c, I2C_HandleTypeDef *hi2c, TX_MUTEX *mutex, TX_EVENT_FLAGS_GROUP *ef);

/**
 * @brief I2C deinitialization
 *
 * @param i2c I2C bus
 * @return i2c_status_t Status
 *
 * @note Not implemented
 * @todo Implement deinitialization
 */
i2c_status_t I2C_DeInit(i2c_bus_t *i2c);

/**
 * @brief Find I2C bus by HAL I2C handle
 *
 * @param [in] hi2c HAL I2C handle
 * @param [out] i2c Found I2C handle if found, NULL if not found
 * @return i2c_status_t Status
 */
i2c_status_t I2C_GetI2CBus(I2C_HandleTypeDef *hi2c, i2c_bus_t **i2c);

/**
 * @brief Blocking I2C write bytes to device
 *
 * @param i2c I2C bus
 * @param addr I2C device address
 * @param bytes Bytes to write
 * @param size Amount of bytes
 * @return i2c_status_t Status
 */
i2c_status_t I2C_WriteBytes(i2c_bus_t *i2c, uint8_t addr, uint8_t *bytes, size_t size);

/**
 * @brief Non-blocking I2C write bytes via Interrupts to device
 *
 * @param i2c I2C bus
 * @param addr I2C device address
 * @param bytes Bytes to write
 * @param size Amount of bytes
 * @return i2c_status_t Status
 */
i2c_status_t I2C_WriteBytes_IT(i2c_bus_t *i2c, uint8_t addr, uint8_t *bytes, size_t size);

/**
 * @brief Non-blocking I2C write bytes via DMA to device
 *
 * @param i2c I2C bus
 * @param addr I2C device address
 * @param bytes Bytes to write
 * @param size Amount of bytes
 * @return i2c_status_t Status
 */
i2c_status_t I2C_WriteBytes_DMA(i2c_bus_t *i2c, uint8_t addr, uint8_t *bytes, size_t size);

/**
 * @brief Blocking I2C read bytes from device
 *
 * @param i2c I2C bus
 * @param addr I2C device address
 * @param bytes Bytes to read
 * @param size Amount of bytes
 * @return i2c_status_t Status
 */
i2c_status_t I2C_ReadBytes(i2c_bus_t *i2c, uint8_t addr, uint8_t *bytes, size_t size);

/**
 * @brief Non-blocking I2C read bytes via Interrupts from device
 *
 * @param i2c I2C bus
 * @param addr I2C device address
 * @param bytes Bytes to read
 * @param size Amount of bytes
 * @return i2c_status_t Status
 */
i2c_status_t I2C_ReadBytes_IT(i2c_bus_t *i2c, uint8_t addr, uint8_t *bytes, size_t size);

/**
 * @brief Non-blocking I2C read bytes via DMA from device
 *
 * @param i2c I2C bus
 * @param addr I2C device address
 * @param bytes Bytes to read
 * @param size Amount of bytes
 * @return i2c_status_t Status
 */
i2c_status_t I2C_ReadBytes_DMA(i2c_bus_t *i2c, uint8_t addr, uint8_t *bytes, size_t size);

/**
 * @brief Blocking I2C write to specific register
 *
 * @param i2c I2C bus
 * @param addr I2C device address
 * @param reg Register to write
 * @param data Data to write
 * @param size Amount of data
 * @return i2c_status_t Status
 */
i2c_status_t I2C_Write(i2c_bus_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t size);

/**
 * @brief Non-blocking I2C write via Interrupts to specific register
 *
 * @param i2c I2C bus
 * @param addr I2C device address
 * @param reg Register to write
 * @param data Data to write
 * @param size Amount of data
 * @return i2c_status_t Status
 */
i2c_status_t I2C_Write_IT(i2c_bus_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t size);

/**
 * @brief Non-blocking I2C write via DMA to specific register
 *
 * @param i2c I2C bus
 * @param addr I2C device address
 * @param reg Register to write
 * @param data Data to write
 * @param size Amount of data
 * @return i2c_status_t Status
 */
i2c_status_t I2C_Write_DMA(i2c_bus_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t size);

/**
 * @brief Blocking I2C read from specific register
 *
 * @param i2c I2C bus
 * @param addr I2C device address
 * @param reg Register to write
 * @param data Data to write
 * @param size Amount of data
 * @return i2c_status_t Status
 */
i2c_status_t I2C_Read(i2c_bus_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t size);

/**
 * @brief Non-blocking I2C read via Interrupts from specific register
 *
 * @param i2c I2C bus
 * @param addr I2C device address
 * @param reg Register to write
 * @param data Data to write
 * @param size Amount of data
 * @return i2c_status_t Status
 */
i2c_status_t I2C_Read_IT(i2c_bus_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t size);

/**
 * @brief Non-blocking I2C read via DMA from specific register
 *
 * @param i2c I2C bus
 * @param addr I2C device address
 * @param reg Register to write
 * @param data Data to write
 * @param size Amount of data
 * @return i2c_status_t Status
 */
i2c_status_t I2C_Read_DMA(i2c_bus_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t size);

#endif    // PLATFORM_I2C_H_
