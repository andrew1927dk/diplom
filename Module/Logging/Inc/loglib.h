#ifndef _LOGLIB_H_
#define _LOGLIB_H_

#include <stdint.h>

// 1 - log color highlighting on; 0 - log color highlighting off
#define LOG_COLOR_OUT 1

// Specify default logging module
#ifndef LOG_DEFAULT_MODULE
#define LOG_DEFAULT_MODULE LOG_M_DEFAULT
#endif

typedef enum {
    LOG_S_OK,
    LOG_S_ERR
} log_state_t;

typedef enum {
    LOG_T_DEBUG,
    LOG_T_INFO,
    LOG_T_WARN,
    LOG_T_ERROR,
    LOG_TYPES_MAX
} log_type_t;

typedef enum {
    LOG_M_DEFAULT,
    LOG_M_W5500,
    LOG_M_RFM,
    LOG_M_INDICATION,
    LOG_M_FLASH,
    LOG_M_LOGGING,
    LOG_M_MODEM,
    LOG_M_I2C,
    LOG_M_BMP390,
    LOG_M_ESP32,
    LOB_M_BOOT,
    LOG_M_LIS3DH,
    LOG_M_BUZZER,
    LOG_M_SHT40,
    LOG_M_DISPLAY,
    LOG_M_TOUCH,
    LOG_MODS_MAX
} log_module_t;

log_state_t LOG_Init(void *memoryPoolPtr);

// Works the same as usual printf()
log_state_t LOG_Printf(char *logMessage, ...);

// Create a log message specifying the module and priority
log_state_t LOG_Put(log_type_t logType, log_module_t logModule, char *logMessage, ...);

/** In case of one parameter:
 *  1. Module name
 *
 *  In case of two parameters:
 *  1. Module name
 *  2. Level name
 */
void LOG_Setup(uint8_t argc, void **argv);
void LOG_TxCpltCallback(void);

// Create a DEBUG log message
#define LOG_DEBUG(fmt...) LOG_Put(LOG_T_DEBUG, LOG_DEFAULT_MODULE, fmt)

// Create a INFO log message
#define LOG_INFO(fmt...) LOG_Put(LOG_T_INFO, LOG_DEFAULT_MODULE, fmt)

// Create a WARN log message
#define LOG_WARN(fmt...) LOG_Put(LOG_T_WARN, LOG_DEFAULT_MODULE, fmt)

// Create a ERROR log message
#define LOG_ERROR(fmt...) LOG_Put(LOG_T_ERROR, LOG_DEFAULT_MODULE, fmt)

#endif /* _LOGLIB_H_ */