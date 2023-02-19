#include "rfm66.h"
#include "rfm_registers.h"

#include "main.h"
#include "tx_api.h"

#include "extm.h"

#define MODE_MASK (0x07)

#define SPI_READ  (0x00)
#define SPI_WRITE (0x80)

#define MIN_SPI_DMA_MSG_SIZE (3)

#define MOD_MASK (0x78)

#define PREAMBLE_DECT_ON    (1 << 7)
#define PREAMBLE_DECT_1BYTE (0 << 5)
#define PREAMBLE_DECT_2BYTE (1 << 5)
#define PREAMBLE_DECT_3BYTE (2 << 5)

#define PACKET_MODE (1 << 6)

#define CRYSTAL_OSC (0x01E84800)
#define TEN_P6      (0x000F4240)

#define PA_BOOST_PIN (0x80)

#define LNA_DEFAULT (0x20)

#define AGC_ON_PREAMBLE         (1 << 2)
#define PREAMBLE_POLARITY       (1 << 5)
#define START_DEMOD_ON_PREAMBLE (1 << 1)

#define RC_OSC_OFF (0x07)

#define SYNC_ON             (0x10)
#define AUTO_RST_RX_MODE_ON (0x80)
#define CRC_OK_MASK         (0x02)

extern SPI_HandleTypeDef RFM_SPI;

static TX_SEMAPHORE semRFM;

struct {
    uint16_t payload_length;
    uint8_t  variable_pkt_length : 1;
} static _config;

/* can be inlined */
static rfm_stat_t _rfm_spi_xch_data(uint8_t *data, uint16_t size,
                                    HAL_StatusTypeDef (*fn)(SPI_HandleTypeDef *, uint8_t *, uint8_t *, uint16_t)) {
    if (fn(&RFM_SPI, data, data, size) != HAL_OK) {
        /* Error transmiting data to rfm via SPI */
        return RFM_ERR_SPI;
    }
    tx_semaphore_get(&semRFM, TX_WAIT_FOREVER);
    return RFM_OK;
}

/* NOTE: to chose direction add | with 'SPI_READ' or 'SPI_WRITE' to address
 *  example: rfm_spi_TxRx(SPI_WRITE|SomeReg, data, 3);
 */
static rfm_stat_t rfm_spi_TxRx(uint8_t addr, uint8_t *data, uint16_t size) {
    rfm_stat_t status = RFM_OK;

    HAL_GPIO_WritePin(RFM01_NSS_GPIO_Port, RFM01_NSS_Pin, GPIO_PIN_RESET);

    status |= _rfm_spi_xch_data(&addr, 1, HAL_SPI_TransmitReceive_IT);
    status |= _rfm_spi_xch_data(
        data, size, (size > MIN_SPI_DMA_MSG_SIZE ? HAL_SPI_TransmitReceive_DMA : HAL_SPI_TransmitReceive_IT));

    HAL_GPIO_WritePin(RFM01_NSS_GPIO_Port, RFM01_NSS_Pin, GPIO_PIN_SET);

    return status;
}

/* NOTE: RFM TX/RX
 *   we work only in packet mode
 *   all txrx data packets are max 64 bytes (no controling of fifo overflow)
 *   after Tx/Rx corresponding FS mode is set
 */

/* see RFM TX/RX note */
rfm_stat_t RFM_Tx(uint8_t *data, uint8_t size) {
    rfm_stat_t status = RFM_OK;

    if (_config.variable_pkt_length) {
        status |= rfm_spi_TxRx(SPI_WRITE | REG_FIFO, &size, 1);
    }

    status |= rfm_spi_TxRx(SPI_WRITE | REG_FIFO, data, size);
    RFM_SetMode(_RFM_MODE_TX);
    tx_semaphore_get(&semRFM, TX_WAIT_FOREVER);

    RFM_SetMode(RFM_MODE_FSTX);

    return status;
}

