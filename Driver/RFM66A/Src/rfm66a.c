/**
 * @file rfm66a.c
 * @brief RFM driver
 * @version 0.1
 * @date 2022-12-22
 *
 *  (c) 2022
 */

#define LOG_DEFAULT_MODULE LOG_M_RFM

#include "main.h"
#include "rfm66a.h"
#include "rfm66a_defs.h"
#include "tx_api.h"
#include "loglib.h"

#include "extm.h"

extern SPI_HandleTypeDef hspi3;
static TX_SEMAPHORE      semRFM;
static TX_SEMAPHORE      semRFM_SPI;

static uint8_t rfmconf[] = RADIO_CONFIGURATION_DATA_ARRAY;

static _rfm_gen_conf_t genConf = {
    .f_xtal = 0x01C9C380,
    .outdiv = 0x04,
    .n_presc = 0x02,
    .payloadLen = 0x12,
    .variablePkt = 0,
};

static rfm_stat_t _rfm_spi_xch(void *data, uint16_t size,
                               HAL_StatusTypeDef (*fn)(SPI_HandleTypeDef *, uint8_t *, uint16_t)) {
    if (fn(&RFM_SPI, data, size) != HAL_OK) {
        return RFM_ERR_SPI;
    }
    tx_semaphore_get(&semRFM_SPI, TX_WAIT_FOREVER);

    return RFM_OK;
}

static rfm_stat_t rfm_spi_RW(rfm_spi_rw_t rw, uint8_t addr, void *data, uint8_t size) {
    rfm_stat_t stat = RFM_OK;
    HAL_GPIO_WritePin(ISM_NSS_GPIO_Port, ISM_NSS_Pin, GPIO_PIN_RESET);
    stat |= _rfm_spi_xch(&addr, 1, HAL_SPI_Transmit_DMA);
    stat |= _rfm_spi_xch(data, size, (rw) ? HAL_SPI_Transmit_DMA : HAL_SPI_Receive_DMA);
    HAL_GPIO_WritePin(ISM_NSS_GPIO_Port, ISM_NSS_Pin, GPIO_PIN_SET);

    return stat;
}

static rfm_stat_t rfm_spi_tx(void *data, uint8_t size) {
    rfm_stat_t stat = RFM_OK;
    HAL_GPIO_WritePin(ISM_NSS_GPIO_Port, ISM_NSS_Pin, GPIO_PIN_RESET);
    stat |= _rfm_spi_xch(data, size, HAL_SPI_Transmit_DMA);
    HAL_GPIO_WritePin(ISM_NSS_GPIO_Port, ISM_NSS_Pin, GPIO_PIN_SET);
    tx_semaphore_get(&semRFM, TX_WAIT_FOREVER);

    return stat;
}

static rfm_stat_t rfm_spi_rx(void *cmd, uint8_t cmd_size, void *data, uint8_t size) {
    rfm_stat_t stat = RFM_OK;
    stat |= rfm_spi_tx(cmd, cmd_size);
    stat |= rfm_spi_RW(rfm_Read, READ_CMD_BUFF, data, size);

    return stat;
}

rfm_stat_t RFM_Rx(void *data, uint8_t *size, uint32_t timeout) {
    rfm_stat_t  stat = RFM_OK;
    rfm_tx_rx_t cmd = {
        .cmd = START_RX,
        .channel = 0x00,
        .condition = 0x00,
        .LenMsb = 0x00,
        .LenLsb = (genConf.variablePkt) ? (0) : (genConf.payloadLen),
        .TxD_RxTO = 0x00,
        .TxRep_RxVS = 0x03,
        .RxIVs = 0x08,
    };

    stat |= rfm_spi_tx(&cmd, 8);
    if (tx_semaphore_get(&semRFM, timeout) != TX_SUCCESS) {
        stat |= RFM_SetMode(RFM_MODE_READY);
        tx_semaphore_get(&semRFM, timeout);
        stat |= RFM_ERR_TIMEOUT;
    } else {
        if (genConf.variablePkt) {
            stat |= rfm_spi_RW(rfm_Read, READ_RX_FIFO, size, 1);
        } else {
            *size = genConf.payloadLen;
        }
        stat |= rfm_spi_RW(rfm_Read, READ_RX_FIFO, data, *size);
    }

    return stat;
}

