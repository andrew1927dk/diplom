#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include "loglib.h"
#include "parser.h"
#include "usart.h"
#include "tx_api.h"

#define LOG_BUFFER_SIZE   1024
#define NEW_STR_LEN       200
#define LOG_TIMESTAMP_LEN sizeof("hhhh:mm:ss:mss ")    // Timestamp template
#define FLAG_SEND         0x00000001U
#define FLAG_COMPLETE     0x00000002U

#if LOG_COLOR_OUT
#define LOG_COLOR_PUT(_src_str, _c)          \
    do {                                     \
        strcat(_src_str, logcolorTable[_c]); \
    } while (0)
#define LOG_COLOR_PUT_OFFSET 5
#define LOG_COLOR_END(_src_str)      \
    do {                             \
        strcat(_src_str, "\x1b[0m"); \
    } while (0)
#else
#define LOG_COLOR_PUT(_src_str, _c)
#define LOG_COLOR_PUT_OFFSET 0
#define LOG_COLOR_END(_src_str)
#endif

// Table of logs types
static const char *logtypeTable[LOG_TYPES_MAX] = { "[DEBUG]", "[INFO]", "[WARN]", "[ERROR]" };

// Table of logs modules
static const char *logmoduleTable[LOG_MODS_MAX] = { "[DEFAULT]: ", "[W5500]: ",   "[RFM]: ",         "[INDICATION]: ",
                                                    "[FLASH]: ",   "[LOGGING]: ", "[MODEM]: ",       "[I2C]: ",
                                                    "[BMP390]: ",  "[ESP32]: ",   "[BOOT REASON]: ", "[LIS3DH]: ",
                                                    "[BUZZER]: ",  "[SHT40]: ",   "[DISPLAY]: ",     "[TOUCH]: " };

static log_type_t modulesPriorityTable[LOG_MODS_MAX] = { LOG_T_DEBUG };

// Table of colors' codes
static const char *logcolorTable[LOG_TYPES_MAX] = { "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m" };
static uint8_t     logBuffer[LOG_BUFFER_SIZE];
static uint16_t    logHead;
static uint16_t    logTail;
static char        str[NEW_STR_LEN];    // For new string with extended formats
static uint16_t    txLen;               // For output string length

// Definition of synchronization event flags
static ULONG                actual_flags;
static TX_EVENT_FLAGS_GROUP evfLog;

// Definition of synchronization mutex
static TX_MUTEX muxInput;
static TX_MUTEX muxOutput;

// Definition of thread
#define LOG_THREAD_STACK_SIZE 1024 * 4
static TX_THREAD thrLogHandle;

