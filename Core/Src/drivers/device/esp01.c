/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V2.0.0                                                                          */
/* Date              : 09 Dec 2025                                                                     */
/* Description       : esp01.c --> ESP01 WiFi Module Driver Implementation                            */
/*                     V2.0.0: Added multi-connection, AP config, HTTP server support                 */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include "esp01.h"
#include "usart.h"
#include "stk.h"
#include "mqtt.h"
#include <string.h>
#include <stdio.h>
#include "stm32f4xx.h"

/*******************************************************************************************************/
/* Global Variables                                                                                    */
/*******************************************************************************************************/

static ESP01_RxBuffer_t ESP01_RxBuffer;
static uint8_t ESP01_TxBuffer[ESP01_TX_BUFFER_SIZE];
static volatile uint8_t ESP01_DataReceived = 0;
static ESP01_MQTT_Callback_t g_MQTT_MessageCallback = NULL;

/* Interrupt-driven MQTT message processing variables */
static volatile uint8_t ESP01_MQTTMessagePending = 0;
static volatile uint8_t ESP01_IPDDetectionState = 0;
static uint8_t ESP01_IPDBuffer[512];
static volatile uint16_t ESP01_IPDBufferIndex = 0;
static volatile uint16_t ESP01_IPDExpectedLength = 0;
static volatile uint16_t ESP01_IPDCompletedLength = 0; /* Length of completed message */
static volatile uint16_t ESP01_SkippedIPDCount = 0;  /* Debug counter */
static volatile uint16_t ESP01_CapturedIPDCount = 0; /* Debug counter */

/* Multi-connection and HTTP server variables */
static uint8_t ESP01_MultiConnectionEnabled = 0;
static ESP01_HTTPDataCallback_t g_HTTP_DataCallback = NULL;
static ESP01_HTTPBuffer_t ESP01_HTTPBuffer;
static volatile uint8_t ESP01_IPDConnectionId = 0;

/*******************************************************************************************************/
/* Helper Functions                                                                                    */
/*******************************************************************************************************/

void ESP01_RegisterMQTTCallback(ESP01_MQTT_Callback_t callback)
{
    g_MQTT_MessageCallback = callback;
    printf("ESP01: MQTT callback registered\r\n");
}

static void ESP01_ParseMQTTPublish(const uint8_t* data, uint16_t length)
{
    if (g_MQTT_MessageCallback == NULL || length < 4) {
        printf("ESP01: ParseMQTTPublish - no callback or length < 4 (len=%d)\r\n", length);
        return;
    }

    /* Check MQTT packet type - PUBLISH is 0x30-0x3F */
    uint8_t packet_type = data[0] & 0xF0;
    if (packet_type != 0x30) {
        printf("ESP01: Not a PUBLISH packet (type=0x%02X, first byte=0x%02X)\r\n",
               packet_type, data[0]);
        return;
    }

    printf("ESP01: Parsing MQTT PUBLISH packet\r\n");

    uint16_t topic_length = ((uint16_t)data[2] << 8) | data[3];
    uint16_t topic_start = 4;

    if (topic_start + topic_length >= length) {
        printf("ESP01: Invalid MQTT packet\r\n");
        return;
    }

    char topic[65];
    uint16_t copy_len = (topic_length < sizeof(topic) - 1) ? topic_length : sizeof(topic) - 1;
    memcpy(topic, &data[topic_start], copy_len);
    topic[copy_len] = '\0';

    uint16_t payload_start = topic_start + topic_length;
    uint8_t qos = (data[0] >> 1) & 0x03;
    uint16_t packet_id = 0;

    if (qos > 0) {
        /* Extract packet ID for QoS 1/2 messages */
        packet_id = ((uint16_t)data[payload_start] << 8) | data[payload_start + 1];
        payload_start += 2;
        printf("ESP01: QoS=%d, Packet ID=%d\r\n", qos, packet_id);
    }

    if (payload_start >= length) {
        return;
    }

    uint16_t payload_length = length - payload_start;
    const uint8_t* payload = &data[payload_start];

    printf("ESP01: Topic: %s, Payload length: %d\r\n", topic, payload_length);

    /* Call the application callback first */
    g_MQTT_MessageCallback(topic, payload, payload_length);

    /* Send PUBACK for QoS 1 messages - CRITICAL for receiving more messages! */
    if (qos == 1 && packet_id > 0) {
        printf("ESP01: Sending PUBACK for QoS 1 message\r\n");
        MQTT_SendPubAck(packet_id);
    }
}



/*******************************************************************************************************/
/* ESP01_UART_Callback                                                                                 */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> UART receive callback for ESP01                                            */
/* Function Input       -> None                                                                        */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

