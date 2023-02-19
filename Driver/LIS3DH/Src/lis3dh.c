/**
 * @file lis3dh.с
 * @brief accelerometer driver realization
 * @date 2023-01-13
 *
 *   (c) 2022
 */

#include "lis3dh.h"
#include "lis3dh_reg.h"
#include "platform_i2c.h"

#define LOG_DEFAULT_MODULE LOG_M_LIS3DH
#include "loglib.h"

#define LIS3DH_INITIALIZED (0x01)

static i2c_bus_t *i2c_bus = NULL;
static uint8_t    isInit = 0;

static stmdev_ctx_t device;
static int16_t      data_raw_acceleration[3];
static uint8_t      whoamI;

static int32_t  platformWriteReg(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);
static int32_t  platformReadReg(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static void     platformDelay(uint32_t ms);
lis3dh_status_t LIS3DH_SetPreset(lis3dh_odr_t rate, lis3dh_fs_t scale, lis3dh_op_md_t accuracy);

/**
 * @brief Function for accelerometer initialization and set default preset
 */
lis3dh_status_t LIS3DH_Init(i2c_bus_t *i2c) {
    if (i2c == NULL) {
        return LIS3DH_S_ERR;
    }
    i2c_bus = i2c;

    if (isInit != LIS3DH_INITIALIZED) {
        device.write_reg = platformWriteReg;
        device.read_reg = platformReadReg;
        device.mdelay = platformDelay;
        device.handle = i2c->hi2c;

        lis3dh_device_id_get(&device, &whoamI);
        if (whoamI != LIS3DH_ID) {
            LOG_ERROR("Init failed");
            return LIS3DH_S_ERR;
        }
        isInit = LIS3DH_INITIALIZED;
    }

    if (LIS3DH_SetPreset(LIS3DH_ODR_400Hz, LIS3DH_2g, LIS3DH_HR_12bit) != LIS3DH_S_OK) {
        LOG_WARN("Failed to set default preset");
    }

    return LIS3DH_S_OK;
}

/**
 * @brief Set preset for accelerometer. If you did not call this function default preset will be used
 */
lis3dh_status_t LIS3DH_SetPreset(lis3dh_odr_t rate, lis3dh_fs_t scale, lis3dh_op_md_t accuracy) {
    if (isInit != LIS3DH_INITIALIZED) {
        return LIS3DH_S_ERR;
    }

    /* Enable Block Data Update. */
    lis3dh_block_data_update_set(&device, PROPERTY_ENABLE);

    /* Set Output Data Rate to 1Hz. */
    lis3dh_data_rate_set(&device, rate);

    /* Set full scale to 2g. */
    lis3dh_full_scale_set(&device, scale);

    /* Set device in continuous mode with 12 bit resol. */
    lis3dh_operating_mode_set(&device, accuracy);

    return LIS3DH_S_OK;
}

/**
 * @brief Receive data from accelerometer
 */
lis3dh_status_t LIS3DH_DataReceive(struct lis3dh_data_t *dataStruct) {
    if (isInit != LIS3DH_INITIALIZED) {
        return LIS3DH_S_ERR;
    }

    if (dataStruct == NULL) {
        return LIS3DH_S_ERR;
    }

    lis3dh_reg_t reg;
    /* Read output only if new value available */
    lis3dh_xl_data_ready_get(&device, &reg.byte);

    if (reg.byte) {
        /* Read accelerometer data */
        memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
        lis3dh_acceleration_raw_get(&device, data_raw_acceleration);
        dataStruct->axis_X = lis3dh_from_fs2_hr_to_mg(data_raw_acceleration[0]);
        dataStruct->axis_Y = lis3dh_from_fs2_hr_to_mg(data_raw_acceleration[1]);
        dataStruct->axis_Z = lis3dh_from_fs2_hr_to_mg(data_raw_acceleration[2]);
    } else {
        lis3dh_odr_t currMode;
        lis3dh_data_rate_get(&device, &currMode);
        if (currMode != LIS3DH_POWER_DOWN) {
            LOG_INFO("Accelerometer is still busy");

        } else {
            LOG_WARN("Accelerometer turned off");
            return LIS3DH_S_ERR;
        }
    }

    return LIS3DH_S_OK;
}

/**
 * @brief Put accelerometr to sleep mode
 */
lis3dh_status_t LIS3DH_SleepMode() {
    if (isInit != LIS3DH_INITIALIZED) {
        return LIS3DH_S_ERR;
    }

    lis3dh_odr_t currOdrMode;
    lis3dh_data_rate_get(&device, &currOdrMode);

    if (currOdrMode == LIS3DH_POWER_DOWN) {
        return LIS3DH_S_OK;
    }

    if (lis3dh_data_rate_set(&device, LIS3DH_POWER_DOWN) != LIS3DH_S_OK) {
        LOG_WARN("Failed to set sleep power status");
        return LIS3DH_S_ERR;
    }

    return LIS3DH_S_OK;
}

/*
 * @brief  Write generic device registe
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t platformWriteReg(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len) {
    reg |= 0x80;

    I2C_Write_IT(i2c_bus, LIS3DH_I2C_ADD_L, reg, (uint8_t *) bufp, 1);

    return 0;
}

/*
 * @brief  Read generic device register
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platformReadReg(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len) {
    reg |= 0x80;

    I2C_Read_IT(i2c_bus, LIS3DH_I2C_ADD_L, reg, bufp, len);

    return 0;
}

/*
 * @brief   delay
 *
 * @param  ms   delay in ms
 */
static void platformDelay(uint32_t ms) {
    tx_thread_sleep(ms);
}