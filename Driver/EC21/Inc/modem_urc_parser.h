/**
 * @file modem_urc_parser.h
 * @brief An URC parser for Quectel EC21-EC modem.
 * @version 0.1
 * @date 2022-10-28
 *
 *  (c) 2022
 */

#ifndef MODEM_URC_PARSER_H
#define MODEM_URC_PARSER_H

#include "modem.h"

typedef enum mod_token_type_t {
    MOD_TOKEN_TYPE_TOKEN = 0,
    MOD_TOKEN_TYPE_STRING,
    MOD_TOKEN_TYPE_NUMBER
} mod_token_type_t;

mod_status_t     MOD_URC_URCToArray(char *text, char **result, uint8_t *size);
mod_token_type_t MOD_URC_GetTokenType(char *token);
mod_status_t     MOD_URC_removeQuotationMarks(char *text);
mod_status_t     MOD_URC_SetFlags(char **args, uint8_t size);
mod_status_t     MOD_URC_SetFlags2(char *text);
void             MOD_GetResult(char *result);

#endif    // MODEM_URC_PARSER_H