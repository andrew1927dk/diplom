/**
 * @file wiznet_w5500.h
 * @brief A wiznet adapter header
 * @version 0.1
 * @date 2022-10-12
 *
 *  (c) 2022
 */

#ifndef CORE_SRC_W5500_H_
#define CORE_SRC_W5500_H_

#include "main.h"
#include "spi.h"
// #include "cmsis_os2.h"
#include "w5500_registers.h"

// Status enum to be used as return values.
typedef enum W5500Status {
    W5500_STATUS_OK = 0,
    W5500_STATUS_ERROR,
    W5500_STATUS_TIMEOUT,
    W5500_STATUS_BUSY
} W5500Status;

// The enum contains all the alaviable socket mode masks.
typedef enum W5500ModeMask {
    W5500_MODE_RST = 0b10000000,
    W5500_MODE_WOL = 0b00100000,
    W5500_MODE_PB = 0b00010000,
    W5500_MODE_PPPoE = 0b00001000,
    W5500_MODE_FARP = 0b00000010
} W5500ModeMask;

// The enum contains all the alaviable interrupt masks.
typedef enum W5500InterruptMask {
    W5500_INT_CONFLICT = 0b10000000,
    W5500_INT_UNREACH = 0b01000000,
    W5500_INT_PPPoE = 0b00100000,
    W5500_INT_MP = 0b00010000
} W5500InterruptMask;

// The enum contains all the alaviable socket mode masks.
typedef enum W5500SocketModeMask {
    W5500_SOCK_MODE_MULTI_MFEN = 0b10000000,
    W5500_SOCK_MODE_BCASTB = 0b01000000,
    W5500_SOCK_MODE_ND_MC_MMB = 0b00100000,
    W5500_SOCK_MODE_UCASTB_MIP6B = 0b00010000,
    W5500_SOCK_MODE_PROTOCOL_CLOSED = 0b00000000,
    W5500_SOCK_MODE_PROTOCOL_TCP = 0b00000001,
    W5500_SOCK_MODE_PROTOCOL_UDP = 0b00000010,
    W5500_SOCK_MODE_PROTOCOL_MACRAW = 0b00000100
} W5500SocketModeMask;

// The enum contains all the alaviable socket commands.
typedef enum W5500SocketCommand {
    W5500_SOCK_CMD_OPEN = 0x0001,
    W5500_SOCK_CMD_LISTEN = 0x0002,
    W5500_SOCK_CMD_CONNECT = 0x0004,
    W5500_SOCK_CMD_DISCON = 0x0008,
    W5500_SOCK_CMD_CLOSE = 0x0010,
    W5500_SOCK_CMD_SEND = 0x0020,
    W5500_SOCK_CMD_SEND_MAC = 0x0021,
    W5500_SOCK_CMD_SEND_KEEP = 0x0022,
    W5500_SOCK_CMD_RECV = 0x0040
} W5500SocketCommand;

// The enum contains all the alaviable socket interrupt masks.
typedef enum W5500SocketInterruptMask {
    W5500_SOCK_INT_SEND_OK = 0b00010000,
    W5500_SOCK_INT_TIMEOUT = 0b00001000,
    W5500_SOCK_INT_RECV = 0b00000100,
    W5500_SOCK_INT_DISCON = 0b00000010,
    W5500_SOCK_INT_CON = 0b00000001
} W5500SocketInterruptMask;

// The enum contains all the avaliable socket status values.
typedef enum W5500SocketStatus {
    W5500_SOCK_STATUS_CLOSED = 0x0000,
    W5500_SOCK_STATUS_INIT = 0x0013,
    W5500_SOCK_STATUS_LISTEN = 0x0014,
    W5500_SOCK_STATUS_ESTABLISHED = 0x0017,
    W5500_SOCK_STATUS_CLOSE_WAIT = 0x001C,
    W5500_SOCK_STATUS_UDP = 0x0022,
    W5500_SOCK_STATUS_MACRAW = 0x0042,
    W5500_SOCK_STATUS_SYNSET = 0x0015,
    W5500_SOCK_STATUS_SYNRECV = 0x0016,
    W5500_SOCK_STATUS_FIN_WAIT = 0x0018,
    W5500_SOCK_STATUS_CLOSING = 0x001A,
    W5500_SOCK_STATUS_TIME_WAIT = 0x001B,
    W5500_SOCK_STATUS_LAST_ACK = 0x001D
} W5500SocketStatus;

/**
 *  @brief Initialize the driver.
 *  @return W5500_STATUS_ERROR, or W5500_STATUS_OK.
 */
W5500Status w5500Init(void);

/**
 *  @brief DeInitialize the driver.
 *  @return W5500_STATUS_ERROR, or W5500_STATUS_OK.
 */
W5500Status w5500DeInit(void);