static void ESP01_UART_Callback(void)
{
    uint8_t receivedByte = USART_ReadData(ESP01_UART_INSTANCE);

    /* Store in circular buffer */
    if (ESP01_RxBuffer.dataCount < ESP01_RX_BUFFER_SIZE)
    {
        ESP01_RxBuffer.buffer[ESP01_RxBuffer.writeIndex] = receivedByte;
        ESP01_RxBuffer.writeIndex = (ESP01_RxBuffer.writeIndex + 1) % ESP01_RX_BUFFER_SIZE;
        ESP01_RxBuffer.dataCount++;
        ESP01_DataReceived = 1;
    }

    /* State machine for detecting +IPD messages in real-time
     * Single connection format: +IPD,<length>:<data>
     * Multi-connection format:  +IPD,<conn_id>,<length>:<data>
     */
    switch (ESP01_IPDDetectionState) {
        case IPD_STATE_IDLE:  /* Looking for '+' */
            if (receivedByte == '+') {
                /* Start capturing new message - always capture, even if previous pending */
                /* If previous message was pending, it gets overwritten - main buffer fallback will catch it */
                ESP01_IPDDetectionState = IPD_STATE_FOUND_PLUS;
                ESP01_IPDBufferIndex = 0;
                ESP01_IPDExpectedLength = 0;
                ESP01_IPDConnectionId = 0;
                if (ESP01_MQTTMessagePending) {
                    ESP01_SkippedIPDCount++;  /* Previous message was overwritten */
                }
                ESP01_MQTTMessagePending = 0;  /* Clear pending since we're starting fresh */
                ESP01_CapturedIPDCount++;
            }
            break;

        case IPD_STATE_FOUND_PLUS:  /* Looking for 'I' */
            if (receivedByte == 'I') {
                ESP01_IPDDetectionState = IPD_STATE_FOUND_I;
            } else {
                ESP01_IPDDetectionState = IPD_STATE_IDLE;
            }
            break;

        case IPD_STATE_FOUND_I:  /* Looking for 'P' */
            if (receivedByte == 'P') {
                ESP01_IPDDetectionState = IPD_STATE_FOUND_P;
            } else {
                ESP01_IPDDetectionState = IPD_STATE_IDLE;
            }
            break;

        case IPD_STATE_FOUND_P:  /* Looking for 'D' */
            if (receivedByte == 'D') {
                ESP01_IPDDetectionState = IPD_STATE_FOUND_D;
            } else {
                ESP01_IPDDetectionState = IPD_STATE_IDLE;
            }
            break;

        case IPD_STATE_FOUND_D:  /* Looking for ',' */
            if (receivedByte == ',') {
                ESP01_IPDDetectionState = IPD_STATE_FOUND_COMMA1;
            } else {
                ESP01_IPDDetectionState = IPD_STATE_IDLE;
            }
            break;

        case IPD_STATE_FOUND_COMMA1:  /* Reading first number (conn_id or length) */
            if (receivedByte >= '0' && receivedByte <= '9') {
                if (ESP01_MultiConnectionEnabled) {
                    /* Multi-connection: first number is conn_id */
                    ESP01_IPDConnectionId = receivedByte - '0';
                    ESP01_IPDDetectionState = IPD_STATE_READING_CONN_ID;
                } else {
                    /* Single connection: first number is length */
                    ESP01_IPDExpectedLength = receivedByte - '0';
                    ESP01_IPDDetectionState = IPD_STATE_READING_LENGTH;
                }
            } else {
                ESP01_IPDDetectionState = IPD_STATE_IDLE;
            }
            break;

        case IPD_STATE_READING_CONN_ID:  /* Multi-connection: reading more conn_id digits or comma */
            if (receivedByte == ',') {
                ESP01_IPDDetectionState = IPD_STATE_FOUND_COMMA2;
            } else if (receivedByte >= '0' && receivedByte <= '9') {
                ESP01_IPDConnectionId = ESP01_IPDConnectionId * 10 + (receivedByte - '0');
            } else {
                ESP01_IPDDetectionState = IPD_STATE_IDLE;
            }
            break;

        case IPD_STATE_FOUND_COMMA2:  /* Multi-connection: start reading length */
            if (receivedByte >= '0' && receivedByte <= '9') {
                ESP01_IPDExpectedLength = receivedByte - '0';
                ESP01_IPDDetectionState = IPD_STATE_READING_LENGTH;
            } else {
                ESP01_IPDDetectionState = IPD_STATE_IDLE;
            }
            break;

        case IPD_STATE_READING_LENGTH:  /* Reading length digits */
            if (receivedByte >= '0' && receivedByte <= '9') {
                ESP01_IPDExpectedLength = ESP01_IPDExpectedLength * 10 + (receivedByte - '0');
            } else if (receivedByte == ':') {
                /* Length complete, now read the actual data */
                /* Note: ESP01_IPDExpectedLength now contains the full length */
                ESP01_IPDDetectionState = IPD_STATE_READING_DATA;
                ESP01_IPDBufferIndex = 0;
            } else {
                ESP01_IPDDetectionState = IPD_STATE_IDLE;
            }
            break;

        case IPD_STATE_READING_DATA:  /* Reading actual data bytes */
            if (ESP01_IPDBufferIndex < sizeof(ESP01_IPDBuffer)) {
                ESP01_IPDBuffer[ESP01_IPDBufferIndex++] = receivedByte;

                /* Check if we've received all expected bytes */
                if (ESP01_IPDBufferIndex >= ESP01_IPDExpectedLength) {
                    /* Complete message received! Route based on connection mode */
                    ESP01_IPDCompletedLength = ESP01_IPDExpectedLength;  /* Save for debug */
                    if (ESP01_MultiConnectionEnabled && ESP01_IPDConnectionId > 0) {
                        /* HTTP data from server connection (conn_id 1-4) */
                        ESP01_HTTPBuffer.conn_id = ESP01_IPDConnectionId;
                        ESP01_HTTPBuffer.length = ESP01_IPDBufferIndex;
                        memcpy(ESP01_HTTPBuffer.buffer, ESP01_IPDBuffer, ESP01_IPDBufferIndex);
                        ESP01_HTTPBuffer.pending = 1;
                    } else {
                        /* MQTT data (conn_id 0 or single connection mode) */
                        ESP01_MQTTMessagePending = 1;
                    }
                    ESP01_IPDDetectionState = IPD_STATE_IDLE;
                }
            } else {
                /* Buffer overflow protection */
                ESP01_IPDDetectionState = IPD_STATE_IDLE;
            }
            break;

        default:
            ESP01_IPDDetectionState = IPD_STATE_IDLE;
            break;
    }
}

