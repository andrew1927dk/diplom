#ifndef RFM_H
#define RFM_H

#include "main.h"

#define _RFM_RADIO_SLEEP   (0x00)
#define _RFM_RADIO_STANDBY (0x01)
#define _RFM_RADIO_FSTX    (0x02)
#define _RFM_RADIO_TX      (0x03)
#define _RFM_RADIO_FSRX    (0x04)
#define _RFM_RADIO_RX      (0x05)

#define _RFM_MOD_FSK (0 << 5)
#define _RFM_SHAPING (2 << 3)

typedef enum {
    RFM_MODE_SLEEP = _RFM_RADIO_SLEEP,
    RFM_MODE_STANDBY = _RFM_RADIO_STANDBY,
    RFM_MODE_FSTX = _RFM_RADIO_FSTX,
    _RFM_MODE_TX = _RFM_RADIO_TX,
    RFM_MODE_FSRX = _RFM_RADIO_FSRX,
    _RFM_MODE_RX = _RFM_RADIO_RX
} rfm_mode_t;

typedef enum {
    RFM_BW250kHz = 0x01,
    RFM_BW125kHz,
    RFM_BW62_5kHz,
    RFM_BW31_3kHz,
    RFM_BW15_6kHz,
    RFM_BW7_8kHz,
    RFM_BW3_9kHz,
    RFM_BW200kHz = 0x09,
    RFM_BW100kHz,
    RFM_BW50kHz,
    RFM_BW25kHz,
    RFM_BW12_5kHz,
    RFM_BW6_3kHz,
    RFM_BW3_1kHz,
    RFM_BW166_7kHz = 0x11,
    RFM_BW83_3kHz,
    RFM_BW41_7kHz,
    RFM_BW20_8kHz,
    RFM_BW10_4kHz,
    RFM_BW5_2kHz,
    RFM_BW2_6kHz
} rfm_bandwidth_t;

typedef enum {
    RFM_OK,
    RFM_ERR = 1 << 0,
    RFM_ERR_SPI = 1 << 1,
    RFM_ERR_TIMEOUT = 1 << 2,
    RFM_ERR_BUSY = 1 << 3,
    RFM_ERR_CRC = 1 << 4
} rfm_stat_t;

typedef enum {
    RFM_MOD_FSK = _RFM_MOD_FSK,
    RFM_MOD_GFSK = _RFM_MOD_FSK | _RFM_SHAPING
} rfm_mod_t;

typedef union {
    struct {
        uint8_t freqL;
        uint8_t freqM;
        uint8_t freqH;
        uint8_t freqX;
    } bytes;
    uint32_t freq;
} rfm_freq_t;

typedef struct {
    rfm_mod_t       modulation;
    rfm_bandwidth_t bandwidth;
    uint32_t        frequency;
    uint32_t        deviation;
    uint8_t         sync_word[8];
    uint8_t         sync_length;
    uint8_t         output_power;
    uint16_t        payload_length;
    uint16_t        preamble_length;
    uint32_t        baud_rate;
    uint8_t         variable_pkt_length : 1;
    uint8_t         crc_enable : 1;
    uint8_t         crc_autoClearOff : 1;
    uint8_t         agc_auto_on : 1;
    uint8_t         ocp_on : 1;
    uint8_t         crc_ibm : 1;
} rfm_config_t;

rfm_stat_t RFM_Init(void);
rfm_stat_t RFM_Config(rfm_config_t *config);
rfm_stat_t RFM_SetFrequency(uint32_t frequency);
rfm_stat_t RFM_SetDeviation(uint32_t deviation);
rfm_stat_t RFM_SetBandwidth(rfm_bandwidth_t bandwidth);
rfm_stat_t RFM_SetBaudRate(uint32_t baud_rate);
rfm_stat_t RFM_SetOutputPower(uint8_t output_power);
rfm_stat_t RFM_SetModulation(rfm_mod_t mod);
rfm_stat_t RFM_SetSyncWord(uint8_t *sync_word, uint8_t sync_len);
rfm_stat_t RFM_Tx(uint8_t *data, uint8_t size);
rfm_stat_t RFM_Rx(uint8_t *data, uint8_t *size, uint32_t timeout);
rfm_stat_t RFM_SetMode(rfm_mode_t mode);
void       RFM_Reset();
rfm_stat_t RFM_IsCrcOk();

void RFM_SPI_CpltCallback(SPI_HandleTypeDef *);
void RFM_GPIO_EXTI_Callback(uint16_t);

#endif    // RFM_H
