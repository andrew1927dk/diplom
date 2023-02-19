/**
 * @file web_adapter.h
 * @brief A web adapter header
 * @version 0.1
 * @date 2022-10-12
 *
 *  (c) 2022
 */

#ifndef WEB_ADAPTER_H
#define WEB_ADAPTER_H

#include "main.h"

// Data type for validating web adapter
typedef enum web_status_t {
    WEB_STATUS_OK = 0,
    WEB_STATUS_ERROR,
    WEB_STATUS_TIMEOUT,
    WEB_STATUS_BUSY,
} web_status_t;

typedef enum web_protocol_t {
    WEB_PROTOCOL_TCP = 0,
    WEB_PROTOCOL_UDP
} web_protocol_t;

// User module web interface
typedef struct _webInterface_t {
    web_status_t (*Open)(uint8_t socketNumber, uint8_t mode, uint16_t sourcePort, uint8_t flags);
    web_status_t (*Close)(uint8_t socketNumber);
    web_status_t (*Listen)(uint8_t socketNumber);
    web_status_t (*Connect)(uint8_t socketNumber, uint8_t *destinationAddress, uint16_t destinationPort);
    web_status_t (*Disconnect)(uint8_t socketNumber);
    web_status_t (*Send)(uint8_t socketNumber, uint8_t *buffer, uint16_t length);
    web_status_t (*Recv)(uint8_t socketNumber, uint8_t *buffer, uint16_t maxLength);
    web_status_t (*SendTo)(uint8_t socketNumber, uint8_t *buffer, uint16_t length, uint8_t *destinationAddress,
                           uint16_t destinationPort);
    web_status_t (*RecvFrom)(uint8_t socketNumber, uint8_t *buffer, uint16_t maxLength, uint8_t *senderAddress,
                             uint16_t senderPort);

} webInterface_t;

/**
 * @brief Set open/close methods to a web adapter socket configuration structure
 * @param module - pointer to a web interface module structure
 * @param Open - pointer to socket open function
 * @param Close - pointer to socket close function
 * @return None
 */
void WEB_RegisterTCPOpenCloseCallback(webInterface_t *module,
                                      web_status_t (*Open)(uint8_t socketNumber, uint8_t mode, uint16_t sourcePort,
                                                           uint8_t flags),
                                      web_status_t (*Close)(uint8_t socketNumber));

/**
 * @brief Set connect/disconnect methods to a web adapter socket configuration structure
 * @param module - pointer to a web interface module structure
 * @param Connect - pointer to socket connect function
 * @param Disconnect - pointer to socket disconnect function
 * @return None
 */
void WEB_RegisterTCPConnectDisconnectCallback(webInterface_t *module,
                                              web_status_t (*Connect)(uint8_t socketNumber, uint8_t *destinationAddress,
                                                                      uint16_t destinationPort),
                                              web_status_t (*Disconnect)(uint8_t socketNumber));

/**
 * @brief Set connected peer in TCP socket send/receive methods to a web adapter socket configuration structure
 * @param module - pointer to a web interface module structure
 * @param Send - pointer to send function
 * @param Recv - pointer to receive function
 * @return None
 */
void WEB_RegisterTCPSendRecvCallback(webInterface_t *module,
                                     web_status_t (*Send)(uint8_t socketNumber, uint8_t *buffer, uint16_t length),
                                     web_status_t (*Recv)(uint8_t socketNumber, uint8_t *buffer, uint16_t maxLength));

/**
 * @brief Set connection request from a client listen method to a web adapter socket configuration structure
 * @param module - pointer to a web interface module structure
 * @param Listen - pointer to listen to a connection request function
 * @return None
 */
void WEB_RegisterTCPListenCallback(webInterface_t *module, web_status_t (*Listen)(uint8_t socketNumber));

/**
 * @brief Set UDP send/receive methods to a web adapter socket configuration structure
 * @param module - pointer to web interface module structure
 * @param Send - pointer to send function
 * @param Recv - pointer to receive function
 * @return None
 */
void WEB_RegisterUDPSendRecvCallback(webInterface_t *module,
                                     web_status_t (*Send)(uint8_t socketNumber, uint8_t *buffer, uint16_t length,
                                                          uint8_t *destinationAddress, uint16_t destinationPort),
                                     web_status_t (*Recv)(uint8_t socketNumber, uint8_t *buffer, uint16_t maxLength,
                                                          uint8_t *senderAddress, uint16_t senderPort));

void WEB_Init(void);

#endif    // WEB_ADAPTER_H
