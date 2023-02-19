/**
 * @file esp32.h
 * @brief Utilities for AT-based drivers.
 * @version 0.1
 * @date 2023-01-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef AT_UTILITIES_H
#define AT_UTILITIES_H

#include "tx_api.h"
#include <stdarg.h>
#include <main.h>

/**
 * @brief AT Utilities return status.
 */
typedef enum {
    ATU_STATUS_OK,
    ATU_STATUS_ERROR
} atu_status_t;

/**
 * @brief Handler queue item.
 */
typedef struct ATU_listNode_t {
    struct ATU_listNode_t *next;
    char                  *triggerString;
    void (*handlerFunction)(char *);
} ATU_listNode_t;

/**
 * @brief Handler queue item.
 */
typedef struct ATU_flagListNode_t {
    struct ATU_flagListNode_t *next;
    char                      *triggerString;
    ULONG                      flags;
} ATU_flagListNode_t;

void         ATU_RemoveQuotationMarks(char *text);
char        *ATU_AddQuotationMarks(char *text);
char        *ATU_ITOA(int number);
void         ATU_ArgsToArray(char *input, char **result, uint8_t *resultLength);
void         ATU_GenerateCode(char *result, char *operation, uint8_t argCount, ...);
atu_status_t ATU_GetNextURCSimple(uint8_t *circularBuffer, uint16_t circularBufferLength, int16_t *bufferTail,
                                  int16_t (*getHead)(), char *codeText, uint32_t codeTextSize);
atu_status_t ATU_RegisterHandler(ATU_listNode_t *handlerQueueHead, char *string, void (*handlerFunction)(char *));
atu_status_t ATU_RegisterFlagHandler(TX_EVENT_FLAGS_GROUP flagsGroup, ATU_flagListNode_t *handlerQueueHead,
                                     char *string, ULONG flagsToSet);
atu_status_t ATU_ParseString(ATU_listNode_t *handlerQueueHead, char *string);
atu_status_t ATU_ParseStringSetFlags(TX_EVENT_FLAGS_GROUP flagsGroup, ATU_flagListNode_t *handlerQueueHead,
                                     char *string);

#endif    // AT_UTILITIES_H