/**
 *  @brief Write to the socket register specified by name.
 *  @param reg Register name from W5500CommonRegister enum.
 *  @param value Constant that will be written to the register.
 *  @return W5500_STATUS_ERROR, or W5500_STATUS_OK.
 */
W5500Status w5500WriteToRegister(W5500CommonRegister reg, uint8_t value);

/**
 *  @brief Read from the common register specified by name.
 *  @param reg Register name from W5500CommonRegister enum.
 *  @param value Pointer to variable that will be set to the value read from the register.
 *  @return W5500_STATUS_ERROR, or W5500_STATUS_OK.
 */
W5500Status w5500ReadFromRegister(W5500CommonRegister reg, uint8_t *value);

/**
 *  @brief Write to the socket register specified by name.
 *  @param socketN Socket number from 0 to 7.
 *  @param reg Register name from W5500SocketRegister enum.
 *  @param value Pointer to variable that will be set to the value read from the register.
 *  @return W5500_STATUS_ERROR, or W5500_STATUS_OK.
 */
W5500Status w5500WriteToSocketRegister(uint8_t socketN, W5500SocketRegister reg, uint8_t value);

/**
 *  @brief Read from the socket register specified by name.
 *  @param socketN Socket number from 0 to 7.
 *  @param reg Register name from W5500Socketegister enum.
 *  @param value Constant that will be written to the register.
 *  @return W5500_STATUS_ERROR, or W5500_STATUS_OK.
 */
W5500Status w5500ReadFromSocketRegister(uint8_t socketN, W5500SocketRegister reg, uint8_t *value);

/**
 *  @brief Write to the RX buffer of the n-th socket.
 *  @param socketN Socket number from 0 to 7.
 *  @param offset Offset in RX buffer.
 *  @param nOfBytes Number of bytes to write to the socket RX buffer.
 *  @param buffer Pointer to uint8_t buffer that contains the data that will be written to the RX buffer.
 *  @return W5500_STATUS_ERROR, or W5500_STATUS_OK.
 */
W5500Status w5500WriteToSocketRXBuffer(uint8_t socketN, uint16_t offset, uint8_t nOfBytes, uint8_t *buffer);

/**
 *  @brief Write to the TX buffer of the n-th socket.
 *  @param socketN Socket number from 0 to 7.
 *  @param offset Offset in TX buffer.
 *  @param nOfBytes Number of bytes to write to the socket TX buffer.
 *  @param buffer Pointer to uint8_t buffer that contains the data that will be written to the RX buffer.
 *  @return W5500_STATUS_ERROR, or W5500_STATUS_OK.
 */
W5500Status w5500WriteToSocketTXBuffer(uint8_t socketN, uint16_t offset, uint8_t nOfBytes, uint8_t *buffer);

/**
 *  @brief Read from the RX buffer of the n-th socket.
 *  @param socketN Socket number from 0 to 7.
 *  @param offset Offset in RX buffer.
 *  @param nOfBytes Number of bytes to read from the socket RX buffer.
 *  @param buffer Pointer to uint8_t buffer that will contain the data that will be read from the RX buffer.
 *  @return W5500_STATUS_ERROR, or W5500_STATUS_OK.
 */
W5500Status w5500ReadFromSocketRXBuffer(uint8_t socketN, uint16_t offset, uint8_t nOfBytes, uint8_t *buffer);

/**
 *  @brief Read from the TX buffer of the n-th socket.
 *  @param socketN Socket number from 0 to 7.
 *  @param offset Offset in TX buffer.
 *  @param nOfBytes Number of bytes to read from the socket TX buffer.
 *  @param buffer Pointer to uint8_t buffer that will contain the data that will be read from the RX buffer.
 *  @return W5500_STATUS_ERROR, or W5500_STATUS_OK.
 */
W5500Status w5500ReadFromSocketTXBuffer(uint8_t socketN, uint16_t offset, uint8_t nOfBytes, uint8_t *buffer);

/**
 *  @brief Executes the socket command specified by the name.
 *  @param socketN Socket number from 0 to 7.
 *  @param cmd The name of the command from the socket command enum to be executed.
 *  @return W5500_STATUS_ERROR, or W5500_STATUS_OK.
 */
W5500Status w5500SocketExecuteCommand(uint8_t socketN, W5500SocketCommand cmd);

/**
 *  @brief Sends information to W5500 using SPI protocol.
 *  @param bufferTX Pointer to uint8_t buffer that contains the data to be transmitted.
 *  @param bufferRX Pointer to uint8_t buffer to write the received data.
 *  @param nOfBytes Sets the length of the buffers (in bytes).
 *  @return W5500_STATUS_ERROR, or W5500_STATUS_OK.
 */
W5500Status w5500TransceiveViaSPI(uint8_t *bufferTX, uint8_t *bufferRX, uint8_t nOfBytes);

/**
 *  @brief Releases the SPI semaphore.
 *  @return None.
 */
void w5500_TxRxCpltCallback();

#endif /* CORE_SRC_W5500_H_ */
