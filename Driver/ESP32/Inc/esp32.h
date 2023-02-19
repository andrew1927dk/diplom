/**
 * @file esp32.h
 * @brief A driver for ESP32 Wi-Fi module.
 * @version 0.1
 * @date 2023-01-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef ESP32_H
#define ESP32_H

#include <stdint.h>
#include <stddef.h>

#define ESP32_MQTT_NO_RETAIN             (0)
#define ESP32_MQTT_RETAIN                (1)
#define ESP32_MQTT_ENABLE_CLEAN_SEESION  (0)
#define ESP32_MQTT_DISABLE_CLEAN_SESSION (1)

/**
 * @brief ESP32 return status
 */
typedef enum {
    ESP32_S_OK,
    ESP32_S_ERR_GENERAL,
    ESP32_S_ERR_MEMORY,
    ESP32_S_ERR_ALREADY_INITED,
    ESP32_S_ERR_TIMOUT,
} esp32_status_t;

/**
 * @brief ESP32 HTTP(S) Request Methods.
 */
typedef enum {
    ESP32_HTTP_M_HEAD = 1,
    ESP32_HTTP_M_GET = 2,
    ESP32_HTTP_M_POST = 3,
    ESP32_HTTP_M_PUT = 4,
    ESP32_HTTP_M_DELETE = 5
} esp32_http_method_t;

/**
 * @brief ESP32 Content Type.
 */
typedef enum {
    ESP32_HTTP_CT_APPLICATION_WWW_FORM_URLENCODED = 0,
    ESP32_HTTP_CT_APPLICATION_JSON = 1,
    ESP32_HTTP_CT_MULTIPART_FORM_DATA = 2,
    ESP32_HTTP_CT_TEXT_XML = 3,
} esp32_http_content_type_t;

/**
 * @brief ESP32 Select HTTP or HTTPS.
 */
typedef enum {
    ESP32_HTTP_TRANSPORT_OVER_TCP = 1,
    ESP32_HTTP_TRANSPORT_OVER_SSL = 2,
} esp32_http_transport_over_t;

/**
 * @brief ESP32 Quality Of Service.
 */
typedef enum {
    ESP32_MQTT_QOS_AT_MOST_ONCE = 0,
    ESP32_MQTT_QOS_AT_LEAST_ONCE = 1,
    ESP32_MQTT_QOS_EXACTLY_ONCE = 2
} esp32_mqtt_qos_t;

/**
 * @brief Access point type
 */
typedef struct {
    uint8_t enc;             /** @brief Encryption method */
    char    ssid[32 + 1];    /** @brief String parameter showing SSID of the AP */
    uint8_t rssi;            /** @brief Signal strength */
    uint8_t mac[6];          /** @brief MAC address of the AP */
    uint8_t channel;         /** @brief Channel */
    uint8_t pairwise_cipher; /** @brief Pairwise cipher type */
    uint8_t group_cipher;    /** @brief Group cipher type */
    uint8_t bgn;             /** @brief 802.11 b/g/n */
    uint8_t wps;             /** @brief WPS flag */
} esp32_ap_t;

/**
 * @brief Init WiFi
 *
 * @param[in] byte_pool Pointer to byte pool for stack
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_Init(void *byte_pool);

/**
 * @brief Send data to Wi-Fi using UART.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_SendATCommand(char *commandText);

/**
 * @brief List avaliable Acess Points.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_ListAvaliableAPs();

/**
 * @brief Connect to an Acess Point.
 *
 * @return esp32_status_t Status
 * @param[in] ssid SSID of the AP.
 * @param[in] password Password.
 */
esp32_status_t ESP32_ConnectToAP(char *ssid, char *password);

/**
 * @brief Disconnect from the Acess Point.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_DisconnectFromAP();

/**
 * @brief Request ASCII Date format date and time.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_GetTimeFromSNTP();

/**
 * @brief Request UNIX Time Stamp.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_GetUnixTime();

/**
 * @brief HTTPS Client by URL.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_httpsClientByURL(char *url, esp32_http_method_t method, esp32_http_content_type_t contentType,
                                      esp32_http_transport_over_t protocol);

/**
 * @brief Connect using TCP protocol.
 *
 * @return esp32_status_t Status
 * @param[in] remoteAddress The IP of the remote server.
 * @param[in] remotePort Remote server port.
 */
esp32_status_t ESP32_Connect(char *remoteAddress, uint16_t remotePort);

/**
 * @brief Send data using TCP protocol.
 *
 * @return esp32_status_t Status
 * @param[in] data Data to be sent.
 */
esp32_status_t ESP32_Send(char *data);

/**
 * @brief Get IP of ESP32.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_GetIP();

/**
 * @brief MQTT Protocol configuration function.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_MQTT_ConfigureUser(char *clientID, char *username, char *password, char *path);

/**
 * @brief MQTT Protocol configuration function.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_MQTT_ConfigureConnection(uint8_t disableCleanSession, char *lastWillTopic, char *lastWillMessage,
                                              esp32_mqtt_qos_t lastWillQOS, uint8_t lastWillRetain);

/**
 * @brief MQTT Protocol connect function.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_MQTT_Connect(char *host, uint16_t port);

/**
 * @brief MQTT Protocol subscribe function.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_MQTT_Subscribe(char *topic, esp32_mqtt_qos_t qos);

/**
 * @brief MQTT Protocol unsubscribe function.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_MQTT_Unsubscribe(char *topic);

/**
 * @brief MQTT Protocol publish function.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_MQTT_Publish(char *topic, char *message, esp32_mqtt_qos_t qos, uint8_t retain);

/**
 * @brief MQTT Protocol close function.
 *
 * @return esp32_status_t Status
 */
esp32_status_t ESP32_MQTT_Close();

/**
 * @brief MQTT Protocol receive callback function.
 *
 * @return esp32_status_t Status
 */
void ESP32_MQTTClientReceivedMessageCallback(char *topic, char *text, uint32_t size);

/**
 * @brief HTTP(S) Protocol close function.
 *
 * @return esp32_status_t Status
 */
void ESP32_httpClientGotResponseCallback(char *text, uint32_t size);

void ESP32_RxCallback();
void ESP32_TxCallback();

/**
 * @brief Scan access points
 *
 * @note User should free memory, allocated by this function ('aps' parameter)
 *
 * @param[out] aps List of APs
 * @param[out] num Number of found APs
 * @return esp32_status_t
 */
esp32_status_t ESP32_ScanAPs(esp32_ap_t **aps, size_t *num);

#endif    // ESP32_H
