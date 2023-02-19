/**
 * @file lis3dh.h
 * @brief accelerometer driver header file
 * @date 2023-01-13
 *
 *   (c) 2022
 */

#include "platform_i2c.h"
#include "lis3dh_reg.h"

typedef enum {
    LIS3DH_S_OK,
    LIS3DH_S_ERR
} lis3dh_status_t;

/**
 * @name Struct for data receive
 *
 * @param axis_X x-axis value
 * @param axis_Y y-axis value
 * @param axis_Z z-axis value
 */
struct lis3dh_data_t {
    int32_t axis_X;
    int32_t axis_Y;
    int32_t axis_Z;
};

/**
 * @brief Function for initialization and set default preset
 *
 * @param i2c Bus for data transfer
 *
 * @attention Call this function again if you want to change i2c bus or to set default preset
 *
 * @return LIS3DH_S_OK if everything is okay otherwise error status
 */
lis3dh_status_t LIS3DH_Init(i2c_bus_t *i2c);

/**
 * @brief Set preset for accelerometer. If you did not call this function default preset and filter will be used
 *
 * @param rate Output data rate
 * @param scale Full scale bit
 * @param accuracy Bit resolution mode
 *
 * @return LIS3DH_S_OK if everything is okay otherwise error status
 */
lis3dh_status_t LIS3DH_SetPreset(lis3dh_odr_t rate, lis3dh_fs_t scale, lis3dh_op_md_t accuracy);

/**
 * @brief Receive data from accelerometer
 *
 * @param dataStruct Pointer to store data
 *
 * @return LIS3DH_S_OK if everything is okay otherwise error status
 */
lis3dh_status_t LIS3DH_DataReceive(struct lis3dh_data_t *dataStruct);

/**
 * @brief Put accelerometer to sleep mode
 *
 * @return LIS3DH_S_OK if everything is okay otherwise error status
 */
lis3dh_status_t LIS3DH_SleepMode();