/* see RFM TX/RX note */
rfm_stat_t RFM_Rx(uint8_t *data, uint8_t *size, uint32_t timeout) {
    rfm_stat_t status = RFM_OK;

    RFM_SetMode(_RFM_MODE_RX);
    if (tx_semaphore_get(&semRFM, timeout) != TX_SUCCESS) {
        status |= RFM_ERR_TIMEOUT;
    } else {
        if (_config.variable_pkt_length) {
            status |= rfm_spi_TxRx(SPI_READ | REG_FIFO, size, 1);
        } else {
            *size = _config.payload_length;
        }
        status |= rfm_spi_TxRx(SPI_READ | REG_FIFO, data, *size);
    }

    RFM_SetMode(RFM_MODE_FSRX);

    return status;
}

static uint8_t rfm_select_ramping(uint32_t baud_rate) {

    uint32_t ramp = TEN_P6 / (baud_rate * 4);

    if (ramp <= 10) {
        return 0x0F;    // 10us
    } else if (ramp <= 12) {
        return 0x0E;    // 12us
    } else if (ramp <= 15) {
        return 0x0D;    // 15us
    } else if (ramp <= 20) {
        return 0x0C;    // 20us
    } else if (ramp <= 25) {
        return 0x0B;    // 25us
    } else if (ramp <= 31) {
        return 0x0A;    // 31us
    } else if (ramp <= 40) {
        return 0x09;    // 40us
    } else if (ramp <= 50) {
        return 0x08;    // 50us
    } else if (ramp <= 62) {
        return 0x07;    // 62us
    } else if (ramp <= 100) {
        return 0x06;    // 100us
    } else if (ramp <= 125) {
        return 0x05;    // 125us
    } else if (ramp <= 250) {
        return 0x04;    // 250us
    } else if (ramp <= 500) {
        return 0x03;    // 500us
    } else if (ramp <= 1000) {
        return 0x02;    // 1ms
    } else if (ramp <= 2000) {
        return 0x01;    // 2ms
    } else {
        return 0x00;    // 3.4ms
    }

    return 0x09;
}

/* yet we are handling only one DIO mappins same for Rx and Tx
 * DIO0 - PacketSent / PayloadReady for Tx / Rx respectively
 * as result there is no need to configure pins "on-the-fly"
 */
#define RFM_TXRX_MAPPING (0x00)

rfm_stat_t RFM_Init() {

    if (tx_semaphore_create(&semRFM, NULL, 0) != TX_SUCCESS) {
        return RFM_ERR;
    }

    return RFM_OK;
}

