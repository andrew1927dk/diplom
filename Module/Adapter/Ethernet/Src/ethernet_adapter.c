/**
 * @file web_adapter.c
 * @brief An ethernet adapter.
 * @version 0.1
 * @date 2022-10-12
 *
 *  (c) 2022
 */

#include <stdlib.h>
#include "ethernet_adapter.h"
#include "wiznet_w5500.h"
#include "wizchip.h"
#include "socket.h"
#include "ctype.h"
#include "string.h"
#include "Internet/DHCP/dhcp.h"
#include "Internet/DNS/dns.h"
#define LOG_DEFAULT_MODULE LOG_M_W5500
#include "loglib.h"

static uint8_t         DNSBuff[400];
static webInterface_t *adapter = NULL;

web_status_t ETH_SOCK_Open(uint8_t socketNumber, uint8_t mode, uint16_t srcPort, uint8_t flags) {
    if (mode == WEB_PROTOCOL_TCP) {
        socket(socketNumber, Sn_MR_TCP, srcPort, flags);
    } else if (mode == WEB_PROTOCOL_UDP) {
        socket(socketNumber, Sn_MR_UDP, srcPort, flags);
    }
    return WEB_STATUS_OK;
}

web_status_t ETH_SOCK_Close(uint8_t socketNumber) {
    close(socketNumber);
    return WEB_STATUS_OK;
}

web_status_t ETH_SOCK_Listen(uint8_t socketNumber) {
    listen(socketNumber);
    return WEB_STATUS_OK;
}

web_status_t ETH_SOCK_Connect(uint8_t socketNumber, uint8_t *destAddress, uint16_t destPort) {
    uint8_t IPFromDNS[4];
    uint8_t DNSIP[4] = { 1, 1, 1, 1 };
    if (isalpha(destAddress[0]) != 0 && isalpha(destAddress[strlen((char *) destAddress) - 1]) != 0) {
        getDNSfromDHCP(DNSIP);
        DNS_run(DNSIP, destAddress, IPFromDNS);
    } else if (isdigit(destAddress[0]) != 0 && isdigit(destAddress[strlen((char *) destAddress) - 1]) != 0) {
        char *token = strtok((char *) destAddress, ".");
        for (int i = 0; i < 4; i++) {
            IPFromDNS[i] = atoi(token);
            token = strtok(NULL, ".");
        }
    }
    LOG_INFO("The IP from DNS is %d.%d.%d.%d.", IPFromDNS[0], IPFromDNS[1], IPFromDNS[2], IPFromDNS[3]);
    connect(socketNumber, IPFromDNS, destPort);
    return WEB_STATUS_OK;
}

web_status_t ETH_SOCK_Disconnect(uint8_t socketNumber) {
    disconnect(socketNumber);
    return WEB_STATUS_OK;
}

web_status_t ETH_SOCK_Send(uint8_t socketNumber, uint8_t *buf, uint16_t len) {
    send(socketNumber, buf, len);
    return WEB_STATUS_OK;
}

web_status_t ETH_SOCK_Recv(uint8_t socketNumber, uint8_t *buf, uint16_t maxLen) {
    recv(socketNumber, buf, maxLen);
    return WEB_STATUS_OK;
}
web_status_t ETH_SOCK_SendTo(uint8_t socketNumber, uint8_t *buf, uint16_t len, uint8_t *destAddress,
                             uint16_t destPort) {
    uint8_t IP[4];
    char   *token = strtok((char *) destAddress, ".");
    for (int i = 0; i < 4; i++) {
        IP[i] = atoi(token);
        token = strtok(NULL, ".");
    }
    sendto(socketNumber, buf, len, IP, destPort);
    return WEB_STATUS_OK;
}

web_status_t ETH_SOCK_RecvFrom(uint8_t socketNumber, uint8_t *buf, uint16_t maxLen, uint8_t *senderAddress,
                               uint16_t senderPort) {
    uint8_t IP[4];
    char   *token = strtok((char *) senderAddress, ".");
    for (int i = 0; i < 4; i++) {
        IP[i] = atoi(token);
        token = strtok(NULL, ".");
    }
    recvfrom(socketNumber, buf, maxLen, IP, &senderPort);
    return WEB_STATUS_OK;
}

web_status_t ETH_AdapterInit(void *memoryPointer) {
    if (wizChipInit(memoryPointer) != WIZ_OK) {
        return WEB_STATUS_ERROR;
    }

    if (adapter != NULL) {
        return WEB_STATUS_OK;
    }

    adapter = (webInterface_t *) malloc(sizeof(webInterface_t));

    DNS_init(6, DNSBuff);
    WEB_RegisterTCPOpenCloseCallback(adapter, ETH_SOCK_Open, ETH_SOCK_Close);
    WEB_RegisterTCPConnectDisconnectCallback(adapter, ETH_SOCK_Connect, ETH_SOCK_Disconnect);
    WEB_RegisterTCPSendRecvCallback(adapter, ETH_SOCK_Send, ETH_SOCK_Recv);
    WEB_RegisterTCPListenCallback(adapter, ETH_SOCK_Listen);
    WEB_RegisterUDPSendRecvCallback(adapter, ETH_SOCK_SendTo, ETH_SOCK_RecvFrom);
    return WEB_STATUS_OK;
}

webInterface_t *ETH_GetAdapter(void) {
    return adapter;
}

void ETH_AdapterDeInit(void) {
    free(adapter);
}