/**
 * @file modem_urc_parser.c
 * @brief An URC parser for Quectel EC21-EC modem.
 * @version 0.1
 * @date 2022-10-28
 *
 *  (c) 2022
 */

#include "modem_urc_parser.h"
#include "modem_adapter.h"
#include <string.h>
#define LOG_DEFAULT_MODULE LOG_M_MODEM
#include "loglib.h"

#define RESULT_BUFFER_SIZE 4096

extern osEventFlagsId_t MODEM_flags;

char resultBuffer[RESULT_BUFFER_SIZE];

static char *smsArray[255];

mod_status_t MOD_URC_URCToArray(char *text, char **result, uint8_t *size) {
    *size = 0;
    if (text[0] != '+') {
        return MOD_STATUS_ERROR;
    }
    char *ptr1 = strstr(text, ": ");
    char *ptr2;
    strncpy(result[0], &text[1], ptr1 - text - 1);
    *size = *size + 1;
    result[0][ptr1 - text - 1] = '\0';
    ptr1 += 2;
    ptr2 = strstr(ptr1, ",");
    if (ptr2 == NULL) {
        ptr2 = text + strlen(text);
        strncpy(result[1], ptr1, ptr2 - ptr1);
        result[1][ptr2 - ptr1] = '\0';
        *size = *size + 1;
        return MOD_STATUS_OK;
    }
    strncpy(result[1], ptr1, ptr2 - ptr1);
    result[1][ptr2 - ptr1] = '\0';
    *size = *size + 1;
    int i = 2;
    while (ptr2 != NULL) {
        result[i][0] = '\0';
        ptr1 = ptr2 + 1;
        ptr2 = strstr(ptr1, ",");
        if (ptr2 == NULL) {
            ptr2 = text + strlen(text);
            strncpy(result[i], ptr1, ptr2 - ptr1);
            result[i][ptr2 - ptr1] = '\0';
            *size = *size + 1;
            return MOD_STATUS_OK;
        }
        strncpy(result[i], ptr1, ptr2 - ptr1);
        result[i][ptr2 - ptr1] = '\0';
        *size = *size + 1;
        i++;
        if (ptr2 == NULL) {
            ptr2 = text + strlen(text);
            strncpy(result[i], ptr1, ptr2 - ptr1);
            result[i][ptr2 - ptr1] = '\0';
            *size = *size + 1;
            return MOD_STATUS_OK;
        }
    }
    return MOD_STATUS_OK;
}

mod_token_type_t MOD_URC_GetTokenType(char *token) {
    if (token[0] == '"') {
        return MOD_TOKEN_TYPE_STRING;
    } else if (token[strlen(token) - 1] == '"') {
        return MOD_TOKEN_TYPE_STRING;
    } else if (isdigit((unsigned char) token[0])) {
        return MOD_TOKEN_TYPE_NUMBER;
    } else {
        return MOD_TOKEN_TYPE_TOKEN;
    }
}

mod_status_t MOD_URC_removeQuotationMarks(char *text) {
    if (text[0] == '"') {
        char *tmp = (char *) calloc(strlen(text), 1);
        strncpy(tmp, text + 1, strlen(text) - 1);
        strncpy(text, tmp, strlen(text) - 1);
        free(tmp);
        text[strlen(text) - 2] = '\0';
    }
    if (text[strlen(text) - 1] == '"') {
        text[strlen(text) - 1] = '\0';
    }
    return MOD_STATUS_OK;
}

