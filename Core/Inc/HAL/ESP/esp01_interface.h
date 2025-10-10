/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 08 Aug 2025                                                                     */
/* Description       : esp01_interface.h --> ESP01 Interface Header File                              */
/*******************************************************************************************************/

#ifndef ESP01_INTERFACE_H_
#define ESP01_INTERFACE_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>

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
/* Function Prototypes                                                                                 */
/*******************************************************************************************************/

/* MQTT Message Callback Type */
typedef void (*ESP01_MQTT_Callback_t)(const char* topic, const uint8_t* payload, uint16_t length);

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

#endif /* ESP01_INTERFACE_H_ */
