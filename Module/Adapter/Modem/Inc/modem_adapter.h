/**
 * @file modem_adapter.h
 * @brief An adapter for Quectel EC21-21.
 * @version 0.1
 * @date 2022-11-03
 *
 *  (c) 2022
 */

#ifndef MODEM_ADAPTER_H
#define MODEM_ADAPTER_H

#include "modem.h"
#include "modem_command_gen.h"
#include "modem_urc_parser.h"
#include "web_adapter.h"

mod_status_t    MOD_AdapterInit(void);
webInterface_t *MOD_GetAdapter(void);
mod_status_t    MOD_SMS_Send(char *number, char *text);
mod_status_t    MOD_SMS_Recv(void);
int             MOD_WEB_Open(uint8_t socketNumber, uint8_t mode, uint16_t sourcePort, uint8_t flags);
int             MOD_WEB_Close(uint8_t socketNumber);
int             MOD_WEB_Listen(uint8_t socketNumber);
int             MOD_WEB_Connect(uint8_t socketNumber, uint8_t *destinationAddress, uint16_t destinationPort);
int             MOD_WEB_Disconnect(uint8_t socketNumber);
int             MOD_WEB_Send(uint8_t socketNumber, uint8_t *buffer, uint32_t length);
int             MOD_WEB_Recv(uint8_t socketNumber, uint8_t *buffer, uint32_t maxLength);
int             MOD_WEB_SendTo(uint8_t socketNumber, uint8_t *buffer, uint32_t length, uint8_t *destinationAddress,
                               uint16_t destinationPort);
int             MOD_WEB_RecvFrom(uint8_t socketNumber, uint8_t *buffer, uint32_t maxLength, uint8_t *senderAddress,
                                 uint16_t senderPort);

#endif    // MODEM_ADAPTER_H