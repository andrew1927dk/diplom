/**
 * @file modem.c
 * @brief An API for Quectel EC21-EC modem.
 * @version 0.1
 * @date 2022-10-26
 *
 *  (c) 2022
 */

#include "modem.h"
#include "modem_cmd.h"
#include "modem_command_gen.h"
#include "modem_urc_parser.h"
#define LOG_DEFAULT_MODULE LOG_M_MODEM
#include "loglib.h"

#define CIRCULAR_BUFFER_LENGTH 4096

extern UART_HandleTypeDef MODEM_UART;

osEventFlagsId_t MODEM_flags;
static uint8_t   circularBuffer[CIRCULAR_BUFFER_LENGTH];
static int16_t   bufferHead = 0;
static int16_t   bufferTail = 0;
static uint8_t   modInit = 0;

/* Definitions for muxUART4 */
static osMutexId_t         muxUART4Handle;
static const osMutexAttr_t muxUART4_attributes = {
    .name = "muxUART4",
};

/* Definitions for semUART4 */
static osSemaphoreId_t         semUART4Handle;
static const osSemaphoreAttr_t semUART4_attributes = {
    .name = "semUART4",
};

mod_status_t MOD_PowerOn() {
    LOG_INFO("Modem powering on...");
    PIN_RESET(GSM_VDD_DISCHG);
    osDelay(10);
    PIN_SET(GSM_VDD_ON);
    osDelay(100);
    LOG_INFO("Modem power on OK");
    LOG_INFO("Modem switching on...");
    PIN_SET(GSM_ON);
    osDelay(600);
    PIN_RESET(GSM_ON);
    while (PIN_READ(GSM_STATUS) == GPIO_PIN_SET) {
        osDelay(333);
        LOG_DEBUG("Wait Modem status UP");
    }
    LOG_DEBUG("Modem booted OK");
    HAL_UART_Receive_DMA(&MODEM_UART, (uint8_t *) circularBuffer, CIRCULAR_BUFFER_LENGTH);
    return MOD_STATUS_OK;
}
static void startTaskRxProcessingModem(void *argument) {
    char *args[15];
    for (int i = 0; i < 15; i++) {
        args[i] = (char *) calloc(100, sizeof(char));
    }
    static char codeText[1024] = { 0 };
    uint8_t     size = 0;
    osDelay(5000);
    for (;;) {
        osDelay(50);
        osEventFlagsWait(MODEM_flags, MOD_FLAG_PARSE_NEXT_ALLOWED, osFlagsWaitAny | osFlagsNoClear, osWaitForever);
        osEventFlagsSet(MODEM_flags, MOD_FLAG_PARSE_NEXT_ALLOWED);
        if (MOD_GetNextURC(codeText, 1024) == MOD_STATUS_OK) {
            LOG_INFO("%s", codeText);
            if (MOD_URC_URCToArray(codeText, args, &size) == MOD_STATUS_OK) {
                MOD_URC_SetFlags(args, size);
            } else {
                MOD_URC_SetFlags2(codeText);
            }
            // ParseCommand(codeText);
        }
    }
}

static osThreadId_t         taskRxProcessingModemHandle;
static const osThreadAttr_t taskRxProcessingModem_attributes = {
    .name = "TaskRxProcessingModem",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};

mod_status_t MOD_Init() {
    if (modInit != 0) {
        return MOD_STATUS_OK;
    }
    /* Creation of muxSPIW5500 */
    muxUART4Handle = osMutexNew(&muxUART4_attributes);
    /* Creation of semSPIW5500 */
    semUART4Handle = osSemaphoreNew(1, 0, &semUART4_attributes);
    bufferHead = 0;
    bufferTail = 0;
    /* creation of RxProcessingTask */
    taskRxProcessingModemHandle = osThreadNew(startTaskRxProcessingModem, NULL, &taskRxProcessingModem_attributes);
    MOD_CMD_Init();

    // INIT MODEM
    osEventFlagsClear(MODEM_flags, MOD_FLAG_INIT);
    MOD_PowerOn();
    osEventFlagsWait(MODEM_flags, MOD_FLAG_INIT, 0, 15000);
    osDelay(10000);
    MOD_GEN_Init();
    modInit = 1;
    return MOD_STATUS_OK;
}

void MOD_TxCpltCallback() {
    osSemaphoreRelease(semUART4Handle);
}

mod_status_t MOD_SendATCommand(char *commandText) {
    osMutexAcquire(muxUART4Handle, osWaitForever);
    HAL_UART_Transmit_IT(&MODEM_UART, (uint8_t *) commandText, strlen(commandText));
    osSemaphoreAcquire(semUART4Handle, osWaitForever);
    osMutexRelease(muxUART4Handle);
    osDelay(100);
    return MOD_STATUS_OK;
}

mod_status_t MOD_GetNextURC(char *codeText, uint32_t codeTextSize) {
    memset(codeText, '\0', codeTextSize);
    bufferHead = CIRCULAR_BUFFER_LENGTH - MODEM_UART.hdmarx->Instance->NDTR;
    while (bufferHead != bufferTail) {
        if (strncmp((char *) &circularBuffer[bufferTail], "\r\n", 2) == 0) {
            bufferTail += 2;
            return MOD_STATUS_OK;
        }
        strncat(codeText, (char *) &circularBuffer[bufferTail], 1);
        bufferTail++;
        bufferTail %= CIRCULAR_BUFFER_LENGTH;
    }
    if (strlen(codeText) == 0) {
        return MOD_STATUS_ERROR;
    } else {
        return MOD_STATUS_OK;
    }
}

mod_status_t MOD_GetNextURCUntilOK(char *codeText, uint32_t codeTextSize) {
    memset(codeText, '\0', codeTextSize);
    bufferHead = CIRCULAR_BUFFER_LENGTH - MODEM_UART.hdmarx->Instance->NDTR;
    while (bufferHead != bufferTail) {
        if (strncmp((char *) &circularBuffer[bufferTail], "\r\nOK\r\n", 6) == 0) {
            bufferTail += 6;
            return MOD_STATUS_OK;
        } else if (strncmp((char *) &circularBuffer[bufferTail], "\r\nERROR\r\n", 9) == 0) {
            bufferTail += 9;
            return MOD_STATUS_OK;
        } else if (strncmp((char *) &circularBuffer[bufferTail], "\r\n+", 3) == 0) {
            bufferTail += 2;
            return MOD_STATUS_OK;
        }
        strncat(codeText, (char *) &circularBuffer[bufferTail], 1);
        bufferTail++;
        bufferTail %= CIRCULAR_BUFFER_LENGTH;
    }
    return MOD_STATUS_ERROR;
}

mod_status_t MOD_GetNextNSymbols(char *codeText, uint32_t n, uint32_t codeTextSize) {
    memset(codeText, '\0', codeTextSize);
    bufferHead = CIRCULAR_BUFFER_LENGTH - MODEM_UART.hdmarx->Instance->NDTR;
    uint32_t i = 0;
    while (i < n) {
        strncat(codeText, (char *) &circularBuffer[bufferTail], 1);
        bufferTail++;
        bufferTail %= CIRCULAR_BUFFER_LENGTH;
        i++;
    }
    return MOD_STATUS_OK;
}