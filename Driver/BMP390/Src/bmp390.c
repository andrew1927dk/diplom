/**
 * @file bmp390.c
 * @brief barometer driver realization
 * @date 2022-12-28
 *
 *   (c) 2022
 */

#include "bmp390.h"
#include "tx_api.h"

#define LOG_DEFAULT_MODULE LOG_M_BMP390
#include "loglib.h"

#define BMP390_INITIALIZED (0x01)

/** @name I2C read/write layer */
static BMP3_INTF_RET_TYPE read_i2c(uint8_t reg, uint8_t *regData, uint32_t length, void *deviceAddr);
static BMP3_INTF_RET_TYPE write_i2c(uint8_t reg, const uint8_t *regData, uint32_t length, void *deviceAddr);

/** @name Set interface parameters */
static void interfaceInit();

/** @name ThreadX delay layer */
static void delayStart(uint32_t delay, void *intPtr);

static struct bmp3_dev      device;
static struct bmp3_settings settings = { 0 };

static uint8_t    devAddress = BMP3_ADDR_I2C_PRIM << 1;
static i2c_bus_t *i2c_bus = NULL;
static uint8_t    isInit = 0;

/**
 * @brief Function for initialization barometer and set default preset
 */
baro_status_t BARO_Init(i2c_bus_t *i2c) {
    if (i2c == NULL) {
        return BARO_S_NULL_PTR_ERR;
    }
    i2c_bus = i2c;

    if (isInit != BMP390_INITIALIZED) {
        interfaceInit();
        if (bmp3_init(&device) != BMP3_OK) {
            LOG_ERROR("Sensor and device structure init failed");
            return BARO_S_GENERAL_ERR;
        }
        isInit = BMP390_INITIALIZED;
    }

    if (BARO_SetPreset(BARO_P_STANDARD_RES, BARO_IIR_15) != BARO_S_OK) {
        LOG_WARN("Failed to set default preset");
    }

    return BARO_S_OK;
}

/**
 * @brief Put barometer to sleep mode
 */
baro_status_t BARO_TurnOff() {
    if (isInit != BMP390_INITIALIZED) {
        return BARO_S_NOT_INITED_ERR;
    }

    uint8_t currOpMode;
    bmp3_get_op_mode(&currOpMode, &device);

    if (currOpMode == BMP3_MODE_SLEEP) {
        return BARO_S_OK;
    }

    settings.op_mode = BMP3_MODE_SLEEP;
    if (bmp3_set_op_mode(&settings, &device) != BMP3_OK) {
        LOG_WARN("Failed to set sleep power status");
        return BARO_S_PWR_SET_ERR;
    }

    return BARO_S_OK;
}

/**
 * @brief Put barometer to normal work mode
 */
baro_status_t BARO_TurnOn() {
    if (isInit != BMP390_INITIALIZED) {
        return BARO_S_NOT_INITED_ERR;
    }

    uint8_t currOpMode;
    bmp3_get_op_mode(&currOpMode, &device);

    if (currOpMode == BMP3_MODE_NORMAL) {
        return BARO_S_OK;
    }

    settings.op_mode = BMP3_MODE_NORMAL;
    if (bmp3_set_op_mode(&settings, &device) != BMP3_OK) {
        LOG_WARN("Failed to set normal power status");
        return BARO_S_PWR_SET_ERR;
    }
    tx_thread_sleep(80);
    return BARO_S_OK;
}

/**
 * @brief Receive data from barometer
 */
baro_status_t BARO_DataReceive(baro_dataType_t dataType, struct baro_data_t *dataStruct) {
    if (isInit != BMP390_INITIALIZED) {
        return BARO_S_NOT_INITED_ERR;
    }

    if (dataStruct == NULL) {
        return BARO_S_NULL_PTR_ERR;
    }

    if ((dataType & BARO_D_PRESS_AND_TEMP) == 0) {
        LOG_WARN("Invalid dataType parameter");
        return BARO_S_GENERAL_ERR;
    }

    uint8_t currMode;
    bmp3_get_op_mode(&currMode, &device);
    if (currMode != BMP3_MODE_NORMAL) {
        LOG_WARN("Barometer turned off");
        return BARO_S_GET_DATA_ERR;
    }

    struct bmp3_status status = { { 0 } };
    struct bmp3_data   data = { 0 };
    bmp3_get_status(&status, &device);

    if (status.intr.drdy != BMP3_ENABLE) {
        LOG_INFO("Barometer is still busy");
        return BARO_S_OK;
    }

    if (bmp3_get_sensor_data(dataType, &data, &device) != BMP3_OK) {
        return BARO_S_GET_DATA_ERR;
    }
    bmp3_get_status(&status, &device);
    dataStruct->temperature = (int32_t) data.temperature;
    dataStruct->pressure = (uint32_t) data.pressure;
    return BARO_S_OK;
}