rfm_stat_t RFM_Config(rfm_config_t *config) {
    uint8_t tmp;

    RFM_Reset();

    _config.payload_length = config->payload_length;
    _config.variable_pkt_length = config->variable_pkt_length;

    if (RFM_SetMode(RFM_MODE_STANDBY) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Modulation mode */
    if (RFM_SetModulation(config->modulation) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Baud rate */
    if (RFM_SetBaudRate(config->baud_rate) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Deviation */
    if (RFM_SetDeviation(config->deviation) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Frequency */
    if (RFM_SetFrequency(config->frequency) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Output power */
    if (RFM_SetOutputPower(config->output_power) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Rx bandwidth */
    if (RFM_SetBandwidth(config->bandwidth) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Over current protection */
    tmp = (config->ocp_on << 5) | 0x12; /* 150 mA */
    if (rfm_spi_TxRx(SPI_WRITE | REG_OCP, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* PA ramp */
    tmp = rfm_select_ramping(config->baud_rate);
    if (rfm_spi_TxRx(SPI_WRITE | REG_PA_RAMP, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Low noise amplifier */
    tmp = LNA_DEFAULT;
    if (rfm_spi_TxRx(SPI_WRITE | REG_LNA, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Rx config */
    tmp = (config->agc_auto_on << 3) | AGC_ON_PREAMBLE | START_DEMOD_ON_PREAMBLE;
    if (rfm_spi_TxRx(SPI_WRITE | REG_RX_CONFIG, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* RC oscillator */
    tmp = RC_OSC_OFF;
    if (rfm_spi_TxRx(SPI_WRITE | REG_OSC, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Preamble detect */
    tmp = PREAMBLE_DECT_ON | PREAMBLE_DECT_2BYTE | 0x0A; /* 2 bytes (recommended) */
    if (rfm_spi_TxRx(SPI_WRITE | REG_PREAMBLE_DETECT, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }
    tmp = (uint8_t) (config->preamble_length >> 8);
    if (rfm_spi_TxRx(SPI_WRITE | REG_PREAMBLE_MSB, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }
    tmp = (uint8_t) config->preamble_length;
    if (rfm_spi_TxRx(SPI_WRITE | REG_PREAMBLE_LSB, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Sync config */
    tmp = SYNC_ON | AUTO_RST_RX_MODE_ON | ((config->sync_length - 1) & 0x07);
    if (rfm_spi_TxRx(SPI_WRITE | REG_SYNC_CONFIG, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }
    if (rfm_spi_TxRx(SPI_WRITE | REG_SYNC_VALUE1, (uint8_t *) config->sync_word, config->sync_length) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Packet config 1 */
    tmp = (config->variable_pkt_length << 7) | (config->crc_enable << 4) | (config->crc_ibm) |
          (config->crc_autoClearOff << 3);
    if (rfm_spi_TxRx(SPI_WRITE | REG_PACKET_CONFIG1, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Packet config 2 */
    tmp = PACKET_MODE | ((uint8_t) (config->payload_length >> 8) & 0x03);
    if (rfm_spi_TxRx(SPI_WRITE | REG_PACKET_CONFIG2, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Payload length */
    if (config->variable_pkt_length) {
        tmp = 0x40;
    } else {
        tmp = (uint8_t) (config->payload_length);
    }
    if (rfm_spi_TxRx(SPI_WRITE | REG_PAYLOAD_LENGTH, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Tx start condition */
    tmp = 0x80;
    if (rfm_spi_TxRx(SPI_WRITE | REG_FIFO_THRESH, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    tmp = RFM_TXRX_MAPPING;
    if (rfm_spi_TxRx(SPI_WRITE | REG_DIO_MAPPING1, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    /* Preamble detect interrupt */
    tmp = 0x01;
    if (rfm_spi_TxRx(SPI_WRITE | REG_DIO_MAPPING2, &tmp, 1) != RFM_OK) {
        return RFM_ERR_SPI;
    }

    return RFM_OK;
}

rfm_stat_t RFM_SetSyncWord(uint8_t *data, uint8_t size) {
    uint8_t tmp;
    return rfm_spi_TxRx(SPI_WRITE | REG_SYNC_VALUE1, data, size) | rfm_spi_TxRx(SPI_READ | REG_SYNC_CONFIG, &tmp, 1) |
           rfm_spi_TxRx(SPI_WRITE | REG_SYNC_CONFIG, ((tmp = (tmp & ~0x07) | ((size - 1) & 0x07)), &tmp), 1);
}

/* can be inlined */
static rfm_stat_t _rfm_set_mode_reg(uint8_t data, uint8_t mask) {
    rfm_stat_t status = RFM_OK;

    uint8_t tmp;
    status |= rfm_spi_TxRx(SPI_READ | REG_OP_MODE, &tmp, 1);
    tmp &= mask;
    tmp |= data;

    status |= rfm_spi_TxRx(SPI_WRITE | REG_OP_MODE, &tmp, 1);
    /* wait mode ready flag? */

    return status;
}

rfm_stat_t RFM_SetMode(rfm_mode_t mode) {
    return _rfm_set_mode_reg(mode, ~MODE_MASK);
}

rfm_stat_t RFM_SetFrequency(uint32_t frequency) {
    rfm_stat_t status = RFM_OK;

    rfm_freq_t frequency_value;
    frequency_value.freq = (frequency << 11) / 125;
    status |= rfm_spi_TxRx(SPI_WRITE | REG_FRF_MSB, &frequency_value.bytes.freqH, 1);
    status |= rfm_spi_TxRx(SPI_WRITE | REG_FRF_MID, &frequency_value.bytes.freqM, 1);
    status |= rfm_spi_TxRx(SPI_WRITE | REG_FRF_LSB, &frequency_value.bytes.freqL, 1);

    return status;
}

rfm_stat_t RFM_SetDeviation(uint32_t deviation) {
    rfm_stat_t status = RFM_OK;
    uint8_t    tmp;

    tmp = (uint8_t) (((((deviation / 1000) << 11) / 125) >> 8) & 0x3F);
    status |= rfm_spi_TxRx(SPI_WRITE | REG_FDEV_MSB, &tmp, 1);
    tmp = (uint8_t) ((((deviation / 1000) << 11) / 125) & 0xFF);
    status |= rfm_spi_TxRx(SPI_WRITE | REG_FDEV_LSB, &tmp, 1);

    return status;
}

rfm_stat_t RFM_SetBandwidth(rfm_bandwidth_t bandwidth) {
    rfm_stat_t status = RFM_OK;
    uint8_t    tmp;

    tmp = (uint8_t) (bandwidth);
    status |= rfm_spi_TxRx(SPI_WRITE | REG_RX_BW, &tmp, 1);

    return status;
}

rfm_stat_t RFM_SetBaudRate(uint32_t baud_rate) {
    rfm_stat_t status = RFM_OK;
    uint8_t    tmp;

    tmp = (uint8_t) ((CRYSTAL_OSC / baud_rate) >> 8);
    status |= rfm_spi_TxRx(SPI_WRITE | REG_BITRATE_MSB, &tmp, 1);
    tmp = (uint8_t) (CRYSTAL_OSC / baud_rate);
    status |= rfm_spi_TxRx(SPI_WRITE | REG_BITRATE_LSB, &tmp, 1);

    return status;
}

rfm_stat_t RFM_SetOutputPower(uint8_t output_power) {
    rfm_stat_t status = RFM_OK;
    uint8_t    tmp;

    if (output_power > 17) {
        tmp = (output_power - 5) | PA_BOOST_PIN;
        status |= rfm_spi_TxRx(SPI_WRITE | REG_PA_CONFIG, &tmp, 1);
        tmp = 0x87;
    } else if (output_power > 13) {
        tmp = (output_power - 2) | PA_BOOST_PIN;
        status |= rfm_spi_TxRx(SPI_WRITE | REG_PA_CONFIG, &tmp, 1);
        tmp = 0x83;
    } else {
        tmp = (output_power + 1) | PA_BOOST_PIN;
        status |= rfm_spi_TxRx(SPI_WRITE | REG_PA_CONFIG, &tmp, 1);
        ;
        tmp = 0x83;
    }
    status |= rfm_spi_TxRx(SPI_WRITE | REG_PA_DAC, &tmp, 1);

    return status;
}

rfm_stat_t RFM_SetModulation(rfm_mod_t mod) {
    return _rfm_set_mode_reg(mod, ~MOD_MASK);
}

void RFM_Reset() {
    HAL_GPIO_TogglePin(RFM01_RST_GPIO_Port, RFM01_RST_Pin);
    tx_thread_sleep(EXT_MS_TO_TICKS(5));    // 5ms
    HAL_GPIO_TogglePin(RFM01_RST_GPIO_Port, RFM01_RST_Pin);
    tx_thread_sleep(EXT_MS_TO_TICKS(10));    // 10ms
}

rfm_stat_t RFM_IsCrcOk() {
    rfm_stat_t status = RFM_OK;
    uint8_t    tmp;

    status |=
        rfm_spi_TxRx(SPI_READ | REG_IRQ_FLAGS2, &tmp, 1) |
        !(tmp & CRC_OK_MASK) * RFM_ERR_CRC; /* branchless version of (tmp & CRC_OK_MASK) ? RFM_OK : RFM_ERR_CRC; */

    return status;
}

void RFM_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == RFM01_RDIO0_Pin) {
        tx_semaphore_put(&semRFM);
    }
}

void RFM_SPI_CpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &RFM_SPI) {
        tx_semaphore_put(&semRFM);
    }
}
