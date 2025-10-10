/****************************************************************************/
/* Author  : Ameer Alwadiya                                                */
/* Version : V2.1.0                                                        */
/* Date    : 12 Aug 2025                                                   */
/* Desc    : mqtt_program.c --> MQTT Implementation File (Modified ESP01)  */
/****************************************************************************/


/****************************************************************************/
/* Includes                                                                 */
/****************************************************************************/
#include "mqtt_private.h"
#include "mqtt_config.h"
#include "mqtt_interface.h"
#include "esp01_interface.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


/****************************************************************************/
/* Global Variables                                                         */
/****************************************************************************/
static volatile uint16_t MQTT_PacketID = 1U;
static uint8_t MQTT_PacketBuffer[MQTT_MAX_PACKET_SIZE];
static uint16_t MQTT_PacketIndex;


/****************************************************************************/
/* Helper Functions                                                         */
/****************************************************************************/


/* Write UTF-8 string to packet buffer */
static void MQTT_WriteUTF8String(uint8_t* Copy_Buffer, uint16_t* Copy_Index, const char* Copy_String)
{
   uint16_t stringLength = (uint16_t)strlen(Copy_String);
   Copy_Buffer[(*Copy_Index)++] = (uint8_t)(stringLength >> 8);
   Copy_Buffer[(*Copy_Index)++] = (uint8_t)(stringLength & 0xFF);
   strcpy((char*)&Copy_Buffer[*Copy_Index], Copy_String);
   *Copy_Index += stringLength;
}


/* Calculate remaining length for CONNECT packet */
static uint8_t MQTT_CalculateConnectRemainingLength(const char* Copy_ClientId)
{
//   uint8_t protocolNameLen = (uint8_t)strlen(MQTT_PROTOCOL_NAME);
   uint8_t clientIdLen = (uint8_t)strlen(Copy_ClientId);
   return (10U + 2U + clientIdLen);
}


/* Calculate remaining length for PUBLISH packet */
static uint8_t MQTT_CalculatePublishRemainingLength(const char* Copy_TopicName, MQTT_QoS_t Copy_QoS, uint8_t Copy_MessageLength)
{
   uint8_t topicNameLen = (uint8_t)strlen(Copy_TopicName);
   uint8_t remainingLength = 2U + topicNameLen + Copy_MessageLength;
   if (Copy_QoS > MQTT_QOS_0) remainingLength += 2U;
   return remainingLength;
}


/* Calculate remaining length for SUBSCRIBE packet */
static uint8_t MQTT_CalculateSubscribeRemainingLength(const char* Copy_TopicName)
{
   uint8_t topicNameLen = (uint8_t)strlen(Copy_TopicName);
   return (2U + 2U + topicNameLen + 1U);
}


/* Send MQTT packet through ESP01 TCP connection */
static MQTT_Status_t MQTT_SendPacket(const uint8_t* Copy_Packet, uint16_t Copy_Length)
{
   ESP01_Status_t esp_status;
   printf("MQTT: Sending %d byte packet.", Copy_Length);
   printf("\r\n");


   esp_status = ESP01_SendData(Copy_Packet, Copy_Length);
   if (esp_status == ESP01_STATUS_OK)
   {
       printf("MQTT: Packet sent successfully via ESP01\r\n");
       return MQTT_STATUS_OK;
   }
   else
   {
       printf("MQTT: Failed to send packet via ESP01, status: %d\r\n", esp_status);
       return MQTT_STATUS_ERROR;
   }
}


/****************************************************************************/
/* Function Implementations                                                 */
/****************************************************************************/


/* Send MQTT CONNECT packet */
MQTT_Status_t MQTT_Connect(const char* Copy_ClientId)
{
   uint16_t clientIdLen;
   if (Copy_ClientId == NULL || strlen(Copy_ClientId) == 0)
   {
       printf("MQTT: Invalid client ID\r\n");
       return MQTT_STATUS_INVALID_PARAMETER;
   }
   clientIdLen = strlen(Copy_ClientId);
   if (!MQTT_VALIDATE_CLIENT_ID_LENGTH(clientIdLen))
   {
       printf("MQTT: Client ID too long (%d > %d)\r\n", clientIdLen, MQTT_CONFIG_MAX_CLIENT_ID_LEN);
       return MQTT_STATUS_INVALID_PARAMETER;
   }


   uint8_t remainingLength = MQTT_CalculateConnectRemainingLength(Copy_ClientId);
   if ((2U + remainingLength) > MQTT_MAX_PACKET_SIZE)
   {
       printf("MQTT: CONNECT packet too large\r\n");
       return MQTT_STATUS_BUFFER_OVERFLOW;
   }


   MQTT_PacketIndex = 0;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = MQTT_PACKET_TYPE_CONNECT;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = remainingLength;
   MQTT_WriteUTF8String(MQTT_PacketBuffer, &MQTT_PacketIndex, MQTT_PROTOCOL_NAME);
   MQTT_PacketBuffer[MQTT_PacketIndex++] = MQTT_PROTOCOL_LEVEL;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = MQTT_CONNECT_FLAG_CLEAN_SESSION;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = MQTT_KEEP_ALIVE_MSB;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = MQTT_KEEP_ALIVE_LSB;
   MQTT_WriteUTF8String(MQTT_PacketBuffer, &MQTT_PacketIndex, Copy_ClientId);


   printf("MQTT: Constructed CONNECT packet for client '%s'\r\n", Copy_ClientId);
   return MQTT_SendPacket(MQTT_PacketBuffer, MQTT_PacketIndex);
}