/*******************************************************************************************************/
/* ESP01_ReadFromBuffer                                                                                */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Read data from circular buffer                                             */
/* Function Input       -> Buffer pointer, max length                                                  */
/* Function Return      -> Number of bytes read                                                        */
/*******************************************************************************************************/

static uint16_t ESP01_ReadFromBuffer(uint8_t* Copy_Buffer, uint16_t Copy_MaxLength)
{
    uint16_t bytesRead = 0;

    while ((ESP01_RxBuffer.dataCount > 0) && (bytesRead < Copy_MaxLength))
    {
        Copy_Buffer[bytesRead] = ESP01_RxBuffer.buffer[ESP01_RxBuffer.readIndex];
        ESP01_RxBuffer.readIndex = (ESP01_RxBuffer.readIndex + 1) % ESP01_RX_BUFFER_SIZE;
        ESP01_RxBuffer.dataCount--;
        bytesRead++;
    }

    return bytesRead;
}

/*******************************************************************************************************/
/* ESP01_StringSearch                                                                                  */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Search for string in buffer                                                */
/* Function Input       -> Buffer, buffer size, search string                                          */
/* Function Return      -> 1 if found, 0 if not                                                       */
/*******************************************************************************************************/

static uint8_t ESP01_StringSearch(const uint8_t* Copy_Buffer, uint16_t Copy_BufferSize, const uint8_t* Copy_SearchStr)
{
    uint16_t searchLen = strlen((const char*)Copy_SearchStr);

    if (searchLen > Copy_BufferSize)
        return 0;

    for (uint16_t i = 0; i <= (Copy_BufferSize - searchLen); i++)
    {
        if (strncmp((const char*)&Copy_Buffer[i], (const char*)Copy_SearchStr, searchLen) == 0)
        {
            return 1;
        }
    }

    return 0;
}

/*******************************************************************************************************/
/* Function Implementations                                                                            */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* ESP01_Initialize                                                                                    */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Initialize ESP01 module                                                     */
/* Function Input       -> None                                                                        */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_Initialize(void)
{
    ESP01_Status_t status;

    /* Clear buffers */
    ESP01_ClearBuffer();
    memset(ESP01_TxBuffer, 0, ESP01_TX_BUFFER_SIZE);

    /* Set UART callback for ESP01 - THIS MUST BE DONE FIRST */
    USART_SetCallback(ESP01_UART_INSTANCE, ESP01_UART_Callback);

    printf("ESP01: Callback set, waiting for module to stabilize...\r\n");

    /* Wait for ESP01 to stabilize after power-on */
    STK_SudoDelay(ESP01_STARTUP_DELAY);

    /* Clear any garbage data that might have been received during startup */
    ESP01_ClearBuffer();

    /* Test AT connection */
    printf("ESP01: Testing AT connection...\r\n");
    status = ESP01_TestConnection();
    if (status != ESP01_STATUS_OK)
    {
        printf("ESP01: AT test failed with status %d\r\n", status);
        return status;
    }

    printf("ESP01: AT test successful, disabling echo...\r\n");

    /* Disable echo */
    status = ESP01_SendCommand((uint8_t*)ESP01_CMD_ECHO_OFF);
    if (status != ESP01_STATUS_OK)
    {
        printf("ESP01: Failed to send echo off command\r\n");
        return status;
    }

    /* Wait for OK response */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_OK, ESP01_CMD_TIMEOUT);

    printf("ESP01: Initialization complete with status %d\r\n", status);

    return status;
}

/*******************************************************************************************************/
/* ESP01_Reset                                                                                         */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Reset ESP01 module                                                          */
/* Function Input       -> None                                                                        */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_Reset(void)
{
    ESP01_Status_t status;

    /* Send reset command */
    status = ESP01_SendCommand((uint8_t*)ESP01_CMD_RESET);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for ready response */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_READY, ESP01_CONNECT_TIMEOUT);

    /* Clear buffer after reset */
    ESP01_ClearBuffer();

    return status;
}

/*******************************************************************************************************/
/* ESP01_TestConnection                                                                                */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Test AT connection with ESP01                                              */
/* Function Input       -> None                                                                        */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_TestConnection(void)
{
    ESP01_Status_t status;
    uint8_t retryCount = 0;

    while (retryCount < ESP01_MAX_RETRY_COUNT)
    {
        /* Clear buffer before sending */
        ESP01_ClearBuffer();

        /* Send AT command */
        status = ESP01_SendCommand((uint8_t*)ESP01_CMD_AT);
        if (status != ESP01_STATUS_OK)
        {
            retryCount++;
            continue;
        }

        /* Wait for OK response */
        status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_OK, ESP01_CMD_TIMEOUT);
        if (status == ESP01_STATUS_OK)
        {
            return ESP01_STATUS_OK;
        }

        retryCount++;
        STK_SudoDelay(500);
    }

    return ESP01_STATUS_NO_RESPONSE;
}