static void interfaceInit() {
    device.intf = BMP3_I2C_INTF;

    device.read = read_i2c;
    device.write = write_i2c;
    device.delay_us = delayStart;
    device.intf_ptr = &devAddress;
}

static BMP3_INTF_RET_TYPE read_i2c(uint8_t reg, uint8_t *regData, uint32_t length, void *deviceAddr) {
    if (I2C_Read_IT(i2c_bus, *(uint8_t *) deviceAddr, reg, regData, length) != I2C_OK) {
        return BMP3_E_COMM_FAIL;
    }
    return BMP3_OK;
}

static BMP3_INTF_RET_TYPE write_i2c(uint8_t reg, const uint8_t *regData, uint32_t length, void *deviceAddr) {
    if (I2C_Write_IT(i2c_bus, *(uint8_t *) deviceAddr, reg, (uint8_t *) regData, length) != I2C_OK) {
        return BMP3_E_COMM_FAIL;
    }
    return BMP3_OK;
}

static void delayStart(uint32_t delay, void *intPtr) {
    tx_thread_sleep(delay / 1000);
}

/**
 * @brief Set preset for barometer. If you did not call this function default preset and filter will be used
 */
baro_status_t BARO_SetPreset(baro_preset_t preset, baro_iir_filter_t filter) {
    if (isInit != BMP390_INITIALIZED) {
        return BARO_S_NOT_INITED_ERR;
    }

    uint16_t settings_select;
    uint8_t  lastOpMode;
    bmp3_get_op_mode(&lastOpMode, &device);

    if (BARO_TurnOff() != BARO_S_OK) {
        return BARO_S_PWR_SET_ERR;
    }

    settings.int_settings.drdy_en = BMP3_ENABLE;
    settings.press_en = BMP3_ENABLE;
    settings.temp_en = BMP3_ENABLE;

    settings.odr_filter.temp_os = BMP3_NO_OVERSAMPLING;
    settings.odr_filter.iir_filter = filter;

    switch (preset) {

        case BARO_P_ULTRA_LOW_RES:
            settings.odr_filter.press_os = BMP3_NO_OVERSAMPLING;
            settings.odr_filter.odr = BMP3_ODR_100_HZ;
            break;
        case BARO_P_LOW_RES:
            settings.odr_filter.press_os = BMP3_OVERSAMPLING_2X;
            settings.odr_filter.odr = BMP3_ODR_100_HZ;
            break;
        case BARO_P_STANDARD_RES:
            settings.odr_filter.press_os = BMP3_OVERSAMPLING_4X;
            settings.odr_filter.odr = BMP3_ODR_50_HZ;
            break;
        case BARO_P_HIGH_RES:
            settings.odr_filter.press_os = BMP3_OVERSAMPLING_8X;
            settings.odr_filter.odr = BMP3_ODR_25_HZ;
            break;
        case BARo_P_ULTRA_HIGH_RES:
            settings.odr_filter.press_os = BMP3_OVERSAMPLING_16X;
            settings.odr_filter.temp_os = BMP3_OVERSAMPLING_2X;
            settings.odr_filter.odr = BMP3_ODR_12_5_HZ;
            break;
        case BARO_P_HIGHEST_RES:
            settings.odr_filter.press_os = BMP3_OVERSAMPLING_32X;
            settings.odr_filter.temp_os = BMP3_OVERSAMPLING_2X;
            settings.odr_filter.odr = BMP3_ODR_12_5_HZ;
            break;
    }

    settings_select = BMP3_SEL_PRESS_EN | BMP3_SEL_TEMP_EN | BMP3_SEL_DRDY_EN | BMP3_SEL_PRESS_OS | BMP3_SEL_TEMP_OS |
                      BMP3_SEL_ODR | BMP3_SEL_IIR_FILTER;
    if (bmp3_set_sensor_settings(settings_select, &settings, &device) != BMP3_OK) {
        return BARO_S_PRESET_ERR;
    }

    if (lastOpMode == BMP3_MODE_NORMAL) {
        if (BARO_TurnOn() != BARO_S_OK) {
            return BARO_S_PWR_SET_ERR;
        }
    }
    return BARO_S_OK;
}