rfm_stat_t RFM_Tx(void *data, uint8_t size) {
    rfm_stat_t stat = RFM_OK;
    if (genConf.variablePkt) {
        rfm_spi_RW(rfm_Write, WRITE_TX_FIFO, &size, 1);
    }
    rfm_spi_RW(rfm_Write, WRITE_TX_FIFO, data, size);
    rfm_tx_rx_t cmd = {
        .cmd = START_TX,
        .channel = 0x00,
        .condition = 0x30,
        .LenMsb = 0x00,
        .LenLsb = size + genConf.variablePkt,
        .TxD_RxTO = 0x00,
        .TxRep_RxVS = 0x00,
    };
    stat |= rfm_spi_tx(&cmd, 7);
    tx_semaphore_get(&semRFM, TX_WAIT_FOREVER);

    return stat;
}

static rfm_stat_t _rfm_conf(uint32_t timeout) {
    uint8_t *ptr = rfmconf;
    uint8_t  tmp = *ptr;
    while (tmp) {
        if (rfm_spi_tx(ptr + 1, tmp) != RFM_OK) {
            return RFM_ERR_SPI;
        }
        tx_thread_sleep(15);
        ptr += tmp + 1;
        tmp = *ptr;
    }

    return RFM_OK;
}

rfm_stat_t RFM_Init() {
    if ((tx_semaphore_create(&semRFM, NULL, 0) != TX_SUCCESS) ||
        (tx_semaphore_create(&semRFM_SPI, NULL, 0) != TX_SUCCESS)) {
        LOG_ERROR("Semaphore create error");
        return RFM_ERR;
    }

    return _rfm_conf(TX_WAIT_FOREVER);
}

rfm_stat_t RFM_Config(rfm_config_t *config) {
    rfm_stat_t stat = RFM_OK;

    if (config->frequency > 850000) {
        genConf.outdiv = 4;
    } else if (config->frequency > 420000) {
        genConf.outdiv = 8;
    } else if (config->frequency > 350000) {
        genConf.outdiv = 10;
    } else if (config->frequency > 284000) {
        genConf.outdiv = 12;
    } else if (config->frequency > 142000) {
        genConf.outdiv = 24;
    }
    stat |= RFM_SetFrequency(config->frequency);
    stat |= RFM_SetDeviation(config->deviation);
    stat |= (config->variablePkt) ? (RFM_SetVarPktMode()) : (RFM_SetPayloadLen(config->payload_len));
    stat |= RFM_SetBaudRate(config->baud_rate);
    stat |= RFM_SetPreambleLen(config->preamble_len);
    stat |= RFM_SetPayloadLen(config->payload_len);
    stat |= RFM_SetModulation(config->modulation);
    stat |= RFM_SetSyncWord(config->sync_word, config->sync_len);
    stat |= RFM_SelectCRC(config->crc);

    return stat;
}

rfm_stat_t RFM_SetFrequency(uint32_t frequency) {
    uint32_t f_int = ((frequency * 1000) / (genConf.n_presc * genConf.f_xtal / genConf.outdiv)) - 1;
    uint32_t f_frac =
        ((((frequency * 1000) - f_int * (genConf.n_presc * genConf.f_xtal / genConf.outdiv)) / 100000) * (1 << 19)) /
        ((genConf.n_presc * genConf.f_xtal / genConf.outdiv) / 100000);
    rfm_sp_cmd_t cmd = {
        .cmd = SET_PROPERTY,
        .group = PROP_GROUP_FREQ_CONTROL,
        .num_byte = 0x04,
        .offset = 0x00,
        .arg = {f_int, EXT_GET_REG_BYTE(f_frac, 2), EXT_GET_REG_BYTE(f_frac, 1), EXT_GET_REG_BYTE(f_frac, 0)}
    };

    return rfm_spi_tx(&cmd, cmd.num_byte + 4);
}

rfm_stat_t RFM_SetDeviation(uint32_t deviation) {
    uint32_t     dev = (((1LLU << 19) * (uint64_t) deviation * genConf.outdiv) / (genConf.n_presc * genConf.f_xtal));
    rfm_sp_cmd_t cmd = {
        .cmd = SET_PROPERTY,
        .group = PROP_GROUP_MODEM,
        .num_byte = 0x03,
        .offset = 0x0A,
        .arg = {EXT_GET_REG_BYTE(dev, 2), EXT_GET_REG_BYTE(dev, 1), EXT_GET_REG_BYTE(dev, 0)}
    };

    return rfm_spi_tx(&cmd, cmd.num_byte + 4);
}