/*******************************************************************************************************/
/* ESP01_SetWiFiMode                                                                                   */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Set WiFi operating mode                                                     */
/* Function Input       -> WiFi mode                                                                   */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_SetWiFiMode(ESP01_WiFiMode_t Copy_Mode)
{
    ESP01_Status_t status;

    /* Prepare command */
    sprintf((char*)ESP01_TxBuffer, "%s%d\r\n", ESP01_CMD_SET_MODE, Copy_Mode);

    /* Send command */
    status = ESP01_SendCommand(ESP01_TxBuffer);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for OK response */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_OK, ESP01_CMD_TIMEOUT);

    return status;
}

/*******************************************************************************************************/
/* ESP01_ConnectToAP                                                                                   */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Connect to WiFi Access Point                                               */
/* Function Input       -> SSID, Password                                                              */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_ConnectToAP(const uint8_t* Copy_SSID, const uint8_t* Copy_Password)
{
    ESP01_Status_t status;

    /* Prepare command */
    sprintf((char*)ESP01_TxBuffer, "%s\"%s\",\"%s\"\r\n", ESP01_CMD_CONNECT_AP, Copy_SSID, Copy_Password);

    /* Send command */
    status = ESP01_SendCommand(ESP01_TxBuffer);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for connection confirmation */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_WIFI_CONN, ESP01_CONNECT_TIMEOUT);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for IP assignment */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_GOT_IP, ESP01_CONNECT_TIMEOUT);

    return status;
}

/*******************************************************************************************************/
/* ESP01_DisconnectFromAP                                                                              */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Disconnect from WiFi Access Point                                          */
/* Function Input       -> None                                                                        */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_DisconnectFromAP(void)
{
    ESP01_Status_t status;

    /* Send disconnect command */
    status = ESP01_SendCommand((uint8_t*)ESP01_CMD_DISCONNECT_AP);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for OK response */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_OK, ESP01_CMD_TIMEOUT);

    return status;
}

/*******************************************************************************************************/
/* ESP01_GetIPAddress                                                                                  */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Get IP address information                                                  */
/* Function Input       -> Buffer for IP, buffer size                                                  */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_GetIPAddress(uint8_t* Copy_IPBuffer, uint8_t Copy_BufferSize)
{
    ESP01_Status_t status;
    uint8_t responseBuffer[ESP01_RX_BUFFER_SIZE];
    uint16_t bytesRead;

    /* Clear buffers */
    ESP01_ClearBuffer();
    memset(responseBuffer, 0, ESP01_RX_BUFFER_SIZE);

    /* Send get IP command */
    status = ESP01_SendCommand((uint8_t*)ESP01_CMD_GET_IP);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for response */
    STK_SudoDelay(ESP01_RESPONSE_DELAY);

    /* Read response */
    bytesRead = ESP01_ReadFromBuffer(responseBuffer, ESP01_RX_BUFFER_SIZE);

    /* Copy IP information to user buffer */
    if (bytesRead > 0 && bytesRead < Copy_BufferSize)
    {
        memcpy(Copy_IPBuffer, responseBuffer, bytesRead);
        Copy_IPBuffer[bytesRead] = '\0';
        return ESP01_STATUS_OK;
    }

    return ESP01_STATUS_ERROR;
}

/*******************************************************************************************************/
/* ESP01_ConnectTCP                                                                                    */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Establish TCP connection                                                    */
/* Function Input       -> Server IP, Port                                                             */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_ConnectTCP(const uint8_t* Copy_ServerIP, uint16_t Copy_Port)
{
    ESP01_Status_t status;

    /* Set single connection mode */
    sprintf((char*)ESP01_TxBuffer, "%s0\r\n", ESP01_CMD_SET_MUX);
    status = ESP01_SendCommand(ESP01_TxBuffer);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_OK, ESP01_CMD_TIMEOUT);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Prepare TCP connection command */
    sprintf((char*)ESP01_TxBuffer, "%s\"TCP\",\"%s\",%d\r\n", ESP01_CMD_START_TCP, Copy_ServerIP, Copy_Port);

    /* Send command */
    status = ESP01_SendCommand(ESP01_TxBuffer);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for connection confirmation */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_CONNECT, ESP01_CONNECT_TIMEOUT);

    return status;
}

/*******************************************************************************************************/
/* ESP01_SendData                                                                                      */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Send data over established connection                                       */
/* Function Input       -> Data buffer, length                                                         */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_SendData(const uint8_t* Copy_Data, uint16_t Copy_Length)
{
    ESP01_Status_t status;

    /* Prepare send command with length */
    sprintf((char*)ESP01_TxBuffer, "%s%d\r\n", ESP01_CMD_SEND_DATA, Copy_Length);

    /* Send command */
    status = ESP01_SendCommand(ESP01_TxBuffer);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for prompt (>) */
    STK_SudoDelay(100);

    /* Send actual data */
    USART_SendData(ESP01_UART_INSTANCE, (uint8_t*)Copy_Data, Copy_Length);

    /* Wait for send confirmation */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_SEND_OK, ESP01_CMD_TIMEOUT);

    return status;
}

