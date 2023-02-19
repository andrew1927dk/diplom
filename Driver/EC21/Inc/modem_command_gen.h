/**
 * @file modem_command_gen.h
 * @brief A command generator for Quectel EC21-EC modem.
 * @version 0.1
 * @date 2022-10-28
 *
 *  (c) 2022
 */

#ifndef MODEM_COMMAND_GEN_H
#define MODEM_COMMAND_GEN_H

#include "modem.h"

typedef enum mod_protocol_t {
    MOD_PROTOCOL_TCP = 0,
    MOD_PROTOCOL_UDP,
    MOD_PROTOCOL_TCP_SERVICE,
    MOD_PROTOCOL_TCP_LISTENER,
    MOD_PROTOCOL_UDP_SERVICE,
    MOD_PROTOCOL_UDP_LISTENER,
} mod_protocol_t;

typedef enum mod_access_mode_t {
    MOD_ACCESS_MODE_BUFFERED = 0,
    MOD_ACCESS_MODE_DIRECT_PUSH = 1,
    MOD_ACCESS_MODE_TRANSPARENT = 2
} mod_access_mode_t;

mod_status_t MOD_GEN_Init();
mod_status_t MOD_GEN_DeInit();
mod_status_t MOD_GEN_Open(uint8_t socketID, mod_protocol_t protocol, char *remoteAddress, uint16_t remotePort,
                          uint16_t localPort, mod_access_mode_t accessMode);
mod_status_t MOD_GEN_Close(uint8_t socketID);
mod_status_t MOD_GEN_Send(uint8_t socketID, uint16_t sendLength, char *textToSend);
mod_status_t MOD_GEN_SendTo(uint8_t socketID, uint16_t sendLength, char *remoteIP, uint16_t remotePort,
                            char *textToSend);
mod_status_t MOD_GEN_Recv(uint8_t socketID, uint16_t readLength);
mod_status_t MOD_GEN_RecvFrom(uint8_t socketID, uint16_t readLength);

#endif    // MODEM_COMMAND_GEN_H