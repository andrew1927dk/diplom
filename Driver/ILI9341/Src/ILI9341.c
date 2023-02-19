/**
 ******************************************************************************
 * @file           : ILI9341.c
 * @date           : 2022-12-21
 * @brief          : This file contains driver functions for ILI9341
 ******************************************************************************
 */

#include "ILI9341.h"
#include "tx_api.h"
#include "spi.h"
#include "main.h"

#define ILI9341_DELAY 0x80

#define ILI9341_SPI_PTR &hspi2

#define ILI9341_FLAG_SEND 0x01
#define ILI9341_FLAG_READ 0x02

static TX_EVENT_FLAGS_GROUP ILI9341_flag;
static ULONG                actual_events;

static TX_MUTEX muxILI9341;

#define ILI9341_LIST_USED_COMMANDS 20
static const uint8_t ILI9341_paramsList[] = {
    /* clang-format off */
    ILI9341_LIST_USED_COMMANDS,
    0xEF, 3, 0x03, 0x80, 0x02,
    ILI9341_CONTROL_POWER_B, 3, 0x00, 0xC1, 0x30,
    ILI9341_CONTROL_POWER_ON_SEQUENCE, 4, 0x64, 0x03, 0x12, 0x81,
    ILI9341_CONTROL_DRIVER_TIMING_A1, 3, 0x85, 0x00, 0x78,
    ILI9341_CONTROL_POWER_A, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    ILI9341_CONTROL_PUMP_RATIO, 1, 0x20,
    ILI9341_CONTROL_DRIVER_TIMING_B, 2, 0x00, 0x00,
    ILI9341_CONTROL_POWER1, 1, 0x23,
    ILI9341_CONTROL_POWER2, 1, 0x10,
    ILI9341_CONTROL_VCOM1, 2, 0x3e, 0x28,
    ILI9341_CONTROL_VCOM2, 1, 0x86,
    ILI9341_SET_SCROLLING_VERTICAL_START_ADDRESS, 1, 0x00,
    ILI9341_SET_PIXEL_FORMAT, 1, 0x55,
    ILI9341_CONTROL_FRAME_RATE_NORMAL_FULLCOLOR, 2, 0x00, 0x18,
    ILI9341_CONTROL_DISPLAY_FUNCTION, 3, 0x08, 0x82, 0x27,
    ILI9341_CONTROL_GAMMA_3GAMMA, 1, 0x00,
    ILI9341_SET_GAMMA, 1, 0x01,

    ILI9341_DISABLE_SLEEP_MODE, ILI9341_DELAY, 10,
    ILI9341_ENABLE_DISP_SCREEN, ILI9341_DELAY, 100,
    ILI9341_SET_MEMORY_ACCESS_CONTROL, 1, 0x48,
    /* clang-format on */
};

static inline void ILI9341_sendCommand(uint8_t com) {
    ILI9341_START_COMMAND;
    HAL_SPI_Transmit_IT(ILI9341_SPI_PTR, &com, 1);
    tx_event_flags_get(&ILI9341_flag, ILI9341_FLAG_SEND, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);
    ILI9341_START_DATA;
}

void ILI9341_sendData(uint8_t *data, size_t size) {
    HAL_SPI_Transmit_DMA(ILI9341_SPI_PTR, data, size);
    tx_event_flags_get(&ILI9341_flag, ILI9341_FLAG_SEND, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);
}

static inline void ILI9341_readData(uint8_t *data, size_t size) {
    HAL_SPI_Receive_IT(ILI9341_SPI_PTR, data, size);
    tx_event_flags_get(&ILI9341_flag, ILI9341_FLAG_READ, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);
}

static ILI9341_state_t ILI9341_initParams(void) {
    uint8_t        numCommands, cmd, numArgs;
    uint16_t       ms;
    const uint8_t *ptr = ILI9341_paramsList;

    tx_mutex_get(&muxILI9341, TX_WAIT_FOREVER);

    numCommands = *ptr++;

    while (numCommands--) {
        cmd = *ptr++;
        numArgs = *ptr++;
        ms = numArgs & ILI9341_DELAY;
        numArgs &= ~ILI9341_DELAY;

        ILI9341_START_WRITE;
        ILI9341_sendCommand(cmd);
        if (numArgs) {
            ILI9341_sendData((uint8_t *) ptr, numArgs);
        }
        ILI9341_END_WRITE;

        ptr += numArgs;

        if (ms) {
            ms = *ptr++;
            if (ms == 255)
                ms = 500;
            tx_thread_sleep(ms);
        }
    }

    tx_mutex_put(&muxILI9341);

    return ILI9341_OK;
}

void ILI9341_TxCpltCallback() {
    tx_event_flags_set(&ILI9341_flag, ILI9341_FLAG_SEND, TX_OR);
}

void ILI9341_RxCpltCallback() {
    tx_event_flags_set(&ILI9341_flag, ILI9341_FLAG_READ, TX_OR);
}

void ILI9341_setWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    tx_mutex_get(&muxILI9341, TX_WAIT_FOREVER);
    uint8_t xa[4] = { x >> 8, x, (x + w - 1) >> 8, (x + w - 1) };
    uint8_t ya[4] = { y >> 8, y, (y + h - 1) >> 8, (y + h - 1) };

    ILI9341_sendCommand(ILI9341_SET_COLUMN_ADDRESS);
    ILI9341_sendData(xa, 4);
    ILI9341_sendCommand(ILI9341_SET_PAGE_ADDRESS);
    ILI9341_sendData(ya, 4);
    ILI9341_sendCommand(ILI9341_WRITE_MEMORY);
    tx_mutex_put(&muxILI9341);
}

ILI9341_state_t ILI9341_init(void) {
    if (tx_mutex_create(&muxILI9341, "ILI9341 Mutex", TX_NO_INHERIT) != TX_SUCCESS) {
        return ILI9341_TX_ERR;
    }

    if (tx_event_flags_create(&ILI9341_flag, "ILI9341 Flag") != TX_SUCCESS) {
        return ILI9341_TX_ERR;
    }

    tx_mutex_get(&muxILI9341, TX_WAIT_FOREVER);
    ILI9341_RST_SET;
    tx_thread_sleep(10);
    ILI9341_RST_RESET;
    tx_thread_sleep(10);
    ILI9341_RST_SET;
    tx_thread_sleep(10);
    tx_mutex_put(&muxILI9341);

    tx_thread_sleep(150);

    return ILI9341_initParams();
}