/*******************************************************************************************************/
/* ESP01_CloseConnection                                                                               */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Close TCP connection                                                        */
/* Function Input       -> None                                                                        */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_CloseConnection(void)
{
    ESP01_Status_t status;
    uint8_t responseBuffer[ESP01_RX_BUFFER_SIZE];
    uint16_t bytesRead;

    /* First check if there's a CLOSED message in buffer */
    ESP01_ClearBuffer();
    STK_SudoDelay(100);

    /* Try to read any pending data */
    bytesRead = ESP01_ReadFromBuffer(responseBuffer, ESP01_RX_BUFFER_SIZE);

    /* Check if connection is already closed */
    if (bytesRead > 0)
    {
        if (ESP01_StringSearch(responseBuffer, bytesRead, (uint8_t*)ESP01_RESPONSE_CLOSED))
        {
            /* Connection already closed by server */
            return ESP01_STATUS_OK;
        }
    }

    /* If not already closed, send close command */
    status = ESP01_SendCommand((uint8_t*)ESP01_CMD_CLOSE_CONN);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for response - could be OK or ERROR (if already closed) */
    STK_SudoDelay(100);
    bytesRead = ESP01_ReadFromBuffer(responseBuffer, ESP01_RX_BUFFER_SIZE);

    if (bytesRead > 0)
    {
        /* Accept both OK and ERROR as valid responses */
        if (ESP01_StringSearch(responseBuffer, bytesRead, (uint8_t*)ESP01_RESPONSE_OK) ||
            ESP01_StringSearch(responseBuffer, bytesRead, (uint8_t*)ESP01_RESPONSE_ERROR))
        {
            return ESP01_STATUS_OK;
        }
    }

    return ESP01_STATUS_OK;  /* Even if no response, consider it closed */
}

/*******************************************************************************************************/
/* ESP01_SendCommand                                                                                   */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Send AT command to ESP01                                                    */
/* Function Input       -> Command string                                                              */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_SendCommand(const uint8_t* Copy_Command)
{
    uint16_t cmdLength = strlen((const char*)Copy_Command);
    USART_Status_t uart_status;

    if (cmdLength == 0 || cmdLength > ESP01_TX_BUFFER_SIZE)
    {
        return ESP01_STATUS_ERROR;
    }

    printf("ESP01: Sending command: %s", Copy_Command);  /* Command already has \r\n */

    /* Send command via UART */
    uart_status = USART_SendData(ESP01_UART_INSTANCE, (uint8_t*)Copy_Command, cmdLength);

    if (uart_status != USART_STATUS_OK)
    {
        printf("ESP01: UART send failed with status %d\r\n", uart_status);
        return ESP01_STATUS_ERROR;
    }

    return ESP01_STATUS_OK;
}

/*******************************************************************************************************/
/* ESP01_WaitForResponse                                                                               */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Wait for expected response from ESP01                                       */
/* Function Input       -> Expected response string, timeout                                           */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_WaitForResponse(const uint8_t* Copy_ExpectedResponse, uint32_t Copy_Timeout)
{
    uint8_t responseBuffer[ESP01_RX_BUFFER_SIZE];
    uint16_t bytesRead = 0;
    uint32_t timeoutCounter = 0;
    uint32_t maxIterations = Copy_Timeout / 10;

    memset(responseBuffer, 0, ESP01_RX_BUFFER_SIZE);

    printf("ESP01: Waiting for response '%s' (timeout: %lu ms)\r\n", Copy_ExpectedResponse, Copy_Timeout);

    while (timeoutCounter < maxIterations)
    {
        if (ESP01_DataReceived)
        {
            printf("ESP01: Data received flag set\r\n");

            uint16_t newBytes = ESP01_ReadFromBuffer(&responseBuffer[bytesRead],
                                                     ESP01_RX_BUFFER_SIZE - bytesRead);
            bytesRead += newBytes;
            ESP01_DataReceived = 0;

            if (newBytes > 0)
            {
                printf("ESP01: Read %d bytes, total: %d\r\n", newBytes, bytesRead);
                printf("ESP01: Buffer content: ");
                for (uint16_t i = 0; i < bytesRead; i++)
                {
                    if (responseBuffer[i] >= 32 && responseBuffer[i] <= 126)
                        printf("%c", responseBuffer[i]);
                    else
                        printf("[0x%02X]", responseBuffer[i]);
                }
                printf("\r\n");

                // Process any +IPD messages found in the buffer
                uint16_t search_pos = 0;
                while (bytesRead >= 5 && search_pos <= bytesRead - 5)
                {
                    // Look for "+IPD," pattern
                    if (responseBuffer[search_pos] == '+' &&
                        responseBuffer[search_pos + 1] == 'I' &&
                        responseBuffer[search_pos + 2] == 'P' &&
                        responseBuffer[search_pos + 3] == 'D' &&
                        responseBuffer[search_pos + 4] == ',')
                    {
                        printf("ESP01: Found +IPD in WaitForResponse buffer!\r\n");

                        // Parse length
                        search_pos += 5; // Skip "+IPD,"
                        uint16_t ipd_length = 0;

                        while (search_pos < bytesRead &&
                               responseBuffer[search_pos] >= '0' &&
                               responseBuffer[search_pos] <= '9')
                        {
                            ipd_length = ipd_length * 10 + (responseBuffer[search_pos] - '0');
                            search_pos++;
                        }

                        // Skip ':'
                        if (search_pos < bytesRead && responseBuffer[search_pos] == ':')
                        {
                            search_pos++;

                            printf("ESP01: IPD length: %d, available bytes: %d\r\n",
                                   ipd_length, bytesRead - search_pos);

                            // Check if we have all the MQTT data
                            if (search_pos + ipd_length <= bytesRead)
                            {
                                printf("ESP01: Processing MQTT packet immediately...\r\n");

                                // Process the MQTT PUBLISH packet
                                ESP01_ParseMQTTPublish(&responseBuffer[search_pos], ipd_length);

                                printf("ESP01: MQTT packet processed!\r\n");

                                // Continue searching for more +IPD messages
                                search_pos += ipd_length;
                            }
                            else
                            {
                                printf("ESP01: Incomplete IPD packet, need %d more bytes\r\n",
                                       ipd_length - (bytesRead - search_pos));
                                break; // Wait for more data
                            }
                        }
                    }
                    else
                    {
                        search_pos++;
                    }
                }
            }

            // Check if expected response is in buffer
            if (ESP01_StringSearch(responseBuffer, bytesRead, Copy_ExpectedResponse))
            {
                printf("ESP01: Found expected response!\r\n");
                return ESP01_STATUS_OK;
            }

            // Check for error response
            if (ESP01_StringSearch(responseBuffer, bytesRead, (uint8_t*)ESP01_RESPONSE_ERROR))
            {
                printf("ESP01: Received ERROR response\r\n");
                return ESP01_STATUS_ERROR;
            }
        }

        STK_SudoDelay(10);
        timeoutCounter++;
    }

    printf("ESP01: Timeout! Received %d bytes total\r\n", bytesRead);
    if (bytesRead > 0)
    {
        printf("ESP01: Final buffer: ");
        for (uint16_t i = 0; i < bytesRead; i++)
        {
            if (responseBuffer[i] >= 32 && responseBuffer[i] <= 126)
                printf("%c", responseBuffer[i]);
            else
                printf("[0x%02X]", responseBuffer[i]);
        }
        printf("\r\n");
    }

    return ESP01_STATUS_TIMEOUT;
}