rfm_stat_t RFM_SetBaudRate(uint32_t baud_rate) {
    uint32_t NCO = genConf.f_xtal | 0x04000000;
    baud_rate *= 40;
    rfm_sp_cmd_t cmd = {
        .cmd = SET_PROPERTY,
        .group = PROP_GROUP_MODEM,
        .num_byte = 0x07,
        .offset = 0x03,
        .arg = {EXT_GET_REG_BYTE(baud_rate, 2), EXT_GET_REG_BYTE(baud_rate, 1), EXT_GET_REG_BYTE(baud_rate, 0),
                EXT_GET_REG_BYTE(NCO, 3), EXT_GET_REG_BYTE(NCO, 2), EXT_GET_REG_BYTE(NCO, 1),
                EXT_GET_REG_BYTE(NCO, 0)}
    };

    return rfm_spi_tx(&cmd, cmd.num_byte + 4);
}

rfm_stat_t RFM_SetModulation(rfm_mod_t mod) {
    rfm_sp_cmd_t cmd = {
        .cmd = SET_PROPERTY,
        .group = PROP_GROUP_MODEM,
        .num_byte = 0x01,
        .offset = 0x00,
        .arg = { mod & 0x07 },
    };

    return rfm_spi_tx(&cmd, cmd.num_byte + 4);
}

rfm_stat_t RFM_SetSyncWord(uint8_t *sync_word, uint8_t sync_len) {
    rfm_sp_cmd_t cmd = {
        .cmd = SET_PROPERTY,
        .group = PROP_GROUP_SYNC,
        .num_byte = 0x05,
        .offset = 0x00,
        .arg = { 0 },
    };
    cmd.arg[0] = (sync_len - 1) & 0x03;
    memcpy(cmd.arg + 1, sync_word, sync_len);

    return rfm_spi_tx(&cmd, cmd.num_byte + 4);
}

rfm_stat_t RFM_SetPreambleLen(uint8_t len) {
    rfm_sp_cmd_t cmd = {
        .cmd = SET_PROPERTY,
        .group = PROP_GROUP_PREAMBLE,
        .num_byte = 0x01,
        .offset = 0x00,
        .arg = { len },
    };

    return rfm_spi_tx(&cmd, cmd.num_byte + 4);
}

rfm_stat_t RFM_SelectCRC(rfm_crc_t crc) {
    rfm_sp_cmd_t cmd = {
        .cmd = SET_PROPERTY,
        .group = PROP_GROUP_PKT,
        .num_byte = 0x01,
        .offset = 0x00,
        .arg = { crc & 0x0f },
    };

    return rfm_spi_tx(&cmd, cmd.num_byte + 4);
}

rfm_stat_t RFM_SetPayloadLen(uint8_t payload_len) {
    genConf.payloadLen = payload_len;
    rfm_sp_cmd_t cmd = {
        .cmd = SET_PROPERTY,
        .group = PROP_GROUP_PKT,
        .num_byte = 0x03,
        .offset = 0x0C,
        .arg = {payload_len, 0x00, payload_len},
    };

    return rfm_spi_tx(&cmd, cmd.num_byte + 4);
}

rfm_stat_t RFM_SetVarPktMode() {
    genConf.variablePkt = 1;
    rfm_sp_cmd_t cmd = {
        .cmd = SET_PROPERTY,
        .group = PROP_GROUP_PKT,
        .num_byte = 0x04,
        .offset = 0x06,
        .arg = {0x00, 0x00, 0x0A, 0x01}
    };

    return rfm_spi_tx(&cmd, cmd.num_byte + 4);
}

rfm_stat_t RFM_SetMode(rfm_mode_t mode) {
    uint8_t cmd[2] = { CHANGE_STATE, mode & 0x0f };
    return rfm_spi_tx(cmd, sizeof(cmd));
}

rfm_stat_t RFM_GetChipStatus(uint8_t *status) {
    uint8_t cmd = GET_CHIP_STATUS;
    return rfm_spi_rx(&cmd, 1, status, 5) != RFM_OK;
}

void RFM_SPI_SemRelease() {
    tx_semaphore_put(&semRFM_SPI);
}

void RFM_SemRelease() {
    tx_semaphore_put(&semRFM);
}