/****************************************************************************/
/* Author   : Ameer Alwadiya                                                */
/* Version  : V2.1.0                                                        */
/* Date     : 12 Aug 2025                                                   */
/* Desc     : mqtt_private.h --> MQTT Private Header File (Config-Based)    */
/****************************************************************************/
#ifndef MQTT_PRIVATE_H_
#define MQTT_PRIVATE_H_


/****************************************************************************/
/* Includes                                                                 */
/****************************************************************************/
#include <stdint.h>
#include "mqtt_config.h"  // Include config file
#include "mqtt_interface.h"  // Include config file


/****************************************************************************/
/* Private Definitions (Using Config)                                       */
/****************************************************************************/
#define MQTT_MAX_PACKET_SIZE        MQTT_CONFIG_MAX_PACKET_SIZE
#define MQTT_PROTOCOL_NAME          "MQTT"
#define MQTT_PROTOCOL_LEVEL         MQTT_CONFIG_PROTOCOL_VERSION
#define MQTT_KEEP_ALIVE_MSB         (uint8_t)(MQTT_CONFIG_KEEP_ALIVE_TIME >> 8)
#define MQTT_KEEP_ALIVE_LSB         (uint8_t)(MQTT_CONFIG_KEEP_ALIVE_TIME & 0xFF)


/****************************************************************************/
/* Packet Type Definitions                                                  */
/****************************************************************************/
#define MQTT_PACKET_TYPE_CONNECT        0x10
#define MQTT_PACKET_TYPE_PUBLISH_QOS0   0x30
#define MQTT_PACKET_TYPE_PUBLISH_QOS1   0x32
#define MQTT_PACKET_TYPE_SUBSCRIBE      0x82
#define MQTT_PACKET_TYPE_PINGREQ        0xC0


/****************************************************************************/
/* Connect Flags                                                            */
/****************************************************************************/
#define MQTT_CONNECT_FLAG_CLEAN_SESSION 0x02


/****************************************************************************/
/* Subscribe QoS (Using Config)                                             */
/****************************************************************************/
#define MQTT_SUBSCRIBE_QOS              MQTT_CONFIG_DEFAULT_SUB_QOS


/****************************************************************************/
/* Input Validation Macros (Using Config Limits)                            */
/****************************************************************************/
#define MQTT_VALIDATE_CLIENT_ID_LENGTH(len)    ((len) <= MQTT_CONFIG_MAX_CLIENT_ID_LEN)
#define MQTT_VALIDATE_TOPIC_LENGTH(len)        ((len) <= MQTT_CONFIG_MAX_TOPIC_LEN)
#define MQTT_VALIDATE_MESSAGE_LENGTH(len)      ((len) <= MQTT_CONFIG_MAX_MESSAGE_LEN)


/****************************************************************************/
/* Helper Function Prototypes                                               */
/****************************************************************************/
static void     MQTT_WriteUTF8String(uint8_t* Copy_Buffer, uint16_t* Copy_Index, const char* Copy_String);
static uint8_t  MQTT_CalculateConnectRemainingLength(const char* Copy_ClientId);
static uint8_t  MQTT_CalculatePublishRemainingLength(const char* Copy_TopicName, MQTT_QoS_t Copy_QoS, uint8_t Copy_MessageLength);
static uint8_t  MQTT_CalculateSubscribeRemainingLength(const char* Copy_TopicName);


#endif /* MQTT_PRIVATE_H_ */



