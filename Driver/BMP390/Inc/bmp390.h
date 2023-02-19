/**
 * @file bmp390.h
 * @brief barometer driver header
 * @date 2022-12-28
 *
 *   (c) 2022
 */

#ifndef BMP390_H_
#define BMP390_H_

#include "bmp3.h"
#include "platform_i2c.h"

/** @name Barometer statuses */
typedef enum {
    BARO_S_OK,
    BARO_S_GENERAL_ERR,
    BARO_S_NULL_PTR_ERR,
    BARO_S_PWR_SET_ERR,
    BARO_S_PRESET_ERR,
    BARO_S_GET_DATA_ERR,
    BARO_S_NOT_INITED_ERR
} baro_status_t;

/** @name Receive data types */
typedef enum {
    BARO_D_PRESSURE = 1,
    BARO_D_TEMPERATURE,
    BARO_D_PRESS_AND_TEMP
} baro_dataType_t;

/**
 * @name Resolution parameters
 * @param BARO_P_STANDARD_RES Default value
 */
typedef enum {
    BARO_P_ULTRA_LOW_RES,     // 100Hz
    BARO_P_LOW_RES,           // 100Hz
    BARO_P_STANDARD_RES,      // 50Hz
    BARO_P_HIGH_RES,          // 25Hz
    BARo_P_ULTRA_HIGH_RES,    // 12.5Hz
    BARO_P_HIGHEST_RES        // 12.5Hz
} baro_preset_t;

/**
 * @name IIR filter parameters
 * @param BARO_IIR_15 Default value
 */
typedef enum {
    BARO_IIR_DISABLE,
    BARO_IIR_1,
    BARO_IIR_3,
    BARO_IIR_7,
    BARO_IIR_15,
    BARO_IIR_31,
    BARO_IIR_63,
    BARO_IIR_127
} baro_iir_filter_t;

/**
 * @name Struct for data receive
 * @param pressure [Pascales] Receive example -- 10089879 -> 100898.79
 * @param temperature [Celcius] Receive example -- 2048 -> 20.48
 */
struct baro_data_t {
    uint32_t pressure;
    int32_t  temperature;
};

/**
 * @brief Function for initialization barometer and set default preset
 * @param i2c Bus for data transfer
 * @attention Call this function again if you want to change i2c bus or to set default preset
 * @return BARO_S_OK if everything is okay otherwise error status
 */
baro_status_t BARO_Init(i2c_bus_t *i2c);

/**
 * @brief Put barometer to normal work mode
 * @return BARO_S_OK if everything is okay otherwise error status
 */
baro_status_t BARO_TurnOn();

/**
 * @brief Put barometer to sleep mode
 * @return BARO_S_OK if everything is okay otherwise error status
 */
baro_status_t BARO_TurnOff();

/**
 * @brief Receive data from barometer
 * @param dataType Type of data you want to receive: temp or press or both
 * @param dataStruct Pointer to store data
 * @attention Data parameter you don`t want to receive will return as 0
 * @attention Do not call this func faster than barometer preset frequency otherwise you get nothing new
 * @return BARO_S_OK if everything is okay otherwise error status
 */
baro_status_t BARO_DataReceive(baro_dataType_t dataType, struct baro_data_t *dataStruct);

/**
 * @brief Set preset for barometer. If you did not call this function default preset and filter will be used
 * @param preset Preset you want to set
 * @param filter IIR filter you want to set
 * @return BARO_S_OK if everything is okay otherwise error status
 */
baro_status_t BARO_SetPreset(baro_preset_t preset, baro_iir_filter_t filter);

#endif    // BMP390_H_
