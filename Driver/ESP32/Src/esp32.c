/**
 * @file esp32.c
 * @brief A driver for ESP32 Wi-Fi module.
 * @version 0.1
 * @date 2023-01-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "esp32.h"
#include "main.h"
#include "usart.h"
#include "tx_api.h"
#include "at_utilities.h"
#include <stdio.h>
#define CIRCULAR_BUFFER_LENGTH 4096
#define LOG_DEFAULT_MODULE     LOG_M_ESP32
#include "loglib.h"

#define ESP32_FLAG_OK                (1 << 0)
#define ESP32_FLAG_ERROR             (1 << 1)
#define ESP32_FLAG_SET_OK            (1 << 2)
#define ESP32_FLAG_PROMPT            (1 << 3)
#define ESP32_FLAG_READY             (1 << 4)
#define ESP32_FLAG_WIFI_GOT_IP       (1 << 5)
#define ESP32_FLAG_GOT_TIME          (1 << 6)
#define ESP32_FLAG_GOT_UNIX_TIME     (1 << 7)
#define ESP32_FLAG_GOT_HTTP_RESPONSE (1 << 8)

static TX_THREAD    ESP32_URCThread;
static TX_MUTEX     ESP32_Mutex;
static TX_MUTEX     ESP32_FunctionMutex;
static TX_SEMAPHORE ESP32_Semaphore;
static TX_SEMAPHORE ESP32_PromptSemaphore;
static TX_SEMAPHORE ESP32_SetOKSemaphore;
static TX_SEMAPHORE ESP32_OKSemaphore;
static TX_SEMAPHORE ESP32_ReadySemaphore;

static ULONG                ESP32_ActualFlags;
static TX_EVENT_FLAGS_GROUP ESP32_Flags;
static uint8_t              ESP32_circularBuffer[CIRCULAR_BUFFER_LENGTH];
static int16_t              ESP32_bufferTail;

static char    bufferAT[262];
static uint8_t ESP32_RxStackPointer[1024];

static ATU_listNode_t     *ESP32_handlerQueueHead = NULL;
static ATU_flagListNode_t *ESP32_flagHandlerQueueHead = NULL;

int16_t ESP32_getCircularBufferHead() {
    return CIRCULAR_BUFFER_LENGTH - __HAL_DMA_GET_COUNTER(EXT_UART.hdmarx);
}

char codeText[1024] = { 0 };
void ESP32_StartTaskProcessingTask(ULONG argument) {
    for (;;) {
        tx_thread_sleep(100);
        if (ATU_GetNextURCSimple(ESP32_circularBuffer, CIRCULAR_BUFFER_LENGTH, &ESP32_bufferTail,
                                 ESP32_getCircularBufferHead, codeText, 1024) == ATU_STATUS_OK) {
            ATU_ParseStringSetFlags(ESP32_Flags, ESP32_flagHandlerQueueHead, codeText);
            ATU_ParseString(ESP32_handlerQueueHead, codeText);
        }
    }
}

void ESP32_RxCallback() {
}

void ESP32_TxCallback() {
    tx_semaphore_put(&ESP32_Semaphore);
}

esp32_status_t ESP32_SendATCommand(char *commandText) {
#if 1
    HAL_UART_Transmit_IT(&EXT_UART, (uint8_t *) commandText, strlen(commandText));
    tx_semaphore_get(&ESP32_Semaphore, TX_WAIT_FOREVER);
#else
    HAL_UART_Transmit(&EXT_UART, (uint8_t *) commandText, strlen(commandText), HAL_MAX_DELAY);
#endif
    return ESP32_S_OK;
}

__weak void ESP32_okATHandler(char *string) {
    LOG_DEBUG("OK Handler");
    tx_event_flags_set(&ESP32_Flags, ESP32_FLAG_OK, TX_OR);
    tx_semaphore_put(&ESP32_OKSemaphore);
}

__weak void ESP32_readyHandler(char *string) {
    LOG_DEBUG("Ready Handler");
    tx_event_flags_set(&ESP32_Flags, ESP32_FLAG_READY, TX_OR);
    tx_semaphore_put(&ESP32_ReadySemaphore);
}

__weak void ESP32_promptATHandler(char *string) {
    LOG_DEBUG("> Handler.");
    tx_event_flags_set(&ESP32_Flags, ESP32_FLAG_PROMPT, TX_OR);
    tx_semaphore_put(&ESP32_PromptSemaphore);
}

__weak void ESP32_setOKATHandler(char *string) {
    LOG_DEBUG("SET OK Handler.");
    tx_event_flags_set(&ESP32_Flags, ESP32_FLAG_SET_OK, TX_OR);
    tx_semaphore_put(&ESP32_SetOKSemaphore);
}

__weak void ESP32_errorATHandler(char *string) {
    LOG_ERROR("ERROR Handler.");
    tx_event_flags_set(&ESP32_Flags, ESP32_FLAG_ERROR, TX_OR);
    tx_semaphore_put(&ESP32_OKSemaphore);
}

__weak void ESP32_wifiConnectedHandler(char *string) {
    LOG_DEBUG("Wi-Fi Connected Handler");
}

__weak void ESP32_wifiGetIPHandler(char *string) {
    LOG_DEBUG("Wi-Fi Get IP Handler");
    LOG_INFO("The IP is: %s", string);
}

__weak void ESP32_wifiDisconnectedHandler(char *string) {
    tx_event_flags_get(&ESP32_Flags, ESP32_FLAG_WIFI_GOT_IP, TX_OR_CLEAR, &ESP32_ActualFlags, 5000);
    LOG_DEBUG("Wi-Fi Disconnected Handler");
}

__weak void ESP32_wifiGotIPHandler(char *string) {
    tx_event_flags_set(&ESP32_Flags, ESP32_FLAG_WIFI_GOT_IP, TX_OR);
    tx_semaphore_put(&ESP32_ReadySemaphore);
    LOG_DEBUG("Wi-Fi Got IP Handler");
}

__weak void ESP32_sntpTimeHandler(char *string) {
    tx_semaphore_put(&ESP32_ReadySemaphore);
    tx_event_flags_set(&ESP32_Flags, ESP32_FLAG_GOT_TIME, TX_OR);
    LOG_DEBUG("Wi-Fi SNTP Time Handler");
    LOG_INFO("The time is %s", string);
}

__weak void ESP32_unixTimeHandler(char *string) {
    tx_event_flags_set(&ESP32_Flags, ESP32_FLAG_GOT_UNIX_TIME, TX_OR);
    tx_semaphore_put(&ESP32_ReadySemaphore);
    LOG_DEBUG("Wi-Fi UNIX Time Handler");
    LOG_INFO("The UNIX time is %s", string);
}

__weak void ESP32_httpClientGotResponseCallback(char *text, uint32_t size) {
    LOG_WARN("Handler (httpClientGotResponseCallback):");
    LOG_DEBUG("Received size: %d", size);
    LOG_DEBUG("Received text: %s", text);
}

__weak void ESP32_RecvCallback(char *text, uint32_t size) {
    LOG_WARN("Handler (Recv TCP):");
    LOG_DEBUG("Received size: %d", size);
    LOG_DEBUG("Received text: %s", text);
}

__weak void ESP32_getAPsHandler(char *string) {
    LOG_DEBUG("AP Found Handler");
    LOG_INFO("Found AP: %s.", string);
}

__weak void ESP32_MQTTClientReceivedMessageCallback(char *topic, char *text, uint32_t size) {
    LOG_WARN("Handler (MQTTClientReceivedMessageCallback):");
    LOG_DEBUG("Received topic: %s", topic);
    LOG_DEBUG("Received size: %d", size);
    LOG_DEBUG("Received text: %s", text);
}

__weak void ESP32_mqttSubRecvHandler(char *string) {
    LOG_DEBUG("MQTT Subscription Recv Handler.");
    char *tmp = NULL, *topic = NULL, *size = NULL;
    tmp = strdup(string);
    if (strstr(tmp, ",") != NULL) {
        topic = strtok(tmp, ",");
    }
    if (topic != NULL) {
        size = strtok(NULL, ",");
    }
    ATU_RemoveQuotationMarks(topic);
    ESP32_MQTTClientReceivedMessageCallback(topic, string + strlen(topic) + strlen(size) + 4, atoi(size));
    free(tmp);
}

__weak void ESP32_httpclientHandler(char *string) {
    tx_event_flags_set(&ESP32_Flags, ESP32_FLAG_GOT_HTTP_RESPONSE, TX_OR);
    char    *tmp = strdup(string);
    char    *token = strtok(tmp, ",");
    uint32_t resultStringSize = atoi(token);
    char    *resultString = string + strlen(token) + 1;
    free(tmp);
    ESP32_httpClientGotResponseCallback(resultString, resultStringSize);
}

__weak void ESP32_recvTCPHandler(char *string) {
    char    *tmp = strdup(string);
    char    *token = strtok(tmp, ":");
    uint32_t resultStringSize = atoi(token);
    char    *resultString = string + strlen(token) + 1;
    free(tmp);
    ESP32_RecvCallback(resultString, resultStringSize);
}

esp32_status_t ESP32_ListAvaliableAPs() {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    ESP32_SendATCommand("AT+CWLAP\r\n");
    tx_semaphore_get(&ESP32_OKSemaphore, 5000);
    tx_mutex_put(&ESP32_FunctionMutex);
    return ESP32_S_OK;
}

esp32_status_t ESP32_Connect(char *remoteAddress, uint16_t remotePort) {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    ESP32_SendATCommand("AT+CIPSTART=\"TCP\",\"");
    ESP32_SendATCommand(remoteAddress);
    ESP32_SendATCommand("\",");
    char *tmp = malloc(10);
    itoa(remotePort, tmp, 10);
    ESP32_SendATCommand(tmp);
    ESP32_SendATCommand("\r\n");
    free(tmp);
    tx_semaphore_get(&ESP32_OKSemaphore, 5000);
    tx_mutex_put(&ESP32_FunctionMutex);
    return ESP32_S_OK;
}

esp32_status_t ESP32_Send(char *data) {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    ESP32_SendATCommand("AT+CIPSEND=");
    char *tmp = malloc(10);
    itoa(strlen(data), tmp, 10);
    ESP32_SendATCommand(tmp);
    ESP32_SendATCommand("\r\n");
    free(tmp);
    if (tx_semaphore_get(&ESP32_PromptSemaphore, 5000) == TX_SUCCESS) {
        ESP32_SendATCommand(data);
        tx_semaphore_get(&ESP32_OKSemaphore, 5000);
        tx_mutex_put(&ESP32_FunctionMutex);
        return ESP32_S_OK;
    }
    tx_mutex_put(&ESP32_FunctionMutex);
    LOG_ERROR("SEND has failed.");
    return ESP32_S_ERR_GENERAL;
}

esp32_status_t ESP32_GetIP() {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    ESP32_SendATCommand("AT+CIPSTA?\r\n");
    tx_semaphore_get(&ESP32_ReadySemaphore, 5000);
    tx_semaphore_get(&ESP32_OKSemaphore, 5000);
    tx_mutex_put(&ESP32_FunctionMutex);
    return ESP32_S_OK;
}

esp32_status_t ESP32_ConnectToAP(char *ssid, char *password) {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    sprintf(bufferAT, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    ESP32_SendATCommand(bufferAT);
    tx_event_flags_get(&ESP32_Flags, ESP32_FLAG_WIFI_GOT_IP, TX_OR, &ESP32_ActualFlags, 5000);
    tx_event_flags_get(&ESP32_Flags, ESP32_FLAG_OK, TX_OR_CLEAR, &ESP32_ActualFlags, 5000);
    tx_mutex_put(&ESP32_FunctionMutex);
    return ESP32_S_OK;
}

esp32_status_t ESP32_GetTimeFromSNTP() {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    ESP32_SendATCommand("AT+CIPSNTPTIME?\r\n");
    tx_semaphore_get(&ESP32_ReadySemaphore, 5000);
    tx_semaphore_get(&ESP32_OKSemaphore, 5000);
    tx_mutex_put(&ESP32_FunctionMutex);
    return ESP32_S_OK;
}

esp32_status_t ESP32_GetUnixTime() {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    ESP32_SendATCommand("AT+SYSTIMESTAMP?\r\n");
    tx_semaphore_get(&ESP32_ReadySemaphore, 5000);
    tx_semaphore_get(&ESP32_OKSemaphore, 5000);
    tx_mutex_put(&ESP32_FunctionMutex);
    return ESP32_S_OK;
}

esp32_status_t ESP32_DisconnectFromAP() {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    ESP32_SendATCommand("AT+CWQAP\r\n");
    tx_semaphore_get(&ESP32_OKSemaphore, 5000);
    tx_mutex_put(&ESP32_FunctionMutex);
    return ESP32_S_OK;
}

esp32_status_t ESP32_httpsClientByURL(char *url, esp32_http_method_t method, esp32_http_content_type_t contentType,
                                      esp32_http_transport_over_t protocol) {
    if (strlen(url) == 0) {
        return ESP32_S_ERR_GENERAL;
    }
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    sprintf(bufferAT, "AT+HTTPURLCFG=%d\r\n", strlen(url));
    ESP32_SendATCommand(bufferAT);
    if (tx_semaphore_get(&ESP32_PromptSemaphore, 5000) == TX_SUCCESS) {
        ESP32_SendATCommand(url);
        tx_semaphore_get(&ESP32_SetOKSemaphore, 5000);
        sprintf(bufferAT, "AT+HTTPCLIENT=%d,%d,,,,%d\r\n", method, contentType, protocol);
        ESP32_SendATCommand(bufferAT);
        tx_mutex_put(&ESP32_FunctionMutex);
        return ESP32_S_OK;
    }
    if (tx_semaphore_get(&ESP32_ReadySemaphore, 10000) != TX_SUCCESS) {
        tx_mutex_put(&ESP32_FunctionMutex);
        LOG_ERROR("HTTP(S) request has failed (1).");
        return ESP32_S_ERR_GENERAL;
    }
    if (tx_semaphore_get(&ESP32_OKSemaphore, 10000) != TX_SUCCESS) {
        tx_mutex_put(&ESP32_FunctionMutex);
        LOG_ERROR("HTTP(S) request has failed (2).");
        return ESP32_S_ERR_GENERAL;
    }
    tx_mutex_put(&ESP32_FunctionMutex);
    LOG_ERROR("HTTP(S) request has failed (3).");
    return ESP32_S_ERR_GENERAL;
}

esp32_status_t ESP32_MQTT_ConfigureUser(char *clientID, char *username, char *password, char *path) {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    sprintf(bufferAT, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"%s\"\r\n", clientID, username, password, path);
    ESP32_SendATCommand(bufferAT);
    tx_event_flags_get(&ESP32_Flags, ESP32_FLAG_OK, TX_OR_CLEAR, &ESP32_ActualFlags, 5000);
    tx_mutex_put(&ESP32_FunctionMutex);
    return ESP32_S_OK;
}

esp32_status_t ESP32_MQTT_ConfigureConnection(uint8_t disableCleanSession, char *lastWillTopic, char *lastWillMessage,
                                              esp32_mqtt_qos_t lastWillQOS, uint8_t lastWillRetain) {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    sprintf(bufferAT, "AT+MQTTCONNCFG=0,10,%d,\"%s\",\"%s\",%d,%d\r\n", disableCleanSession, lastWillTopic,
            lastWillMessage, lastWillQOS, lastWillRetain);
    ESP32_SendATCommand(bufferAT);
    tx_event_flags_get(&ESP32_Flags, ESP32_FLAG_OK, TX_OR_CLEAR, &ESP32_ActualFlags, 5000);
    tx_mutex_put(&ESP32_FunctionMutex);
    return ESP32_S_OK;
}

esp32_status_t ESP32_MQTT_Connect(char *host, uint16_t port) {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    sprintf(bufferAT, "AT+MQTTCONN=0,\"%s\",%d,1\r\n", host, port);
    ESP32_SendATCommand(bufferAT);
    if (tx_event_flags_get(&ESP32_Flags, ESP32_FLAG_OK, TX_OR_CLEAR, &ESP32_ActualFlags, 10000) != TX_SUCCESS) {
        tx_mutex_put(&ESP32_FunctionMutex);
        LOG_ERROR("Failed to connect to MQTT broker (1).");
        return ESP32_S_ERR_GENERAL;
    }
    if (tx_event_flags_get(&ESP32_Flags, ESP32_FLAG_OK, TX_OR_CLEAR, &ESP32_ActualFlags, 5000) != TX_SUCCESS) {
        tx_mutex_put(&ESP32_FunctionMutex);
        LOG_ERROR("Failed to connect to MQTT broker (2).");
        return ESP32_S_ERR_GENERAL;
    }
    tx_mutex_put(&ESP32_FunctionMutex);
    LOG_INFO("Connected to MQTT broker.");
    return ESP32_S_OK;
}

esp32_status_t ESP32_MQTT_Subscribe(char *topic, esp32_mqtt_qos_t qos) {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    sprintf(bufferAT, "AT+MQTTSUB=0,\"%s\",%d\r\n", topic, qos);
    ESP32_SendATCommand(bufferAT);
    tx_semaphore_get(&ESP32_ReadySemaphore, 5000);
    tx_semaphore_get(&ESP32_OKSemaphore, 5000);
    tx_mutex_put(&ESP32_FunctionMutex);
    LOG_INFO("MQTT Subscribed OK.");
    return ESP32_S_OK;
}

esp32_status_t ESP32_MQTT_Unsubscribe(char *topic) {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    sprintf(bufferAT, "AT+MQTTUNSUB=0,\"%s\"\r\n", topic);
    ESP32_SendATCommand(bufferAT);
    tx_semaphore_get(&ESP32_OKSemaphore, 5000);
    tx_mutex_put(&ESP32_FunctionMutex);
    LOG_INFO("MQTT Unsubscribed OK.");
    return ESP32_S_OK;
}

esp32_status_t ESP32_MQTT_Publish(char *topic, char *message, esp32_mqtt_qos_t qos, uint8_t retain) {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    ESP32_SendATCommand("AT+MQTTPUBRAW=0,\"");
    ESP32_SendATCommand(topic);
    ESP32_SendATCommand("\",");
    char *tmp = malloc(8);
    itoa(strlen(message), tmp, 10);
    ESP32_SendATCommand(tmp);
    ESP32_SendATCommand(",");
    itoa(qos, tmp, 10);
    ESP32_SendATCommand(tmp);
    ESP32_SendATCommand(",");
    itoa(retain, tmp, 10);
    ESP32_SendATCommand(tmp);
    free(tmp);
    ESP32_SendATCommand("\r\n");
    if (tx_semaphore_get(&ESP32_PromptSemaphore, 5000) == TX_SUCCESS) {
        ESP32_SendATCommand(message);
        tx_semaphore_get(&ESP32_SetOKSemaphore, 5000);
        tx_semaphore_get(&ESP32_OKSemaphore, 5000);
        tx_mutex_put(&ESP32_FunctionMutex);
        LOG_INFO("MQTT Publish OK.");
        return ESP32_S_OK;
    }
    tx_mutex_put(&ESP32_FunctionMutex);
    LOG_ERROR("Publish() has failed.");
    return ESP32_S_ERR_GENERAL;
}

esp32_status_t ESP32_MQTT_Close() {
    tx_mutex_get(&ESP32_FunctionMutex, TX_WAIT_FOREVER);
    ESP32_SendATCommand("AT+MQTTCLEAN=0\r\n");
    tx_semaphore_get(&ESP32_SetOKSemaphore, 5000);
    tx_mutex_put(&ESP32_FunctionMutex);
    LOG_INFO("MQTT Close OK.");
    return ESP32_S_ERR_GENERAL;
}

static void ESP32_registerCallbacks() {
    ATU_listNode_t *prevQueueItemPointer = malloc(sizeof(ATU_listNode_t));
    ATU_listNode_t *currentQueueItemPointer = malloc(sizeof(ATU_listNode_t));
    prevQueueItemPointer->triggerString = strdup("OK");
    currentQueueItemPointer->triggerString = strdup("ERROR");
    prevQueueItemPointer->next = currentQueueItemPointer;
    currentQueueItemPointer->handlerFunction = ESP32_okATHandler;
    prevQueueItemPointer->handlerFunction = ESP32_okATHandler;
    currentQueueItemPointer->next = NULL;
    ESP32_handlerQueueHead = prevQueueItemPointer;
    ATU_RegisterHandler(ESP32_handlerQueueHead, "SEND OK", ESP32_okATHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "WIFI GOT IP", ESP32_readyHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "+HTTPCLIENT:", ESP32_httpclientHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "+CIPSTA:ip:", ESP32_wifiGetIPHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "+CWLAP:", ESP32_getAPsHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "WIFI CONNECTED", ESP32_wifiConnectedHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "WIFI DISCONNECT", ESP32_wifiDisconnectedHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "WIFI GOT IP", ESP32_wifiGotIPHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "+CIPSNTPTIME:", ESP32_sntpTimeHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "+SYSTIMESTAMP:", ESP32_unixTimeHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "+MQTTSUBRECV:0,", ESP32_mqttSubRecvHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "+MQTTCONNECTED:0,", ESP32_readyHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "+MQTTPUB:OK", ESP32_setOKATHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "+MQTTSUB:0,", ESP32_readyHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "ready", ESP32_readyHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "+IPD,", ESP32_recvTCPHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, ">", ESP32_promptATHandler);
    ATU_RegisterHandler(ESP32_handlerQueueHead, "SET OK", ESP32_setOKATHandler);
    ATU_flagListNode_t *prevQueueItemPointer1 = malloc(sizeof(ATU_flagListNode_t));
    ATU_flagListNode_t *currentQueueItemPointer1 = malloc(sizeof(ATU_flagListNode_t));
    prevQueueItemPointer1->triggerString = strdup("OK");
    currentQueueItemPointer1->triggerString = strdup("ERROR");
    prevQueueItemPointer1->next = currentQueueItemPointer1;
    currentQueueItemPointer1->flags = ESP32_FLAG_OK;
    prevQueueItemPointer1->flags = ESP32_FLAG_ERROR;
    currentQueueItemPointer1->next = NULL;
    ESP32_flagHandlerQueueHead = prevQueueItemPointer1;
    ATU_RegisterFlagHandler(ESP32_Flags, ESP32_flagHandlerQueueHead, "WIFI GOT IP", ESP32_FLAG_WIFI_GOT_IP);
    ATU_RegisterFlagHandler(ESP32_Flags, ESP32_flagHandlerQueueHead, "+HTTPCLIENT:", ESP32_FLAG_GOT_HTTP_RESPONSE);
}

esp32_status_t ESP32_Init(void *byte_pool) {
    PIN_SET(PP3V1_WIFI_ON);    // Turn of WIFI power
    tx_thread_sleep(10);
    tx_mutex_create(&ESP32_Mutex, "AT UART Mutex", 1);
    tx_mutex_create(&ESP32_FunctionMutex, "Function Mutex", 1);
    tx_event_flags_create(&ESP32_Flags, "Wi-Fi Event Flags");
    tx_semaphore_create(&ESP32_Semaphore, "AT Utils Semaphore", 0);
    tx_semaphore_create(&ESP32_OKSemaphore, "OK Semaphore", 0);
    tx_semaphore_create(&ESP32_ReadySemaphore, "Ready Semaphore", 0);
    tx_semaphore_create(&ESP32_PromptSemaphore, "> Semaphore", 0);
    tx_semaphore_create(&ESP32_SetOKSemaphore, "SET OK Semaphore", 0);
    ESP32_registerCallbacks();

    HAL_UART_Receive_DMA(&EXT_UART, ESP32_circularBuffer, CIRCULAR_BUFFER_LENGTH);

    tx_thread_create(&ESP32_URCThread, "AT-Utils URC Parser Thread", ESP32_StartTaskProcessingTask, 0,
                     ESP32_RxStackPointer, 1024, 29, 0, TX_NO_TIME_SLICE, TX_AUTO_START);

    if (tx_semaphore_get(&ESP32_ReadySemaphore, 5000) != TX_SUCCESS) {
        return ESP32_S_ERR_TIMOUT;
    }
    ESP32_SendATCommand("AT+CWMODE=1\r\n");
    if (tx_semaphore_get(&ESP32_OKSemaphore, 5000) != TX_SUCCESS) {
        return ESP32_S_ERR_TIMOUT;
    }
    ESP32_SendATCommand("AT+CIPSNTPCFG=1,2\r\n");
    if (tx_semaphore_get(&ESP32_OKSemaphore, 5000) != TX_SUCCESS) {
        return ESP32_S_ERR_TIMOUT;
    }
    ESP32_SendATCommand("AT+CIPSNTPINTV=15\r\n");
    if (tx_semaphore_get(&ESP32_OKSemaphore, 5000) != TX_SUCCESS) {
        return ESP32_S_ERR_TIMOUT;
    }
    return ESP32_S_OK;
}