/**
 * @file wiznet_w5500.c
 * @brief A web adapter header
 * @version 0.1
 * @date 2022-10-12
 *
 *  (c) 2022
 */

#include "wiznet_w5500.h"
#include "spi.h"
#include "main.h"
#include "tx_api.h"

static TX_MUTEX     W5500_Mutex;
static TX_SEMAPHORE W5500_Semaphore;

W5500Status w5500Init(void) {
    tx_mutex_create(&W5500_Mutex, "Mutex Ethernet", 1);
    tx_semaphore_create(&W5500_Semaphore, "Ethernet Semaphore", 0);
    tx_thread_sleep(100);
    uint8_t value = 0x01;
    w5500WriteToRegister(W5500_REG_MR, 0x80);
    w5500WriteToRegister(W5500_REG_PHYCFGR, 0b00000000);
    w5500ReadFromRegister(W5500_REG_MR, &value);
    if (value != 0) {
        return W5500_STATUS_ERROR;
    }
    w5500WriteToRegister(W5500_REG_MR, 0x08);
    w5500ReadFromRegister(W5500_REG_MR, &value);
    if (value != 0x08) {
        return W5500_STATUS_ERROR;
    }
    w5500WriteToRegister(W5500_REG_MR, 0x10);
    w5500ReadFromRegister(W5500_REG_MR, &value);
    if (value != 0x10) {
        return W5500_STATUS_ERROR;
    }
    w5500WriteToRegister(W5500_REG_MR, 0x00);
    w5500ReadFromRegister(W5500_REG_MR, &value);
    if (value != 0x00) {
        return W5500_STATUS_ERROR;
    }
    // Read version:
    w5500ReadFromRegister(W5500_REG_VERSIONR, &value);
    if (value != 0x04) {
        return W5500_STATUS_ERROR;
    }
    // Set the sizes of the sockets:
    for (int i = 0; i < 8; i++) {
        w5500WriteToSocketRegister(i, W5500_REG_SOCK_Sn_RXBUF_SIZE, 2);
        w5500WriteToSocketRegister(i, W5500_REG_SOCK_Sn_TXBUF_SIZE, 2);
    }
    w5500WriteToSocketRegister(0, W5500_REG_SOCK_Sn_MR, 0b00000000);
    w5500WriteToRegister(W5500_REG_PHYCFGR, 0);
    w5500WriteToRegister(W5500_REG_PHYCFGR, 0b11100111);
    return W5500_STATUS_OK;
}

W5500Status w5500DeInit(void) {
    // Do reset:
    w5500WriteToRegister(W5500_REG_MR, 0x80);
    return W5500_STATUS_OK;
}

W5500Status w5500WriteToRegister(W5500CommonRegister reg, uint8_t value) {
    uint8_t txBuff[4] = { 0 }, rxBuff[4] = { 0 };
    txBuff[0] = (reg & 0xFF00) >> 8;
    txBuff[1] = (reg & 0x00FF);
    txBuff[2] = 0b00000101;
    txBuff[3] = value;
    w5500TransceiveViaSPI(txBuff, rxBuff, 4);
    return W5500_STATUS_OK;
}

W5500Status w5500ReadFromRegister(W5500CommonRegister reg, uint8_t *value) {
    uint8_t txBuff[4] = { 0 }, rxBuff[4] = { 0 };
    txBuff[0] = (reg & 0xFF00) >> 8;
    txBuff[1] = (reg & 0x00FF);
    txBuff[2] = 0b00000001;
    txBuff[3] = 0x00;
    w5500TransceiveViaSPI(txBuff, rxBuff, 4);
    *value = rxBuff[3];
    return W5500_STATUS_OK;
}

W5500Status w5500WriteToSocketRegister(uint8_t socketN, W5500SocketRegister reg, uint8_t value) {
    if (socketN < 0 || socketN > 7) {
        return W5500_STATUS_ERROR;
    }
    uint8_t txBuff[4] = { 0 }, rxBuff[4] = { 0 };
    txBuff[0] = (reg & 0xFF00) >> 8;
    txBuff[1] = (reg & 0x00FF);
    txBuff[2] = 0b00000101 + 8 + (32 * socketN);
    txBuff[3] = value;
    w5500TransceiveViaSPI(txBuff, rxBuff, 4);
    return W5500_STATUS_OK;
}

W5500Status w5500ReadFromSocketRegister(uint8_t socketN, W5500SocketRegister reg, uint8_t *value) {
    if (socketN < 0 || socketN > 7) {
        return W5500_STATUS_ERROR;
    }
    uint8_t txBuff[4] = { 0 }, rxBuff[4] = { 0 };
    txBuff[0] = (reg & 0xFF00) >> 8;
    txBuff[1] = (reg & 0x00FF);
    txBuff[2] = 0b00000001 + 8 + (32 * socketN);
    txBuff[3] = 0x00;
    w5500TransceiveViaSPI(txBuff, rxBuff, 4);
    *value = rxBuff[3];
    return W5500_STATUS_OK;
}

