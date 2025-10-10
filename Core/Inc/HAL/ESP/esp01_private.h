/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 08 Aug 2025                                                                     */
/* Description       : esp01_private.h --> ESP01 Private Header File                                  */
/*******************************************************************************************************/

#ifndef ESP01_PRIVATE_H_
#define ESP01_PRIVATE_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>
#include "esp01_config.h"

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
#define ESP01_CMD_SERVER_CONFIG     "AT+CIPSERVER="

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

#endif /* ESP01_PRIVATE_H_ */
