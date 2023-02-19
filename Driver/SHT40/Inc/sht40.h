/**
 * @file sht40.h
 * @brief SHT40 Humidity and temperature sensor driver
 * @version 0.1
 * @date 2023-01-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SHT40_H
#define SHT40_H

#include "main.h"

/**
 * @brief Return status type
 */
typedef enum {
    HUMID_S_OK,
    HUMID_S_ERR_GENERAL,
    HUMID_S_ERR_NULL_PTR,
    HUMID_S_ERR_MEMORY,
    HUMID_S_ERR_NOT_INITED,
    HUMID_S_ERR_WRONG_PARAM,
} humid_status_t;

/**
 * @brief Precision type
 */
typedef enum {
    HUMID_PRECIS_LPM, /** @brief Lowest precision */
    HUMID_PRECIS_MPM, /** @brief Medium precision */
    HUMID_PRECIS_HPM  /** @brief Highest precision */
} humid_precision_t;

/**
 * @brief Preheat type
 */
typedef enum {
    HUMID_PREHEAT_20_MW_0_1_S,  /** @brief 20 mW for 0.1 sec */
    HUMID_PREHEAT_20_MW_1_S,    /** @brief 20 mW for 1 sec */
    HUMID_PREHEAT_110_MW_0_1_S, /** @brief 110 mW for 0.1 sec */
    HUMID_PREHEAT_110_MW_1_S,   /** @brief 110 mW for 1 sec */
    HUMID_PREHEAT_200_MW_0_1_S, /** @brief 200 mW for 0.1 sec */
    HUMID_PREHEAT_200_MW_1_S,   /** @brief 200 mW for 1 sec */
} humid_preheat_t;

/**
 * @brief SHT40 Init
 *
 * @return humid_status_t Status
 */
humid_status_t HUMID_Init(void);

/**
 * @brief SHT40 Deinit
 *
 * @return humid_status_t Status
 */
humid_status_t HUMID_DeInit(void);

/**
 * @brief Get sensor's init
 *
 * @return uint32_t (0) - not inited, !(0) = inited
 */
uint32_t HUMID_IsInited(void);

/**
 * @brief Software reset
 *
 * @return humid_status_t Status
 */
humid_status_t HUMID_SoftReset(void);

/**
 * @brief Set sensor precision
 *
 * @param precision Precision
 * @return humid_status_t Status
 */
humid_status_t HUMID_SetPrecision(humid_precision_t precision);

/**
 * @brief Measure temperature and humidity
 *
 * @param[out] humidity Humidity, in mili % RH
 * @param[out] temperature Temperature, in mili deg C
 * @return humid_status_t Status
 */
humid_status_t HUMID_Measure(uint32_t *humidity, int32_t *temperature);

/**
 * @brief
 *
 * @param humidity
 * @param temperature
 * @return humid_status_t
 */
humid_status_t HUMID_MeasureWithPreheat(humid_preheat_t preheat, uint32_t *humidity, int32_t *temperature);

/**
 * @brief Read sensor's unique serial number (two 16-bit words)
 *
 * @param[out] sn Serial number
 * @return humid_status_t Status
 */
humid_status_t HUMID_ReadSerialNumber(uint16_t *sn);

#endif    // SHT40_H