W5500Status w5500WriteToSocketRXBuffer(uint8_t socketN, uint16_t offset, uint8_t nOfBytes, uint8_t *buffer) {
    if (socketN < 0 || socketN > 7) {
        return W5500_STATUS_ERROR;
    }
    uint8_t txBuff[4] = { 0 }, rxBuff[4] = { 0 };
    txBuff[2] = 0b00000101 + 24 + (32 * socketN);
    for (uint16_t i = 0; i < nOfBytes; i++) {
        txBuff[0] = (offset & 0xFF00) >> 8;
        txBuff[1] = (offset & 0x00FF);
        txBuff[3] = buffer[i];
        w5500TransceiveViaSPI(txBuff, rxBuff, 4);
        offset++;
    }
    return W5500_STATUS_OK;
}

W5500Status w5500WriteToSocketTXBuffer(uint8_t socketN, uint16_t offset, uint8_t nOfBytes, uint8_t *buffer) {
    if (socketN < 0 || socketN > 7) {
        return W5500_STATUS_ERROR;
    }
    uint8_t txBuff[4] = { 0 }, rxBuff[4] = { 0 };
    txBuff[2] = 0b00000101 + 16 + (32 * socketN);
    for (uint16_t i = 0; i < nOfBytes; i++) {
        txBuff[0] = (offset & 0xFF00) >> 8;
        txBuff[1] = (offset & 0x00FF);
        txBuff[3] = buffer[i];
        w5500TransceiveViaSPI(txBuff, rxBuff, 4);
        offset++;
    }
    return W5500_STATUS_OK;
}

W5500Status w5500ReadFromSocketRXBuffer(uint8_t socketN, uint16_t offset, uint8_t nOfBytes, uint8_t *buffer) {
    if (socketN < 0 || socketN > 7) {
        return W5500_STATUS_ERROR;
    }
    uint8_t txBuff[4] = { 0 }, rxBuff[4] = { 0 };
    txBuff[2] = 0b00000001 + 24 + (32 * socketN);
    for (uint16_t i = 0; i < nOfBytes; i++) {
        txBuff[0] = (offset & 0xFF00) >> 8;
        txBuff[1] = (offset & 0x00FF);
        txBuff[3] = 0;
        w5500TransceiveViaSPI(txBuff, rxBuff, 4);
        buffer[i] = rxBuff[3];
        offset++;
    }
    return W5500_STATUS_OK;
}

W5500Status w5500ReadFromSocketTXBuffer(uint8_t socketN, uint16_t offset, uint8_t nOfBytes, uint8_t *buffer) {
    if (socketN < 0 || socketN > 7) {
        return W5500_STATUS_ERROR;
    }
    uint8_t txBuff[4] = { 0 }, rxBuff[4] = { 0 };
    txBuff[2] = 0b00000001 + 16 + (32 * socketN);
    for (uint16_t i = 0; i < nOfBytes; i++) {
        txBuff[0] = (offset & 0xFF00) >> 8;
        txBuff[1] = (offset & 0x00FF);
        txBuff[3] = 0;
        w5500TransceiveViaSPI(txBuff, rxBuff, 4);
        buffer[i] = rxBuff[3];
        offset++;
    }
    return W5500_STATUS_OK;
}

W5500Status w5500SocketExecuteCommand(uint8_t socketN, W5500SocketCommand cmd) {
    uint8_t value = 1;
    w5500WriteToSocketRegister(socketN, W5500_REG_SOCK_Sn_CR, cmd);
    do {
        w5500ReadFromSocketRegister(socketN, W5500_REG_SOCK_Sn_CR, &value);
    } while (value != 0);
    return W5500_STATUS_OK;
}

W5500Status w5500TransceiveViaSPI(uint8_t *bufferTX, uint8_t *bufferRX, uint8_t nOfBytes) {
    tx_mutex_get(&W5500_Mutex, TX_WAIT_FOREVER);
    HAL_GPIO_WritePin(ETH_NSS_GPIO_Port, ETH_NSS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive_IT(&ETH_SPI, bufferTX, bufferRX, nOfBytes);
    tx_semaphore_get(&W5500_Semaphore, TX_WAIT_FOREVER);
    HAL_GPIO_WritePin(ETH_NSS_GPIO_Port, ETH_NSS_Pin, GPIO_PIN_SET);
    tx_mutex_put(&W5500_Mutex);
    return W5500_STATUS_OK;
}

void w5500_TxRxCpltCallback() {
    tx_semaphore_put(&W5500_Semaphore);
}