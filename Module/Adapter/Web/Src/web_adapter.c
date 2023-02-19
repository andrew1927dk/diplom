/**
 * @file web_adapter.c
 * @brief A web adapter.
 * @version 0.1
 * @date 2022-10-12
 *
 *  (c) 2022
 */

#include "web_adapter.h"

void WEB_RegisterTCPOpenCloseCallback(webInterface_t *module,
                                      web_status_t (*Open)(uint8_t socketNumber, uint8_t mode, uint16_t sourcePort,
                                                           uint8_t flags),
                                      web_status_t (*Close)(uint8_t socketNumber)) {
    module->Open = Open;
    module->Close = Close;
}

void WEB_RegisterTCPConnectDisconnectCallback(webInterface_t *module,
                                              web_status_t (*Connect)(uint8_t socketNumber, uint8_t *destinationAddress,
                                                                      uint16_t destinationPort),
                                              web_status_t (*Disconnect)(uint8_t socketNumber)) {
    module->Connect = Connect;
    module->Disconnect = Disconnect;
}

void WEB_RegisterTCPSendRecvCallback(webInterface_t *module,
                                     web_status_t (*Send)(uint8_t socketNumber, uint8_t *buffer, uint16_t length),
                                     web_status_t (*Recv)(uint8_t socketNumber, uint8_t *buffer, uint16_t maxLength)) {
    module->Send = Send;
    module->Recv = Recv;
}

void WEB_RegisterTCPListenCallback(webInterface_t *module, web_status_t (*Listen)(uint8_t socketNumber)) {
    module->Listen = Listen;
}

void WEB_RegisterUDPSendRecvCallback(webInterface_t *module,
                                     web_status_t (*Send)(uint8_t socketNumber, uint8_t *buffer, uint16_t length,
                                                          uint8_t *destinationAddress, uint16_t destinationPort),
                                     web_status_t (*Recv)(uint8_t socketNumber, uint8_t *buffer, uint16_t maxLength,
                                                          uint8_t *senderAddress, uint16_t senderPort)) {
    module->SendTo = Send;
    module->RecvFrom = Recv;
}

void WEB_Init(void) {
}