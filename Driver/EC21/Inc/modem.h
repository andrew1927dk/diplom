/**
 * @file modem.h
 * @brief An API for Quectel EC21-EC modem.
 * @version 0.1
 * @date 2022-10-26
 *
 *  (c) 2022
 */

#ifndef MODEM_H
#define MODEM_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
// #include "cmsis_os.h"
#include "main.h"

#define MOD_FLAG_RECEIVED           1 << 0
#define MOD_FLAG_SENT               1 << 1
#define MOD_FLAG_CONNECTED          1 << 2
#define MOD_FLAG_CLOSED             1 << 3
#define MOD_FLAG_SMS_RECEIVED       1 << 4
#define MOD_FLAG_SMS_READ           1 << 5
#define MOD_FLAG_OK                 1 << 6
#define MOD_FLAG_EMPTY_LINE         1 << 7
#define MOD_FLAG_PARSE_NEXT_ALLOWED 1 << 8
#define MOD_FLAG_READ               1 << 9
#define MOD_FLAG_PROMPT             1 << 10
#define MOD_FLAG_ERROR              1 << 11
#define MOD_FLAG_INIT               1 << 12

typedef enum mod_status_t {
    MOD_STATUS_OK = 0,
    MOD_STATUS_ERROR,
    MOD_STATUS_TIMEOUT,
    MOD_STATUS_BUSY,
} mod_status_t;

mod_status_t MOD_Init();
mod_status_t MOD_PowerOn();
mod_status_t MOD_SendATCommand(char *commandText);
mod_status_t MOD_GetNextURC(char *codeText, uint32_t codeTextSize);
mod_status_t MOD_GetNextURCUntilOK(char *codeText, uint32_t codeTextSize);
mod_status_t MOD_GetNextNSymbols(char *codeText, uint32_t n, uint32_t codeTextSize);
void         MOD_TxCpltCallback();

#endif    // MODEM_H