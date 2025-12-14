/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V2.0.0                                                                          */
/* Date              : 09 Dec 2025                                                                     */
/* Description       : esp01.h --> ESP01 WiFi Module Driver Header                                    */
/*                     V2.0.0: Added multi-connection, AP config, and HTTP server support             */
/*******************************************************************************************************/

#ifndef ESP01_H_
#define ESP01_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>

/*******************************************************************************************************/
/* ESP01 Configuration Parameters                                                                      */
/*******************************************************************************************************/

/* UART Instance for ESP01 Communication */
#define ESP01_UART_INSTANCE         USART_INSTANCE_2

/* ESP01 Timing Configuration (in milliseconds) */
#define ESP01_STARTUP_DELAY         3000    /* Delay after power-on */
#define ESP01_CMD_TIMEOUT           5000    /* Default command timeout */
#define ESP01_CONNECT_TIMEOUT       10000   /* WiFi connection timeout */
#define ESP01_RESPONSE_DELAY        500     /* Delay between command and response */

/* ESP01 Buffer Sizes */
#define ESP01_RX_BUFFER_SIZE        512     /* Receive buffer size */
#define ESP01_TX_BUFFER_SIZE        128     /* Transmit buffer size */

/* ESP01 Default Settings */
#define ESP01_DEFAULT_BAUD_RATE     115200  /* Must match USART configuration */

/* ESP01 Max Retry Attempts */
#define ESP01_MAX_RETRY_COUNT       3

/*******************************************************************************************************/
/* AT Command Definitions                                                                              */
/*******************************************************************************************************/

/* Basic AT Commands */
#define ESP01_CMD_AT                "AT\r\n"
#define ESP01_CMD_RESET             "AT+RST\r\n"
#define ESP01_CMD_GET_VERSION       "AT+GMR\r\n"
#define ESP01_CMD_ECHO_OFF          "ATE0\r\n"
#define ESP01_CMD_ECHO_ON           "ATE1\r\n"

/* WiFi Commands */
#define ESP01_CMD_SET_MODE          "AT+CWMODE="
#define ESP01_CMD_CONNECT_AP        "AT+CWJAP="
#define ESP01_CMD_DISCONNECT_AP     "AT+CWQAP\r\n"
#define ESP01_CMD_LIST_AP           "AT+CWLAP\r\n"
#define ESP01_CMD_GET_IP            "AT+CIFSR\r\n"

/* TCP/IP Commands */
#define ESP01_CMD_SET_MUX           "AT+CIPMUX="
#define ESP01_CMD_START_TCP         "AT+CIPSTART="
#define ESP01_CMD_SEND_DATA         "AT+CIPSEND="
#define ESP01_CMD_CLOSE_CONN        "AT+CIPCLOSE\r\n"
#define ESP01_CMD_CLOSE_CONN_ID     "AT+CIPCLOSE="
#define ESP01_CMD_SERVER_CONFIG     "AT+CIPSERVER="

/* AP Configuration Commands */
#define ESP01_CMD_CONFIGURE_AP      "AT+CWSAP="
#define ESP01_CMD_GET_AP_IP         "AT+CIPAP?\r\n"

/*******************************************************************************************************/
/* Response Definitions                                                                                */
/*******************************************************************************************************/

#define ESP01_RESPONSE_OK           "OK"
#define ESP01_RESPONSE_ERROR        "ERROR"
#define ESP01_RESPONSE_READY        "ready"
#define ESP01_RESPONSE_CONNECT      "CONNECT"
#define ESP01_RESPONSE_CLOSED       "CLOSED"
#define ESP01_RESPONSE_SEND_OK      "SEND OK"
#define ESP01_RESPONSE_SEND_FAIL    "SEND FAIL"
#define ESP01_RESPONSE_WIFI_CONN    "WIFI CONNECTED"
#define ESP01_RESPONSE_WIFI_DISC    "WIFI DISCONNECT"
#define ESP01_RESPONSE_GOT_IP       "WIFI GOT IP"

/* Multi-Connection Response Patterns */
#define ESP01_RESPONSE_CONN_OPEN    ",CONNECT"
#define ESP01_RESPONSE_CONN_CLOSE   ",CLOSED"
#define ESP01_RESPONSE_IPD          "+IPD,"

/*******************************************************************************************************/
/* Multi-Connection Configuration                                                                      */
/*******************************************************************************************************/

