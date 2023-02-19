/**
 * @file modem_cmd.h
 * @brief Modem command for terminal.
 * @version 0.1
 * @date 2022-11-22
 *
 *  (c) 2022
 */

#ifndef MODEM_CMD_H
#define MODEM_CMD_H

#include <parser.h>
#include "modem.h"

void         MOD_CMD_sendSMS(uint8_t argc, void *argv[argc]);
void         MOD_CMD_readSMS(uint8_t argc, void *argv[argc]);
void         MOD_CMD_changeSIM(uint8_t argc, void *argv[argc]);
mod_status_t MOD_CMD_Init(void);

#endif    // MODEM_CMD_H