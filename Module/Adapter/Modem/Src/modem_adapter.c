/**
 * @file modem_adapter.c
 * @brief An adapter for Quectel EC21-21.
 * @version 0.1
 * @date 2022-11-03
 *
 *  (c) 2022
 */

#include "modem_adapter.h"
#include "modem_command_gen.h"
#define LOG_DEFAULT_MODULE LOG_M_MODEM
#include "loglib.h"

extern osEventFlagsId_t MODEM_flags;

extern char  resultBuffer[];
extern char *smsArray[];
extern char  atCommandBuffer[];

static webInterface_t *adapter = NULL;

mod_status_t MOD_AdapterInit(void) {
    if (MOD_Init() != MOD_STATUS_OK) {
        return MOD_STATUS_ERROR;
    }
    if (adapter != NULL) {
        return MOD_STATUS_OK;
    }
    MODEM_flags = osEventFlagsNew(NULL);
    osEventFlagsSet(MODEM_flags, MOD_FLAG_PARSE_NEXT_ALLOWED);
    adapter = (webInterface_t *) malloc(sizeof(webInterface_t));
    WEB_RegisterTCPOpenCloseCallback(adapter, MOD_WEB_Open, MOD_WEB_Close);
    WEB_RegisterTCPConnectDisconnectCallback(adapter, MOD_WEB_Connect, MOD_WEB_Disconnect);
    WEB_RegisterTCPSendRecvCallback(adapter, MOD_WEB_Send, MOD_WEB_Recv);
    WEB_RegisterTCPListenCallback(adapter, MOD_WEB_Listen);
    WEB_RegisterUDPSendRecvCallback(adapter, MOD_WEB_SendTo, MOD_WEB_RecvFrom);
    return MOD_STATUS_OK;
}

webInterface_t *MOD_GetAdapter(void) {
    return adapter;
}

mod_status_t MOD_SMS_Send(char *number, char *text) {
    sprintf(atCommandBuffer, "AT+CMGS=\"%s\"\r", number);
    MOD_SendATCommand(atCommandBuffer);
    osEventFlagsWait(MODEM_flags, MOD_FLAG_PROMPT | MOD_FLAG_ERROR, osFlagsWaitAny, 5000);
    MOD_SendATCommand(text);
    MOD_SendATCommand("\032");
    return MOD_STATUS_OK;
}

mod_status_t MOD_SMS_Recv(void) {
    sprintf(atCommandBuffer, "AT+CMGL=\"ALL\"\r");
    MOD_SendATCommand(atCommandBuffer);
    osEventFlagsWait(MODEM_flags, MOD_FLAG_SMS_READ, 0, 5000);
    osEventFlagsClear(MODEM_flags, MOD_FLAG_SMS_READ);
    osDelay(1000);
    osEventFlagsSet(MODEM_flags, MOD_FLAG_PARSE_NEXT_ALLOWED);
    return MOD_STATUS_OK;
}

int MOD_WEB_Open(uint8_t socketNumber, uint8_t mode, uint16_t sourcePort, uint8_t flags) {
    return MOD_STATUS_OK;
}

int MOD_WEB_Close(uint8_t socketNumber) {
    return MOD_GEN_Close(socketNumber);
}

int MOD_WEB_Connect(uint8_t socketNumber, uint8_t *destinationAddress, uint16_t destinationPort) {
    return MOD_GEN_Open(socketNumber, MOD_PROTOCOL_TCP, (char *) destinationAddress, destinationPort, rand() % 1000,
                        MOD_ACCESS_MODE_BUFFERED);
}

int MOD_WEB_Disconnect(uint8_t socketNumber) {
    return MOD_GEN_Close(socketNumber);
}

int MOD_WEB_Listen(uint8_t socketNumber) {
    return MOD_GEN_Open(socketNumber, MOD_PROTOCOL_TCP_LISTENER, "127.0.0.1", 0, rand() % 1000,
                        MOD_ACCESS_MODE_BUFFERED);
}

int MOD_WEB_Send(uint8_t socketNumber, uint8_t *buffer, uint32_t length) {
    return MOD_GEN_Send(socketNumber, (uint16_t) length, (char *) buffer);
}

int MOD_WEB_SendTo(uint8_t socketNumber, uint8_t *buffer, uint32_t length, uint8_t *destinationAddress,
                   uint16_t destinationPort) {
    return MOD_GEN_SendTo(socketNumber, (uint16_t) length, (char *) destinationAddress, destinationPort,
                          (char *) buffer);
}

int MOD_WEB_Recv(uint8_t socketNumber, uint8_t *buffer, uint32_t maxLength) {
    MOD_GEN_Recv(socketNumber, maxLength);
    osEventFlagsWait(MODEM_flags, MOD_FLAG_READ, 0, 5000);
    osEventFlagsClear(MODEM_flags, MOD_FLAG_READ);
    strcpy((char *) buffer, resultBuffer);
    osEventFlagsSet(MODEM_flags, MOD_FLAG_PARSE_NEXT_ALLOWED);
    return MOD_STATUS_OK;
}

int MOD_WEB_RecvFrom(uint8_t socketNumber, uint8_t *buffer, uint32_t maxLength, uint8_t *senderAddress,
                     uint16_t senderPort) {
    MOD_GEN_Recv(socketNumber, maxLength);
    osEventFlagsWait(MODEM_flags, MOD_FLAG_READ, 0, 5000);
    osEventFlagsClear(MODEM_flags, MOD_FLAG_READ);
    strcpy((char *) buffer, resultBuffer);
    osEventFlagsSet(MODEM_flags, MOD_FLAG_PARSE_NEXT_ALLOWED);
    return MOD_STATUS_OK;
}