/*******************************************************************************************************/
/* ESP01_ClearBuffer                                                                                   */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Clear receive buffer                                                        */
/* Function Input       -> None                                                                        */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void ESP01_ClearBuffer(void)
{
    ESP01_RxBuffer.writeIndex = 0;
    ESP01_RxBuffer.readIndex = 0;
    ESP01_RxBuffer.dataCount = 0;
    ESP01_DataReceived = 0;
    memset(ESP01_RxBuffer.buffer, 0, ESP01_RX_BUFFER_SIZE);
}

void ESP01_ProcessMQTTMessages(void)
{
    static uint8_t mqtt_buffer[512];
    static uint16_t mqtt_buffer_index = 0;
    uint8_t temp_buffer[256];
    uint16_t bytes_read;

    /* First, process any message that was detected in interrupt */
    if (ESP01_HasPendingMQTTMessage()) {
        ESP01_ProcessPendingMQTTMessage();
        return;  // Process one message at a time
    }

    /* Then check for any messages in the main buffer (fallback) */
    if (!ESP01_DataReceived) {
        return;
    }

    bytes_read = ESP01_ReadFromBuffer(temp_buffer, sizeof(temp_buffer));

    if (bytes_read == 0) {
        return;
    }

    /* Search for +IPD messages that might have been missed */
    for (uint16_t i = 0; i < bytes_read; i++) {
        if (i + 4 < bytes_read &&
            temp_buffer[i] == '+' &&
            temp_buffer[i+1] == 'I' &&
            temp_buffer[i+2] == 'P' &&
            temp_buffer[i+3] == 'D') {

            i += 5;

            uint16_t data_length = 0;
            while (i < bytes_read && temp_buffer[i] >= '0' && temp_buffer[i] <= '9') {
                data_length = data_length * 10 + (temp_buffer[i] - '0');
                i++;
            }

            if (i < bytes_read && temp_buffer[i] == ':') {
                i++;

                printf("ESP01: +IPD found in main buffer, length: %d\r\n", data_length);

                mqtt_buffer_index = 0;
                while (i < bytes_read && mqtt_buffer_index < data_length && mqtt_buffer_index < sizeof(mqtt_buffer)) {
                    mqtt_buffer[mqtt_buffer_index++] = temp_buffer[i++];
                }

                if (mqtt_buffer_index == data_length) {
                    ESP01_ParseMQTTPublish(mqtt_buffer, mqtt_buffer_index);
                    mqtt_buffer_index = 0;
                }
            }
        }
    }
}
/*******************************************************************************************************/
/* ESP01_HasPendingMQTTMessage - NEW FUNCTION                                                         */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Check if MQTT message was detected in interrupt                            */
/* Function Input       -> None                                                                        */
/* Function Return      -> 1 if message pending, 0 otherwise                                          */
/*******************************************************************************************************/

uint8_t ESP01_HasPendingMQTTMessage(void)
{
    return ESP01_MQTTMessagePending;
}