#define ESP01_MAX_CONNECTIONS       5
#define ESP01_HTTP_BUFFER_SIZE      512

/*******************************************************************************************************/
/* IPD Detection State Machine                                                                         */
/*******************************************************************************************************/

typedef enum
{
    IPD_STATE_IDLE = 0,
    IPD_STATE_FOUND_PLUS,
    IPD_STATE_FOUND_I,
    IPD_STATE_FOUND_P,
    IPD_STATE_FOUND_D,
    IPD_STATE_FOUND_COMMA1,
    IPD_STATE_READING_CONN_ID,
    IPD_STATE_FOUND_COMMA2,
    IPD_STATE_READING_LENGTH,
    IPD_STATE_READING_DATA
} ESP01_IPDState_t;

/*******************************************************************************************************/
/* HTTP Data Buffer Structure                                                                          */
/*******************************************************************************************************/

typedef struct
{
    uint8_t buffer[ESP01_HTTP_BUFFER_SIZE];
    uint16_t length;
    uint8_t conn_id;
    uint8_t pending;
} ESP01_HTTPBuffer_t;

/*******************************************************************************************************/
/* Internal Buffer Structure                                                                           */
/*******************************************************************************************************/

typedef struct
{
    uint8_t buffer[ESP01_RX_BUFFER_SIZE];
    uint16_t writeIndex;
    uint16_t readIndex;
    uint16_t dataCount;
} ESP01_RxBuffer_t;

/*******************************************************************************************************/
/* ESP01 Status Definitions                                                                            */
/*******************************************************************************************************/

typedef enum
{
    ESP01_STATUS_OK = 0,
    ESP01_STATUS_ERROR,
    ESP01_STATUS_TIMEOUT,
    ESP01_STATUS_NO_RESPONSE,
    ESP01_STATUS_BUSY,
    ESP01_STATUS_FAIL,
    ESP01_STATUS_IN_PROGRESS
} ESP01_Status_t;

/*******************************************************************************************************/
/* ESP01 WiFi Mode Definitions                                                                         */
/*******************************************************************************************************/

typedef enum
{
    ESP01_MODE_STATION = 1,        /* Station mode (client) */
    ESP01_MODE_AP = 2,              /* Access Point mode */
    ESP01_MODE_BOTH = 3             /* Both Station and AP */
} ESP01_WiFiMode_t;

/*******************************************************************************************************/
/* AP Configuration Encryption Types                                                                   */
/*******************************************************************************************************/

typedef enum
{
    ESP01_ENC_OPEN = 0,            /* Open - no encryption */
    ESP01_ENC_WPA_PSK = 2,         /* WPA_PSK */
    ESP01_ENC_WPA2_PSK = 3,        /* WPA2_PSK */
    ESP01_ENC_WPA_WPA2_PSK = 4     /* WPA_WPA2_PSK */
} ESP01_Encryption_t;

/*******************************************************************************************************/
/* Callback Type Definitions                                                                           */
/*******************************************************************************************************/

/* MQTT Message Callback Type */
typedef void (*ESP01_MQTT_Callback_t)(const char* topic, const uint8_t* payload, uint16_t length);

/**
 * @brief Callback type for HTTP data received from clients
 * @param conn_id: Connection ID that sent the data
 * @param data: Pointer to received data
 * @param length: Length of received data
 */
typedef void (*ESP01_HTTPDataCallback_t)(uint8_t conn_id, const uint8_t* data, uint16_t length);

/*******************************************************************************************************/
/* Function Prototypes                                                                                 */
/*******************************************************************************************************/

/* MQTT Functions */
void ESP01_RegisterMQTTCallback(ESP01_MQTT_Callback_t callback);
void ESP01_ProcessMQTTMessages(void);

/* Basic Initialization and Control */
ESP01_Status_t ESP01_Initialize(void);
ESP01_Status_t ESP01_Reset(void);
ESP01_Status_t ESP01_TestConnection(void);

/* WiFi Configuration */
ESP01_Status_t ESP01_SetWiFiMode(ESP01_WiFiMode_t Copy_Mode);
ESP01_Status_t ESP01_ConnectToAP(const uint8_t* Copy_SSID, const uint8_t* Copy_Password);
ESP01_Status_t ESP01_DisconnectFromAP(void);
ESP01_Status_t ESP01_GetIPAddress(uint8_t* Copy_IPBuffer, uint8_t Copy_BufferSize);