mod_status_t MOD_URC_SetFlags(char **args, uint8_t size) {
    if (strcmp(args[0], "QIRD") == 0) {
        LOG_WARN("Read detected %s", args[0]);
        memset(resultBuffer, '\0', RESULT_BUFFER_SIZE);
        MOD_GetNextNSymbols(resultBuffer, atoi(args[1]), RESULT_BUFFER_SIZE);
        LOG_WARN("Read: %s", resultBuffer);
        osEventFlagsSet(MODEM_flags, MOD_FLAG_READ);
        osEventFlagsClear(MODEM_flags, MOD_FLAG_PARSE_NEXT_ALLOWED);
    }
    if (strcmp(args[0], "QIND") == 0) {
        if (strcmp(args[1], "PB DONE") == 0) {
            LOG_INFO("Init done detected.");
            osEventFlagsSet(MODEM_flags, MOD_FLAG_INIT);
        }
    }
    if (strcmp(args[0], "CMGL") == 0) {
        LOG_INFO("Read SMS:");
        memset(resultBuffer, 0, RESULT_BUFFER_SIZE);
        MOD_GetNextURCUntilOK(resultBuffer, RESULT_BUFFER_SIZE);
        MOD_URC_removeQuotationMarks(args[3]);

        char *tmp = (char *) calloc(1024, 1);
        sprintf(tmp, "sms_recv -id %s -n %s -d %s,%s -t ", args[1], args[3], args[5], args[6]);
        strcat(tmp, resultBuffer);
        LOG_INFO("The SMS number is: %s", args[3]);
        LOG_INFO("The SMS date is: %s,%s", args[5], args[6]);
        LOG_INFO("The SMS ID is: %s", args[1]);
        LOG_INFO("The full SMS content is: %s", resultBuffer);
        LOG_INFO("\n");
        int i = 0;
        while (smsArray[i] != 0 && i < 255) {
            i++;
        }
        smsArray[i] = tmp;

        osEventFlagsSet(MODEM_flags, MOD_FLAG_SMS_READ);
    }
    if (strcmp(args[0], "CMTI") == 0) {
        LOG_WARN("Received SMS number %s", args[2]);
        memset(resultBuffer, '\0', RESULT_BUFFER_SIZE);
        osEventFlagsSet(MODEM_flags, MOD_FLAG_SMS_RECEIVED);
        LOG_INFO("SMS Received flag has been set.");
    }
#if 0
    // An extremely good debug loop.
    for (int i = 0; i < size; i++) {
        switch (MOD_URC_GetTokenType(args[i])) {
            case MOD_TOKEN_TYPE_STRING:
                LOG_WARN("Str %s", args[i]);
                MOD_URC_removeQuotationMarks(args[i]);
                LOG_ERROR("%s", args[i]);
                break;
            case MOD_TOKEN_TYPE_NUMBER:
                LOG_WARN("Num %s", args[i]);
                break;
            case MOD_TOKEN_TYPE_TOKEN:
                LOG_WARN("Tok %s", args[i]);
                break;
            default:
                break;
        }
    }
#endif
    return MOD_STATUS_OK;
}

mod_status_t MOD_URC_SetFlags2(char *text) {
    if (strlen(text) <= 0) {
        LOG_DEBUG("The empty flag has been set.");
        osEventFlagsSet(MODEM_flags, MOD_FLAG_EMPTY_LINE);
    }
    if (((osEventFlagsGet(MODEM_flags) & MOD_FLAG_EMPTY_LINE) != 0) && (strcmp(text, "OK") == 0)) {
        osEventFlagsClear(MODEM_flags, 0x7FFFFFFF);
        osEventFlagsSet(MODEM_flags, MOD_FLAG_OK);
        LOG_DEBUG("The OK flag has been set.");
    }
    if (strcmp(text, "ERROR") == 0) {
        osEventFlagsClear(MODEM_flags, 0x7FFFFFFF);
        osEventFlagsSet(MODEM_flags, MOD_FLAG_ERROR);
        LOG_DEBUG("The ERROR flag has been set.");
    }
    if (strncmp(text, ">", 1) == 0) {
        LOG_DEBUG("The prompt flag has been set.");
        osEventFlagsSet(MODEM_flags, MOD_FLAG_PROMPT);
    }
    if (strncmp(text, "AT+QHTTPREAD", 12) == 0) {
        osEventFlagsClear(MODEM_flags, MOD_FLAG_PARSE_NEXT_ALLOWED);
        memset(resultBuffer, '\0', RESULT_BUFFER_SIZE);
        osDelay(1000);
        MOD_GetNextURCUntilOK(resultBuffer, RESULT_BUFFER_SIZE);
        osEventFlagsSet(MODEM_flags, MOD_FLAG_PARSE_NEXT_ALLOWED);
        LOG_WARN("Read: %s", resultBuffer);
    }
    return MOD_STATUS_OK;
}

void MOD_GetResult(char *result) {
    strcpy(result, resultBuffer);
}