/*******************************************************************************************************/
/* ESP01_ProcessPendingMQTTMessage - NEW FUNCTION                                                     */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Process the MQTT message that was buffered in interrupt                    */
/* Function Input       -> None                                                                        */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void ESP01_ProcessPendingMQTTMessage(void)
{
    if (!ESP01_MQTTMessagePending) {
        return;
    }

    /* Disable interrupts to safely copy data and reset state */
    uint8_t local_buffer[512];
    uint16_t local_length;

    __disable_irq();

    local_length = ESP01_IPDBufferIndex;
    if (local_length > 0 && local_length <= sizeof(local_buffer)) {
        memcpy(local_buffer, (const void*)ESP01_IPDBuffer, local_length);
    }

    /* Clear pending flag and reset buffer index atomically */
    ESP01_MQTTMessagePending = 0;
    ESP01_IPDBufferIndex = 0;

    __enable_irq();

    /* Process the message outside interrupt context */
    if (local_length > 0) {
        printf("ESP01: Processing interrupt-buffered MQTT message (%d bytes, IPD said %d)\r\n",
               local_length, ESP01_IPDCompletedLength);
        printf("ESP01: [DEBUG] Captured: %d, Skipped: %d\r\n", ESP01_CapturedIPDCount, ESP01_SkippedIPDCount);

        /* Debug: show first bytes of the message */
        printf("ESP01: [DEBUG] Message bytes: ");
        for (uint16_t i = 0; i < local_length && i < 16; i++) {
            printf("0x%02X ", local_buffer[i]);
        }
        printf("\r\n");

        /* Check if this is a PINGRESP (0xD0 0x00) - skip it */
        if (local_length == 2 && local_buffer[0] == 0xD0 && local_buffer[1] == 0x00) {
            printf("ESP01: [DEBUG] Skipping PINGRESP packet\r\n");
        } else {
            ESP01_ParseMQTTPublish(local_buffer, local_length);
        }
    }
}

/*******************************************************************************************************/
/* ESP01_SetMultipleConnections                                                                        */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Enable or disable multiple connection mode                                  */
/* Function Input       -> Copy_Enable: 1 to enable, 0 to disable                                     */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_SetMultipleConnections(uint8_t Copy_Enable)
{
    ESP01_Status_t status;

    /* Prepare command: AT+CIPMUX=0 or AT+CIPMUX=1 */
    sprintf((char*)ESP01_TxBuffer, "%s%d\r\n", ESP01_CMD_SET_MUX, Copy_Enable ? 1 : 0);

    /* Send command */
    status = ESP01_SendCommand(ESP01_TxBuffer);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for OK response */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_OK, ESP01_CMD_TIMEOUT);
    if (status == ESP01_STATUS_OK)
    {
        ESP01_MultiConnectionEnabled = Copy_Enable ? 1 : 0;
        printf("ESP01: Multi-connection mode %s\r\n", Copy_Enable ? "enabled" : "disabled");
    }

    return status;
}

/*******************************************************************************************************/
/* ESP01_ConfigureAP                                                                                   */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Configure ESP01 as Access Point                                            */
/* Function Input       -> SSID, Password, Channel, Encryption type                                   */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_ConfigureAP(const uint8_t* Copy_SSID, const uint8_t* Copy_Password,
                                  uint8_t Copy_Channel, ESP01_Encryption_t Copy_Encryption)
{
    ESP01_Status_t status;

    /* Prepare command: AT+CWSAP="ssid","password",channel,encryption */
    sprintf((char*)ESP01_TxBuffer, "%s\"%s\",\"%s\",%d,%d\r\n",
            ESP01_CMD_CONFIGURE_AP, Copy_SSID, Copy_Password, Copy_Channel, Copy_Encryption);

    printf("ESP01: Configuring AP: SSID=%s, Channel=%d\r\n", Copy_SSID, Copy_Channel);

    /* Send command */
    status = ESP01_SendCommand(ESP01_TxBuffer);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for OK response - AP configuration can take a moment */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_OK, ESP01_CONNECT_TIMEOUT);
    if (status == ESP01_STATUS_OK)
    {
        printf("ESP01: AP configured successfully\r\n");
    }

    return status;
}

/*******************************************************************************************************/
/* ESP01_StartServer                                                                                   */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Start TCP server on specified port                                         */
/* Function Input       -> Copy_Port: Port number (e.g., 80 for HTTP)                                */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_StartServer(uint16_t Copy_Port)
{
    ESP01_Status_t status;

    /* Prepare command: AT+CIPSERVER=1,port */
    sprintf((char*)ESP01_TxBuffer, "%s1,%d\r\n", ESP01_CMD_SERVER_CONFIG, Copy_Port);

    printf("ESP01: Starting server on port %d...\r\n", Copy_Port);

    /* Send command */
    status = ESP01_SendCommand(ESP01_TxBuffer);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for OK response */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_OK, ESP01_CMD_TIMEOUT);
    if (status == ESP01_STATUS_OK)
    {
        printf("ESP01: Server started on port %d\r\n", Copy_Port);
    }

    return status;
}

/*******************************************************************************************************/
/* ESP01_StopServer                                                                                    */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Stop TCP server                                                            */
/* Function Input       -> None                                                                        */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_StopServer(void)
{
    ESP01_Status_t status;

    /* Prepare command: AT+CIPSERVER=0 */
    sprintf((char*)ESP01_TxBuffer, "%s0\r\n", ESP01_CMD_SERVER_CONFIG);

    /* Send command */
    status = ESP01_SendCommand(ESP01_TxBuffer);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for OK response */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_OK, ESP01_CMD_TIMEOUT);
    if (status == ESP01_STATUS_OK)
    {
        printf("ESP01: Server stopped\r\n");
    }

    return status;
}

