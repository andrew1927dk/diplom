/**
 * @file sht40.c
 * @brief SHT40 Humidity and temperature sensor driver
 * @version 0.1
 * @date 2023-01-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "sht40.h"
#include "main.h"
#include "platform_i2c.h"
#include "tx_api.h"

#define LOG_DEFAULT_MODULE LOG_M_SHT40
#include "loglib.h"

#define HUMID_CMD_HEATER_20MW_0_1_S  0x15
#define HUMID_CMD_HEATER_20MW_1_S    0x1E
#define HUMID_CMD_HEATER_110MW_0_1_S 0x24
#define HUMID_CMD_HEATER_110MW_1_S   0x2F
#define HUMID_CMD_HEATER_200MW_0_1_S 0x32
#define HUMID_CMD_HEATER_200MW_1_S   0x39
#define HUMID_CMD_READ_SERIAL        0x89
#define HUMID_CMD_SOFT_RESET         0x94
#define HUMID_CMD_MEAS_LPM           0xE0
#define HUMID_CMD_MEAS_MPM           0xF6
#define HUMID_CMD_MEAS_HPM           0xFD
#define HUMID_I2C_ADDR               (0x44 << 1)
#define HUMID_DELAY_LPM              2
#define HUMID_DELAY_MPM              5
#define HUMID_DELAY_HPM              9
#define HUMID_DELAY_100_MS           110
#define HUMID_DELAY_1_S              1010
#define INIT_NO                      0
#define INIT_PROBE                   1
#define INIT_DONE                    2

static i2c_bus_t        *sht_i2c;
static uint16_t          serial_num[2];
static uint32_t          inited = INIT_NO;
static humid_precision_t precision = HUMID_PRECIS_HPM;
static TX_MUTEX          humid_mux;

humid_status_t HUMID_Init(void) {
    if (sht_i2c == NULL) {
        if (I2C_GetI2CBus(&I2C, &sht_i2c) != I2C_OK) {
            LOG_ERROR("Can't find SHT40 I2C bus");
            return HUMID_S_ERR_GENERAL;
        }
    }

    if (tx_mutex_create(&humid_mux, "SHT40 mutex", TX_NO_INHERIT) != TX_SUCCESS) {
        return HUMID_S_ERR_MEMORY;
    }

    inited = INIT_PROBE;

    // Probe
    if (HUMID_ReadSerialNumber(serial_num) != HUMID_S_OK) {
        LOG_ERROR("Probe failed");
        inited = INIT_NO;
        return HUMID_S_ERR_GENERAL;
    }

    inited = INIT_DONE;

#if 0
    LOG_DEBUG("SN: %04X%04X", serial_num[0], serial_num[1]);
#endif

    if (HUMID_SetPrecision(HUMID_PRECIS_HPM) != HUMID_S_OK) {
        return HUMID_S_ERR_GENERAL;
    }

    return HUMID_S_OK;
}

humid_status_t HUMID_DeInit(void) {
    inited = INIT_NO;

    return HUMID_S_OK;
}

uint32_t HUMID_IsInited(void) {
    return (inited == INIT_DONE);
}

humid_status_t HUMID_SoftReset(void) {
    humid_status_t status = HUMID_S_ERR_GENERAL;

    if (!HUMID_IsInited()) {
        return HUMID_S_ERR_NOT_INITED;
    }

    uint8_t cmd = HUMID_CMD_SOFT_RESET;

    tx_mutex_get(&humid_mux, TX_WAIT_FOREVER);

    if (I2C_WriteBytes_IT((i2c_bus_t *) sht_i2c, HUMID_I2C_ADDR, &cmd, 1) != I2C_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }
    tx_thread_sleep(2);

    status = HUMID_S_OK;

quit:
    tx_mutex_put(&humid_mux);
    return status;
}

humid_status_t HUMID_SetPrecision(humid_precision_t pr) {
    if (!HUMID_IsInited()) {
        return HUMID_S_ERR_NOT_INITED;
    }

    if (pr < HUMID_PRECIS_LPM || pr > HUMID_PRECIS_HPM) {
        LOG_WARN("Unacceptable precision value");
        return HUMID_S_ERR_WRONG_PARAM;
    }

    precision = pr;

    return HUMID_S_OK;
}

humid_status_t HUMID_Measure(uint32_t *humidity, int32_t *temperature) {
    humid_status_t status = HUMID_S_ERR_GENERAL;

    if (!HUMID_IsInited()) {
        return HUMID_S_ERR_NOT_INITED;
    }

    uint8_t  cmd = HUMID_CMD_MEAS_LPM;
    uint32_t dly = HUMID_DELAY_LPM;
    uint8_t  val[6];

    switch (precision) {
        case HUMID_PRECIS_LPM:
            cmd = HUMID_CMD_MEAS_LPM;
            dly = HUMID_DELAY_LPM;
            break;
        case HUMID_PRECIS_MPM:
            cmd = HUMID_CMD_MEAS_MPM;
            dly = HUMID_DELAY_MPM;
            break;
        case HUMID_PRECIS_HPM:
            cmd = HUMID_CMD_MEAS_HPM;
            dly = HUMID_DELAY_HPM;
            break;
    }

    tx_mutex_get(&humid_mux, TX_WAIT_FOREVER);

    if (I2C_WriteBytes_IT(sht_i2c, HUMID_I2C_ADDR, &cmd, 1) != I2C_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }
    tx_thread_sleep(dly);
    if (I2C_ReadBytes_IT(sht_i2c, HUMID_I2C_ADDR, val, 6) != I2C_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    int32_t  rawT = val[0] << 8 | val[1];
    uint32_t rawH = val[3] << 8 | val[4];

    *humidity = ((15625 * rawH) >> 13) - 6000;
    *temperature = ((21875 * rawT) >> 13) - 45000;

    if (*humidity >= 100000) {
        *humidity = 100000;
    }

    status = HUMID_S_OK;

quit:
    tx_mutex_put(&humid_mux);
    return status;
}

humid_status_t HUMID_MeasureWithPreheat(humid_preheat_t preheat, uint32_t *humidity, int32_t *temperature) {
    humid_status_t status = HUMID_S_ERR_GENERAL;

    if (!HUMID_IsInited()) {
        return HUMID_S_ERR_NOT_INITED;
    }

    uint8_t  cmd = HUMID_CMD_MEAS_LPM;
    uint32_t dly = HUMID_DELAY_LPM;
    uint8_t  val[6];

    switch (preheat) {
        case HUMID_PREHEAT_20_MW_0_1_S:
            cmd = HUMID_CMD_HEATER_20MW_0_1_S;
            dly = HUMID_DELAY_100_MS;
            break;
        case HUMID_PREHEAT_20_MW_1_S:
            cmd = HUMID_CMD_HEATER_20MW_1_S;
            dly = HUMID_DELAY_1_S;
            break;
        case HUMID_PREHEAT_110_MW_0_1_S:
            cmd = HUMID_CMD_HEATER_110MW_0_1_S;
            dly = HUMID_DELAY_100_MS;
            break;
        case HUMID_PREHEAT_110_MW_1_S:
            cmd = HUMID_CMD_HEATER_110MW_1_S;
            dly = HUMID_DELAY_1_S;
            break;
        case HUMID_PREHEAT_200_MW_0_1_S:
            cmd = HUMID_CMD_HEATER_200MW_0_1_S;
            dly = HUMID_DELAY_100_MS;
            break;
        case HUMID_PREHEAT_200_MW_1_S:
            cmd = HUMID_CMD_HEATER_200MW_1_S;
            dly = HUMID_DELAY_1_S;
            break;
        default:
            LOG_WARN("Wrong preheat");
            return HUMID_S_ERR_WRONG_PARAM;
    }

    tx_mutex_get(&humid_mux, TX_WAIT_FOREVER);

    if (I2C_WriteBytes_IT(sht_i2c, HUMID_I2C_ADDR, &cmd, 1) != I2C_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }
    tx_thread_sleep(dly);
    if (I2C_ReadBytes_IT(sht_i2c, HUMID_I2C_ADDR, val, 6) != I2C_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    int32_t  rawT = val[0] << 8 | val[1];
    uint32_t rawH = val[3] << 8 | val[4];

    *humidity = ((15625 * rawH) >> 13) - 6000;
    *temperature = ((21875 * rawT) >> 13) - 45000;

    if (*humidity >= 100000) {
        *humidity = 100000;
    }

    status = HUMID_S_OK;

quit:
    tx_mutex_put(&humid_mux);
    return status;
}

humid_status_t HUMID_ReadSerialNumber(uint16_t *sn) {
    humid_status_t status = HUMID_S_ERR_GENERAL;

    if (inited < INIT_PROBE) {
        return HUMID_S_ERR_NOT_INITED;
    }

    uint8_t cmd = HUMID_CMD_READ_SERIAL;

    tx_mutex_get(&humid_mux, TX_WAIT_FOREVER);

    if (I2C_WriteBytes_IT((i2c_bus_t *) sht_i2c, HUMID_I2C_ADDR, &cmd, 1) != I2C_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }
    tx_thread_sleep(2);
    if (I2C_ReadBytes_IT(sht_i2c, HUMID_I2C_ADDR, (uint8_t *) serial_num, 4) != I2C_OK) {
        LOG_DEBUG("%s, %d", __FILE__, __LINE__);
        goto quit;
    }

    status = HUMID_S_OK;

quit:
    tx_mutex_put(&humid_mux);
    return status;
}