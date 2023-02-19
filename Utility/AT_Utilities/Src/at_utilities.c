/**
 * @file esp32.c
 * @brief Utilities for AT-based drivers.
 * @version 0.1
 * @date 2023-01-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "at_utilities.h"
#include "loglib.h"
#include <string.h>

atu_status_t ATU_GetNextURCSimple(uint8_t *circularBuffer, uint16_t circularBufferLength, int16_t *bufferTail,
                                  int16_t (*getHead)(), char *codeText, uint32_t codeTextSize) {
    memset(codeText, '\0', codeTextSize);
    uint16_t bufferHead = getHead();
    while (bufferHead != *bufferTail) {
        if (strncmp((char *) &circularBuffer[*bufferTail], "\r\n\r\n", 4) == 0) {
            *bufferTail += 4;
            return ATU_STATUS_OK;
        }
        if (strncmp((char *) &circularBuffer[*bufferTail], "\r\n", 2) == 0) {
            *bufferTail += 2;
            return ATU_STATUS_OK;
        }
        strncat(codeText, (char *) &circularBuffer[*bufferTail], 1);
        if (bufferHead == *bufferTail + 1) {
            tx_thread_sleep(100);
            bufferHead = getHead();
        }
        (*bufferTail)++;
        (*bufferTail) %= circularBufferLength;
    }
    if (strlen(codeText) == 0) {
        return ATU_STATUS_ERROR;
    } else {
        return ATU_STATUS_OK;
    }
}

void ATU_RemoveQuotationMarks(char *text) {
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
}

char  tmpBuffer[256];
char *ATU_AddQuotationMarks(char *text) {
    strcpy(tmpBuffer, "\"");
    strcat(tmpBuffer, text);
    strcat(tmpBuffer, "\"");
    return strdup(tmpBuffer);
}

char *ATU_ITOA(int number) {
    itoa(number, tmpBuffer, 10);
    return strdup(tmpBuffer);
}

void ATU_ArgsToArray(char *input, char **result, uint8_t *resultLength) {
    *resultLength = 0;
    char *comma;
    int   i = 0;
    do {
        comma = strstr(input, ",");
        result[i] = strndup(input, comma - input);
        input = comma + 1;
        *resultLength = *resultLength + 1;
        ATU_RemoveQuotationMarks(result[i]);
        i++;
    } while (comma != NULL);
}

void ATU_GenerateCode(char *result, char *operation, uint8_t argCount, ...) {
    va_list valist;
    va_start(valist, argCount);
    strcpy(result, "AT+");
    strcat(result, operation);
    strcat(result, "=");
    for (int i = 0; i < argCount; i++) {
        strcat(result, va_arg(valist, const char *));
        if (i != argCount - 1) {
            strcat(result, ",");
        }
    }
    strcat(result, "\r\n");
    LOG_WARN("%s", result);
    va_end(valist);
}

atu_status_t ATU_RegisterHandler(ATU_listNode_t *handlerQueueHead, char *string, void (*handlerFunction)(char *)) {
    ATU_listNode_t *newItem = malloc(sizeof(ATU_listNode_t));
    newItem->triggerString = strdup(string);
    newItem->handlerFunction = handlerFunction;
    newItem->next = NULL;
    for (ATU_listNode_t *i = handlerQueueHead; i != NULL; i = i->next) {
        if (i->next == NULL) {
            i->next = newItem;
            return ATU_STATUS_OK;
        }
    }
    return ATU_STATUS_ERROR;
}

atu_status_t ATU_ParseString(ATU_listNode_t *handlerQueueHead, char *string) {
    for (ATU_listNode_t *i = handlerQueueHead; i != NULL; i = i->next) {
        // LOG_WARN("TS: %s", i->triggerString);
        char *checker = strstr(string, i->triggerString);
        if (checker == string) {
            // LOG_DEBUG("Found: %s", string + strlen(i->triggerString));
            i->handlerFunction(string + strlen(i->triggerString));
            break;
        }
        if (i->next == NULL) {
            break;
        }
    }
    return ATU_STATUS_OK;
}

atu_status_t ATU_RegisterFlagHandler(TX_EVENT_FLAGS_GROUP flagsGroup, ATU_flagListNode_t *handlerQueueHead,
                                     char *string, ULONG flagsToSet) {
    ATU_flagListNode_t *newItem = malloc(sizeof(ATU_flagListNode_t));
    newItem->triggerString = strdup(string);
    newItem->flags = flagsToSet;
    newItem->next = NULL;
    for (ATU_flagListNode_t *i = handlerQueueHead; i != NULL; i = i->next) {
        if (i->next == NULL) {
            i->next = newItem;
            return ATU_STATUS_OK;
        }
    }
    return ATU_STATUS_ERROR;
}

atu_status_t ATU_ParseStringSetFlags(TX_EVENT_FLAGS_GROUP flagsGroup, ATU_flagListNode_t *handlerQueueHead,
                                     char *string) {
    for (ATU_flagListNode_t *i = handlerQueueHead; i != NULL; i = i->next) {
        // LOG_WARN("TS: %s", i->triggerString);
        char *checker = strstr(string, i->triggerString);
        if (checker == string) {
            // LOG_DEBUG("Found: %s", string + strlen(i->triggerString));
            tx_event_flags_set(&flagsGroup, i->flags, TX_OR);
            break;
        }
        if (i->next == NULL) {
            break;
        }
    }
    return ATU_STATUS_OK;
}