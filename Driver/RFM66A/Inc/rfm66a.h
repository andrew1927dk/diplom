/**
 * @file rfm66a.h
 * @brief RFM driver
 * @version 0.1
 * @date 2022-12-22
 *
 *  (c) 2022
 */

#ifndef RFM66A_H
#define RFM66A_H

#include "main.h"
#include "rfm66a_defs.h"

typedef enum {
    RFM_OK,
    RFM_ERR = 1 << 0,
    RFM_ERR_SPI = 1 << 1,
    RFM_ERR_TIMEOUT = 1 << 2,
} rfm_stat_t;

typedef enum {
    RFM_MODE_STANDBY = 0x03,
    RFM_MODE_SLEEP = 0x01,
    RFM_MODE_SPI_ACTIVE = 0x02,
    RFM_MODE_READY = 0x03,
    RFM_MODE_TX_TUNE = 0x05,
    RFM_MODE_RX_TUNE = 0x06,
    RFM_MODE_TX = 0x07,
    RFM_MODE_RX = 0x08
} rfm_mode_t;

typedef enum {
    RFM_MOD_CW,
    RFM_MOD_OOK,
    RFM_MOD_2FSK,
    RFM_MOD_2GFSK,
    RFM_MOD_4FSK,
    RFM_MOD_4GFSK
} rfm_mod_t;

typedef enum {
    NO_CRC,
    ITU_T_CRC8,
    IEC_16,
    BAICHEVA_16,
    CRC_16_IBM,
    CCITT_16,
    KOOPMAN,
    IEEE_802_3,
    CASTAGNOLI,
    CRC_16_DNP
} rfm_crc_t;

typedef struct {
    uint32_t  frequency;
    uint32_t  deviation;
    uint32_t  baud_rate;
    uint16_t  preamble_len;
    uint8_t   payload_len;
    uint8_t   sync_len;
    uint8_t   sync_word[4];
    rfm_mod_t modulation;
    rfm_crc_t crc;
    uint8_t   variablePkt : 1;
} rfm_config_t;

/**
 * @brief Function for initialization and set default configuration
 * @return Status
 */
rfm_stat_t RFM_Init();

/**
 * @brief Function for set configuration
 * @return Status
 */
rfm_stat_t RFM_Config(rfm_config_t *config);

/**
 * @brief Function for set frequency
 * @param frequency  (Hz)
 * @return Status
 */
rfm_stat_t RFM_SetFrequency(uint32_t frequency);

/**
 * @brief Function for set deviation
 * @param deviation (Hz)
 * @return Status
 */
rfm_stat_t RFM_SetDeviation(uint32_t deviation);

/**
 * @brief Function for set baud rate
 * @return Status
 */
rfm_stat_t RFM_SetBaudRate(uint32_t baud_rate);

/**
 * @brief Function for set modulation
 * @return Status
 */
rfm_stat_t RFM_SetModulation(rfm_mod_t mod);

/**
 * @brief Function for set payload length
 * @param payload_len (byte)
 * @return Status
 */
rfm_stat_t RFM_SetPayloadLen(uint8_t payload_len);

/**
 * @brief Function for set preamble length
 * @param len (byte)
 * @return Status
 */
rfm_stat_t RFM_SetPreambleLen(uint8_t len);

/**
 * @brief Function for select CRC polynomial
 * @return Status
 */
rfm_stat_t RFM_SelectCRC(rfm_crc_t crc);

/**
 * @brief Function for sync word
 * @param sync_word Sync word
 * @param sync_len sync word length
 * @return Status
 */
rfm_stat_t RFM_SetSyncWord(uint8_t *sync_word, uint8_t sync_len);

/**
 * @brief Function for enable variable length packet
 * @return Status
 */
rfm_stat_t RFM_SetVarPktMode();

/**
 * @brief Function for Tx data
 * @param data Pointer to data
 * @param size Size of data
 * @return Status
 */
rfm_stat_t RFM_Tx(void *data, uint8_t size);

/**
 * @brief Function for Rx data
 * @param data Memory for received data
 * @param size Size of received data
 * @param timeout (ticks)
 * @return Status
 */
rfm_stat_t RFM_Rx(void *data, uint8_t *size, uint32_t timeout);

/**
 * @brief Function for change state
 * @return Status
 */
rfm_stat_t RFM_SetMode(rfm_mode_t mode);

/**
 * @brief Get RFM status
 * @param status Memory for chip status (5 bytes)
 * @return Status
 */
rfm_stat_t RFM_GetChipStatus(uint8_t *status);

void RFM_SPI_SemRelease();
void RFM_SemRelease();

#endif    // RFM66A_H