/*******************************************************************************************************/
/* ESP01_SendDataToConnection                                                                          */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Send data to a specific connection (multi-connection mode)                 */
/* Function Input       -> Copy_ConnId: Connection ID (0-4), Data buffer, Length                     */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_SendDataToConnection(uint8_t Copy_ConnId, const uint8_t* Copy_Data, uint16_t Copy_Length)
{
    ESP01_Status_t status;

    if (Copy_ConnId >= ESP01_MAX_CONNECTIONS)
    {
        return ESP01_STATUS_ERROR;
    }

    /* Prepare send command with connection ID: AT+CIPSEND=<conn_id>,<length> */
    sprintf((char*)ESP01_TxBuffer, "%s%d,%d\r\n", ESP01_CMD_SEND_DATA, Copy_ConnId, Copy_Length);

    /* Send command */
    status = ESP01_SendCommand(ESP01_TxBuffer);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for prompt (>) */
    STK_SudoDelay(100);

    /* Send actual data */
    USART_SendData(ESP01_UART_INSTANCE, (uint8_t*)Copy_Data, Copy_Length);

    /* Wait for send confirmation */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_SEND_OK, ESP01_CMD_TIMEOUT);

    return status;
}

/*******************************************************************************************************/
/* ESP01_CloseConnectionById                                                                           */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Close a specific connection by ID                                          */
/* Function Input       -> Copy_ConnId: Connection ID to close (0-4)                                 */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_CloseConnectionById(uint8_t Copy_ConnId)
{
    ESP01_Status_t status;

    if (Copy_ConnId >= ESP01_MAX_CONNECTIONS)
    {
        return ESP01_STATUS_ERROR;
    }

    /* Prepare command: AT+CIPCLOSE=<conn_id> */
    sprintf((char*)ESP01_TxBuffer, "%s%d\r\n", ESP01_CMD_CLOSE_CONN_ID, Copy_ConnId);

    /* Send command */
    status = ESP01_SendCommand(ESP01_TxBuffer);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for OK or CLOSED response */
    status = ESP01_WaitForResponse((uint8_t*)ESP01_RESPONSE_OK, ESP01_CMD_TIMEOUT);

    return status;
}

/*******************************************************************************************************/
/* ESP01_GetAPIPAddress                                                                                */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Get AP mode IP address                                                     */
/* Function Input       -> Buffer for IP, buffer size                                                  */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ESP01_Status_t ESP01_GetAPIPAddress(uint8_t* Copy_IPBuffer, uint8_t Copy_BufferSize)
{
    ESP01_Status_t status;
    uint8_t responseBuffer[ESP01_RX_BUFFER_SIZE];
    uint16_t bytesRead;

    /* Clear buffers */
    ESP01_ClearBuffer();
    memset(responseBuffer, 0, ESP01_RX_BUFFER_SIZE);

    /* Send get AP IP command */
    status = ESP01_SendCommand((uint8_t*)ESP01_CMD_GET_AP_IP);
    if (status != ESP01_STATUS_OK)
    {
        return status;
    }

    /* Wait for response */
    STK_SudoDelay(ESP01_RESPONSE_DELAY);

    /* Read response */
    bytesRead = ESP01_ReadFromBuffer(responseBuffer, ESP01_RX_BUFFER_SIZE);

    /* Copy IP information to user buffer */
    if (bytesRead > 0 && bytesRead < Copy_BufferSize)
    {
        memcpy(Copy_IPBuffer, responseBuffer, bytesRead);
        Copy_IPBuffer[bytesRead] = '\0';
        return ESP01_STATUS_OK;
    }

    return ESP01_STATUS_ERROR;
}

/*******************************************************************************************************/
/* ESP01_RegisterHTTPCallback                                                                          */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Register callback for HTTP data reception                                  */
/* Function Input       -> callback: Function to call when HTTP data is received                     */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void ESP01_RegisterHTTPCallback(ESP01_HTTPDataCallback_t callback)
{
    g_HTTP_DataCallback = callback;
    printf("ESP01: HTTP callback registered\r\n");
}

/*******************************************************************************************************/
/* ESP01_HasPendingHTTPData                                                                            */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Check if HTTP data is pending from any connection                         */
/* Function Input       -> None                                                                        */
/* Function Return      -> 1 if data pending, 0 otherwise                                             */
/*******************************************************************************************************/

uint8_t ESP01_HasPendingHTTPData(void)
{
    return ESP01_HTTPBuffer.pending;
}

/*******************************************************************************************************/
/* ESP01_ProcessPendingHTTPData                                                                        */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Process pending HTTP data                                                  */
/* Function Input       -> None                                                                        */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void ESP01_ProcessPendingHTTPData(void)
{
    if (!ESP01_HTTPBuffer.pending || g_HTTP_DataCallback == NULL)
    {
        return;
    }

    /* Disable interrupts to safely copy data and reset state */
    uint8_t local_buffer[ESP01_HTTP_BUFFER_SIZE];
    uint16_t local_length;
    uint8_t local_conn_id;

    __disable_irq();

    local_length = ESP01_HTTPBuffer.length;
    local_conn_id = ESP01_HTTPBuffer.conn_id;

    if (local_length > 0 && local_length <= sizeof(local_buffer))
    {
        memcpy(local_buffer, ESP01_HTTPBuffer.buffer, local_length);
    }

    /* Clear pending flag atomically */
    ESP01_HTTPBuffer.pending = 0;
    ESP01_HTTPBuffer.length = 0;

    __enable_irq();

    /* Call the callback with the data */
    if (local_length > 0)
    {
        printf("ESP01: Processing HTTP data from conn %d (%d bytes)\r\n", local_conn_id, local_length);
        g_HTTP_DataCallback(local_conn_id, local_buffer, local_length);
    }
}