static void LOG_UART_Transmit_DMA(const uint8_t *pData, uint16_t Size) {
    HAL_UART_Transmit_DMA(&LOG_UART, pData, Size);
    tx_event_flags_get(&evfLog, FLAG_COMPLETE, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
}

static void setTimestamp(char *timeBuff) {
    memset(timeBuff, 0, LOG_TIMESTAMP_LEN);
    uint64_t ms = (uint64_t) ((1000.0 / TX_TIMER_TICKS_PER_SECOND) * tx_time_get());
    snprintf(timeBuff + strlen(timeBuff), LOG_TIMESTAMP_LEN, "%lu:%.2lu:%.2lu:%.3lu ", (uint32_t) (ms / 3600000 % 1000),
             (uint32_t) (ms / 60000 % 60), (uint32_t) (ms / 1000 % 60), (uint32_t) (ms % 1000));
}

// Output log messages thread implementation
static void StartPullLog(ULONG argument) {
    LOG_UART_Transmit_DMA((uint8_t *) "\n", 1);

    if (PARSER_GetInitState() == PARSER_INIT_OK) {
        LOG_UART_Transmit_DMA((uint8_t *) PARSER_prompt, strlen(PARSER_prompt));
    }

    while (1) {
        tx_event_flags_get(&evfLog, FLAG_SEND, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
        tx_mutex_get(&muxOutput, TX_WAIT_FOREVER);

        if (logBuffer[((logHead) ? logHead : LOG_BUFFER_SIZE) - 1] == '\n') {
            LOG_UART_Transmit_DMA((uint8_t *) "\r", 1);
        }

        if (logHead > logTail) {
            txLen = logHead - logTail;
            LOG_UART_Transmit_DMA(logBuffer + logTail, txLen);
            logTail = (logTail + txLen);
        } else if (logHead < logTail) {
            txLen = LOG_BUFFER_SIZE - logTail;
            LOG_UART_Transmit_DMA(logBuffer + logTail, txLen);
            logTail = (logTail + txLen) % LOG_BUFFER_SIZE;

            txLen = logHead;
            if (logHead != 0) {
                LOG_UART_Transmit_DMA(logBuffer, txLen);
            }
            logTail = (logTail + txLen) % LOG_BUFFER_SIZE;
        }

        if ((PARSER_GetInitState() == PARSER_INIT_OK) &&
            logBuffer[((logHead) ? logHead : LOG_BUFFER_SIZE) - 1] == '\n') {
            if (PARSER_inputStr[0]) {
                LOG_UART_Transmit_DMA((uint8_t *) PARSER_inputStr, strlen(PARSER_inputStr));
            }
        }

        tx_mutex_put(&muxOutput);
    }
}

// Initialize logging
log_state_t LOG_Init(void *memoryPoolPtr) {
    if (tx_event_flags_create(&evfLog, "LOG event flags") != TX_SUCCESS) {
        return LOG_S_ERR;
    }
    if (tx_mutex_create(&muxInput, "InputMutex", TX_NO_INHERIT) != TX_SUCCESS) {
        return LOG_S_ERR;
    }
    if (tx_mutex_create(&muxOutput, "OutputMutex", TX_NO_INHERIT) != TX_SUCCESS) {
        return LOG_S_ERR;
    }
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *) memoryPoolPtr;
    CHAR         *pointer = NULL;
    if (tx_byte_allocate(byte_pool, (void **) &pointer, LOG_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS) {
        return LOG_S_ERR;
    }
    if (tx_thread_create(&thrLogHandle, "LOG Thread", StartPullLog, 1, pointer, LOG_THREAD_STACK_SIZE, 1, 1,
                         TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS) {
        return LOG_S_ERR;
    }

    if (PARSER_Init(memoryPoolPtr) == PARSER_INIT_OK) {
        PARSER_AddCommand(LOG_Setup, "log list");
        PARSER_AddCommand(LOG_Setup, "log level -m 0");
        PARSER_AddCommand(LOG_Setup, "log set -m 0 -l 0");
        PARSER_AddCommand(help, "log help");
    }

    return LOG_S_OK;
}

static log_state_t LOG_Print(char *logMessage) {
    tx_mutex_get(&muxOutput, TX_WAIT_FOREVER);

    uint16_t logLen = strlen(logMessage);

    // Check for free space in the buffer
    if (logLen > ((logHead >= logTail) ? (LOG_BUFFER_SIZE + logTail - logHead) : (logTail - logHead))) {
        tx_mutex_put(&muxOutput);
        return LOG_S_ERR;
    }

    if (logHead + logLen < LOG_BUFFER_SIZE) {
        memcpy(logBuffer + logHead, logMessage, strlen(logMessage));
        logHead += logLen;
    } else {
        // First piece (to the end of buffer)
        memcpy(logBuffer + logHead, logMessage, LOG_BUFFER_SIZE - logHead);
        // Second piece (from the beginning of buffer)
        memcpy(logBuffer, logMessage + (LOG_BUFFER_SIZE - logHead), strlen(logMessage) - (LOG_BUFFER_SIZE - logHead));
        logHead = (logHead + logLen) % LOG_BUFFER_SIZE;
    }

    tx_mutex_put(&muxOutput);
    tx_event_flags_set(&evfLog, FLAG_SEND, TX_OR);

    return LOG_S_OK;
}

log_state_t LOG_Printf(char *logMessage, ...) {
    tx_mutex_get(&muxInput, TX_WAIT_FOREVER);

    // Check for str payload free space
    int32_t freeSpace = NEW_STR_LEN - 1;
    if (freeSpace <= 0) {
        tx_mutex_put(&muxInput);
        return LOG_S_ERR;
    }

    memset(str, 0, NEW_STR_LEN);    // Buffer clearing
    va_list arg;                    // Initializing arguments
    va_start(arg, logMessage);
    if (vsnprintf(str, freeSpace, logMessage, arg) > freeSpace) {    // Write log message to string
        va_end(arg);
        tx_mutex_put(&muxInput);
        return LOG_S_ERR;
    }
    va_end(arg);    // Closing argument list to necessary clean-up

    LOG_Print(str);    // push str to logBuffer

    tx_mutex_put(&muxInput);
    return LOG_S_OK;
}

log_state_t LOG_Put(log_type_t logType, log_module_t logModule, char *logMessage, ...) {
    tx_mutex_get(&muxInput, TX_WAIT_FOREVER);

    // Module log priority check
    if (logType < modulesPriorityTable[logModule]) {
        tx_mutex_put(&muxInput);
        return LOG_S_ERR;
    }

    // Check for str payload free space
    int32_t freeSpace = NEW_STR_LEN - (((LOG_COLOR_OUT) ? (2 * LOG_COLOR_PUT_OFFSET) : 0) + LOG_TIMESTAMP_LEN +
                                       strlen(logtypeTable[logType]) + strlen(logmoduleTable[logModule]));
    if (freeSpace <= 0) {
        tx_mutex_put(&muxInput);
        return LOG_S_ERR;
    }

    memset(str, 0, NEW_STR_LEN);               // Buffer clearing
    setTimestamp(str);                         // Set log timestamp
    LOG_COLOR_PUT(str, logType);               // Writing a color code to a string
    strcat(str, logtypeTable[logType]);        // Write log type to string
    strcat(str, logmoduleTable[logModule]);    // Write log module to string

    va_list arg;    // Initializing arguments
    va_start(arg, logMessage);
    if (vsnprintf(str + strlen(str), freeSpace, logMessage, arg) >= freeSpace) {    // Write log message to string
        va_end(arg);
        tx_mutex_put(&muxInput);
        return LOG_S_ERR;
    }
    va_end(arg);    // Closing argument list to necessary clean-up

    LOG_COLOR_END(str);
    strcat(str, "\n");

    LOG_Print(str);    // push str to logBuffer

    tx_mutex_put(&muxInput);
    return LOG_S_OK;
}

// UART Tx complete callback
void LOG_TxCpltCallback(void) {
    tx_event_flags_set(&evfLog, FLAG_COMPLETE, TX_OR);
}

void LOG_Setup(uint8_t argc, void **argv) {
    const uint8_t BUFFER_SIZE = 20;
    char          buffer[BUFFER_SIZE];
    uint8_t       m = 0, l = 0;

    switch (argc) {
        case 2:
            memset(buffer, 0, BUFFER_SIZE);
            for (uint8_t i = 0; i < strlen((const char *) argv[1]); ++i) {
                buffer[i] = toupper(((char *) argv[1])[i]);
            }
            for (l = 0;; l++) {
                if (l >= LOG_TYPES_MAX || buffer[0] == '\0') {
                    LOG_Printf("Input level unknown: \"%s\"\n", buffer);
                    return;
                }
                if (strstr(logtypeTable[l], buffer)) {
                    break;
                }
            }
        case 1:
            memset(buffer, 0, BUFFER_SIZE);
            uint8_t isFound = 0;
            for (uint8_t i = 0; i < strlen((const char *) argv[0]); ++i) {
                buffer[i] = toupper(((char *) argv[0])[i]);
            }
            for (m = 0;; m++) {
                if (m >= LOG_MODS_MAX || buffer[0] == '\0') {
                    if (!isFound) {
                        LOG_Printf("Input module unknown: \"%s\"\n", buffer);
                    }
                    return;
                }
                if (strstr(logmoduleTable[m], buffer)) {
                    if (argc == 2) {
                        modulesPriorityTable[m] = l;
                    } else {
                        LOG_Printf("%s\t%s\n", logmoduleTable[m], logtypeTable[modulesPriorityTable[m]]);
                    }
                    isFound = 1;
                }
            }
            break;

        case 0:
            LOG_Printf("\n");
            for (uint8_t i = 0; i < LOG_MODS_MAX; ++i) {
                LOG_Printf("%s\t%s\n", logmoduleTable[i], logtypeTable[modulesPriorityTable[i]]);
            }
            break;

        default:
            LOG_Printf("LOG_Setup error: wrong number of args\n");
    }
}