/* TCP/IP Operations */
ESP01_Status_t ESP01_ConnectTCP(const uint8_t* Copy_ServerIP, uint16_t Copy_Port);
ESP01_Status_t ESP01_SendData(const uint8_t* Copy_Data, uint16_t Copy_Length);
ESP01_Status_t ESP01_CloseConnection(void);

/* Utility Functions */
ESP01_Status_t ESP01_SendCommand(const uint8_t* Copy_Command);
ESP01_Status_t ESP01_WaitForResponse(const uint8_t* Copy_ExpectedResponse, uint32_t Copy_Timeout);
void ESP01_ClearBuffer(void);

/**
 * @brief Check if MQTT message was detected in interrupt
 * @return 1 if message pending, 0 otherwise
 */
uint8_t ESP01_HasPendingMQTTMessage(void);

/**
 * @brief Process MQTT message that was buffered in interrupt context
 */
void ESP01_ProcessPendingMQTTMessage(void);

/*******************************************************************************************************/
/* Multi-Connection and Server Functions                                                               */
/*******************************************************************************************************/

/**
 * @brief Enable or disable multiple connection mode
 * @param Copy_Enable: 1 to enable (up to 5 connections), 0 for single connection
 * @return ESP01_Status_t
 * @note Must be called BEFORE starting server or connecting as client
 */
ESP01_Status_t ESP01_SetMultipleConnections(uint8_t Copy_Enable);

/**
 * @brief Configure ESP01 as Access Point
 * @param Copy_SSID: AP network name (max 32 chars)
 * @param Copy_Password: AP password (8-64 chars, or empty for open)
 * @param Copy_Channel: WiFi channel (1-13)
 * @param Copy_Encryption: Encryption type (ESP01_Encryption_t)
 * @return ESP01_Status_t
 * @note WiFi mode must be set to AP or BOTH before calling this
 */
ESP01_Status_t ESP01_ConfigureAP(const uint8_t* Copy_SSID, const uint8_t* Copy_Password,
                                  uint8_t Copy_Channel, ESP01_Encryption_t Copy_Encryption);

/**
 * @brief Start TCP server on specified port
 * @param Copy_Port: Port number to listen on (e.g., 80 for HTTP)
 * @return ESP01_Status_t
 * @note Multiple connections must be enabled first (CIPMUX=1)
 */
ESP01_Status_t ESP01_StartServer(uint16_t Copy_Port);

/**
 * @brief Stop TCP server
 * @return ESP01_Status_t
 */
ESP01_Status_t ESP01_StopServer(void);

/**
 * @brief Send data to a specific connection (multi-connection mode)
 * @param Copy_ConnId: Connection ID (0-4)
 * @param Copy_Data: Data buffer to send
 * @param Copy_Length: Length of data
 * @return ESP01_Status_t
 */
ESP01_Status_t ESP01_SendDataToConnection(uint8_t Copy_ConnId, const uint8_t* Copy_Data, uint16_t Copy_Length);

/**
 * @brief Close a specific connection by ID
 * @param Copy_ConnId: Connection ID to close (0-4)
 * @return ESP01_Status_t
 */
ESP01_Status_t ESP01_CloseConnectionById(uint8_t Copy_ConnId);

/**
 * @brief Get AP mode IP address (typically 192.168.4.1)
 * @param Copy_IPBuffer: Buffer to store IP string
 * @param Copy_BufferSize: Size of buffer
 * @return ESP01_Status_t
 */
ESP01_Status_t ESP01_GetAPIPAddress(uint8_t* Copy_IPBuffer, uint8_t Copy_BufferSize);

/*******************************************************************************************************/
/* HTTP Data Callback Functions                                                                        */
/*******************************************************************************************************/

/**
 * @brief Register callback for HTTP data reception
 * @param callback: Function to call when HTTP data is received
 */
void ESP01_RegisterHTTPCallback(ESP01_HTTPDataCallback_t callback);

/**
 * @brief Check if HTTP data is pending from any connection
 * @return 1 if data pending, 0 otherwise
 */
uint8_t ESP01_HasPendingHTTPData(void);

/**
 * @brief Process pending HTTP data
 */
void ESP01_ProcessPendingHTTPData(void);

#endif /* ESP01_H_ */
