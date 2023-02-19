/**
 * @file modem_command_gen.c
 * @brief A command generator for Quectel EC21-EC modem.
 * @version 0.1
 * @date 2022-10-28
 *
 *  (c) 2022
 */

#include "modem_command_gen.h"
#define LOG_DEFAULT_MODULE LOG_M_MODEM
#include "loglib.h"

extern osEventFlagsId_t MODEM_flags;

char atCommandBuffer[256];

mod_status_t MOD_GEN_Init() {
    sprintf(atCommandBuffer, "AT+CPIN?\r");
    MOD_SendATCommand(atCommandBuffer);
    sprintf(atCommandBuffer, "AT+CREG?\r");
    MOD_SendATCommand(atCommandBuffer);
    sprintf(atCommandBuffer, "AT+CGREG?\r");
    MOD_SendATCommand(atCommandBuffer);
    sprintf(atCommandBuffer, "AT+CEREG?\r");
    MOD_SendATCommand(atCommandBuffer);
    sprintf(atCommandBuffer, "AT+QICSGP=5\r");
    MOD_SendATCommand(atCommandBuffer);
    sprintf(atCommandBuffer, "AT+CMGF=1\r");
    MOD_SendATCommand(atCommandBuffer);
    sprintf(atCommandBuffer, "AT+CPMS=\"ME\",\"ME\",\"ME\"\r");
    MOD_SendATCommand(atCommandBuffer);
    sprintf(atCommandBuffer, "AT+QICSGP=5,3,\"Internet\",\"\",\"\",1\r");
    MOD_SendATCommand(atCommandBuffer);
    osDelay(2500);
    sprintf(atCommandBuffer, "AT+QIACT=5\r");
    return MOD_SendATCommand(atCommandBuffer);
}

mod_status_t MOD_GEN_DeInit() {
    sprintf(atCommandBuffer, "AT+QIDEACT=5\r");
    return MOD_SendATCommand(atCommandBuffer);
}

mod_status_t MOD_GEN_Open(uint8_t socketID, mod_protocol_t protocol, char *remoteAddress, uint16_t remotePort,
                          uint16_t localPort, mod_access_mode_t accessMode) {
    char prt[24];
    switch (protocol) {
        case MOD_PROTOCOL_TCP:
            sprintf(prt, "TCP");
            break;
        case MOD_PROTOCOL_UDP:
            sprintf(prt, "UDP");
            break;
        case MOD_PROTOCOL_TCP_LISTENER:
            sprintf(prt, "TCP LISTENER");
            break;
        case MOD_PROTOCOL_UDP_LISTENER:
            sprintf(prt, "UDP LISTENER");
            break;
        case MOD_PROTOCOL_TCP_SERVICE:
            sprintf(prt, "TCP SERVICE");
            break;
        case MOD_PROTOCOL_UDP_SERVICE:
            sprintf(prt, "UDP SERVICE");
            break;
        default:
            return MOD_STATUS_ERROR;
            break;
    }
    sprintf(atCommandBuffer, "AT+QIOPEN=5,%d,\"%s\",\"%s\",%d,%d,%d\r", socketID, prt, remoteAddress, remotePort,
            localPort, accessMode);
    return MOD_SendATCommand(atCommandBuffer);
}

mod_status_t MOD_GEN_Close(uint8_t socketID) {
    sprintf(atCommandBuffer, "AT+QICLOSE=%d\r", socketID);
    return MOD_SendATCommand(atCommandBuffer);
}

mod_status_t MOD_GEN_Send(uint8_t socketID, uint16_t sendLength, char *textToSend) {
    sprintf(atCommandBuffer, "AT+QISEND=%d\r", socketID);
    MOD_SendATCommand(atCommandBuffer);
    osDelay(500);
    osEventFlagsWait(MODEM_flags, MOD_FLAG_PROMPT | MOD_FLAG_ERROR, 0, 10000);
    MOD_SendATCommand(textToSend);
    MOD_SendATCommand("\032");
    return MOD_STATUS_OK;
}

mod_status_t MOD_GEN_SendTo(uint8_t socketID, uint16_t sendLength, char *remoteIP, uint16_t remotePort,
                            char *textToSend) {
    return MOD_GEN_Send(socketID, sendLength, textToSend);
}

mod_status_t MOD_GEN_Recv(uint8_t socketID, uint16_t readLength) {
    sprintf(atCommandBuffer, "AT+QIRD=%d\r", socketID);
    return MOD_SendATCommand(atCommandBuffer);
}

mod_status_t MOD_GEN_RecvFrom(uint8_t socketID, uint16_t readLength) {
    return MOD_GEN_Recv(socketID, readLength);
}