/* Send MQTT PUBLISH packet */
MQTT_Status_t MQTT_Publish(const char* Copy_TopicName, MQTT_QoS_t Copy_QoS, const uint8_t* Copy_Message, uint8_t Copy_MessageLength)
{
   uint16_t topicLen;
   if (Copy_TopicName == NULL || strlen(Copy_TopicName) == 0 ||
       Copy_Message == NULL || Copy_MessageLength == 0 ||
       Copy_QoS > MQTT_QOS_2)
   {
       printf("MQTT: Invalid PUBLISH parameters\r\n");
       return MQTT_STATUS_INVALID_PARAMETER;
   }
   topicLen = strlen(Copy_TopicName);
   if (!MQTT_VALIDATE_TOPIC_LENGTH(topicLen))
   {
       printf("MQTT: Topic name too long (%d > %d)\r\n", topicLen, MQTT_CONFIG_MAX_TOPIC_LEN);
       return MQTT_STATUS_INVALID_PARAMETER;
   }
   if (!MQTT_VALIDATE_MESSAGE_LENGTH(Copy_MessageLength))
   {
       printf("MQTT: Message too long (%d > %d)\r\n", Copy_MessageLength, MQTT_CONFIG_MAX_MESSAGE_LEN);
       return MQTT_STATUS_INVALID_PARAMETER;
   }


   uint8_t remainingLength = MQTT_CalculatePublishRemainingLength(Copy_TopicName, Copy_QoS, Copy_MessageLength);
   if ((2U + remainingLength) > MQTT_MAX_PACKET_SIZE)
   {
       printf("MQTT: PUBLISH packet too large\r\n");
       return MQTT_STATUS_BUFFER_OVERFLOW;
   }


   MQTT_PacketIndex = 0;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = (Copy_QoS > MQTT_QOS_0) ? MQTT_PACKET_TYPE_PUBLISH_QOS1 : MQTT_PACKET_TYPE_PUBLISH_QOS0;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = remainingLength;
   MQTT_WriteUTF8String(MQTT_PacketBuffer, &MQTT_PacketIndex, Copy_TopicName);


   if (Copy_QoS > MQTT_QOS_0)
   {
       MQTT_PacketBuffer[MQTT_PacketIndex++] = (uint8_t)(MQTT_PacketID >> 8);
       MQTT_PacketBuffer[MQTT_PacketIndex++] = (uint8_t)(MQTT_PacketID & 0xFF);
       MQTT_PacketID++;
   }
   for (uint8_t i = 0; i < Copy_MessageLength; i++) MQTT_PacketBuffer[MQTT_PacketIndex++] = Copy_Message[i];


   printf("MQTT: Constructed PUBLISH packet to topic '%s' with %d bytes\r\n", Copy_TopicName, Copy_MessageLength);
   return MQTT_SendPacket(MQTT_PacketBuffer, MQTT_PacketIndex);
}


/* Send MQTT SUBSCRIBE packet */
MQTT_Status_t MQTT_Subscribe(const char* Copy_TopicName)
{
   uint16_t topicLen;
   if (Copy_TopicName == NULL || strlen(Copy_TopicName) == 0)
   {
       printf("MQTT: Invalid topic name for SUBSCRIBE\r\n");
       return MQTT_STATUS_INVALID_PARAMETER;
   }
   topicLen = strlen(Copy_TopicName);
   if (!MQTT_VALIDATE_TOPIC_LENGTH(topicLen))
   {
       printf("MQTT: Topic name too long (%d > %d)\r\n", topicLen, MQTT_CONFIG_MAX_TOPIC_LEN);
       return MQTT_STATUS_INVALID_PARAMETER;
   }


   uint8_t remainingLength = MQTT_CalculateSubscribeRemainingLength(Copy_TopicName);
   if ((2U + remainingLength) > MQTT_MAX_PACKET_SIZE)
   {
       printf("MQTT: SUBSCRIBE packet too large\r\n");
       return MQTT_STATUS_BUFFER_OVERFLOW;
   }


   MQTT_PacketIndex = 0;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = MQTT_PACKET_TYPE_SUBSCRIBE;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = remainingLength;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = (uint8_t)(MQTT_PacketID >> 8);
   MQTT_PacketBuffer[MQTT_PacketIndex++] = (uint8_t)(MQTT_PacketID & 0xFF);
   MQTT_PacketID++;
   MQTT_WriteUTF8String(MQTT_PacketBuffer, &MQTT_PacketIndex, Copy_TopicName);
   MQTT_PacketBuffer[MQTT_PacketIndex++] = MQTT_SUBSCRIBE_QOS;


   printf("MQTT: Constructed SUBSCRIBE packet for topic '%s'\r\n", Copy_TopicName);
   return MQTT_SendPacket(MQTT_PacketBuffer, MQTT_PacketIndex);
}


/* Send MQTT PINGREQ packet */
MQTT_Status_t MQTT_SendPingRequest(void)
{
   MQTT_PacketIndex = 0;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = MQTT_PACKET_TYPE_PINGREQ;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = 0x00;
   printf("MQTT: Constructed PINGREQ packet\r\n");
   return MQTT_SendPacket(MQTT_PacketBuffer, MQTT_PacketIndex);
}


/* Send MQTT DISCONNECT packet and close connection */
MQTT_Status_t MQTT_Disconnect(void)
{
   MQTT_Status_t mqtt_status;
   MQTT_PacketIndex = 0;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = 0xE0;
   MQTT_PacketBuffer[MQTT_PacketIndex++] = 0x00;
   printf("MQTT: Sending DISCONNECT packet\r\n");
   mqtt_status = MQTT_SendPacket(MQTT_PacketBuffer, MQTT_PacketIndex);
   ESP01_CloseConnection();
   printf("MQTT: Disconnected from broker\r\n");
   return mqtt_status;
}



