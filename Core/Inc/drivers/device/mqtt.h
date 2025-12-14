/****************************************************************************/
/* Author   : Ameer Alwadiya                                                */
/* Version  : V2.1.0                                                        */
/* Date     : 12 Aug 2025                                                   */
/* Desc     : mqtt.h --> MQTT Driver Header (Merged)                        */
/****************************************************************************/
#ifndef MQTT_H_
#define MQTT_H_

/****************************************************************************/
/* Includes                                                                 */
/****************************************************************************/
#include <stdint.h>


/****************************************************************************/
/* MQTT Configuration Parameters                                            */
/****************************************************************************/

/* Maximum packet size in bytes */
#define MQTT_CONFIG_MAX_PACKET_SIZE     220U

/* Maximum client ID length */
#define MQTT_CONFIG_MAX_CLIENT_ID_LEN   50U

/* Maximum topic name length */
#define MQTT_CONFIG_MAX_TOPIC_LEN       64U

/* Maximum message length */
#define MQTT_CONFIG_MAX_MESSAGE_LEN     200U

/* Keep-alive time in seconds (0xFFFF = 65535 seconds) */
#define MQTT_CONFIG_KEEP_ALIVE_TIME     0xFFFFU

/* Protocol version */
#define MQTT_CONFIG_PROTOCOL_VERSION    4U

/* Default QoS level for subscriptions */
#define MQTT_CONFIG_DEFAULT_SUB_QOS     1U


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
/* MQTT QoS Levels                                                          */
/****************************************************************************/
typedef enum
{
   MQTT_QOS_0 = 0,   // At most once
   MQTT_QOS_1,       // At least once
   MQTT_QOS_2        // Exactly once
} MQTT_QoS_t;

/****************************************************************************/
/* Function Return Status                                                   */
/****************************************************************************/
typedef enum
{
   MQTT_STATUS_OK = 0,
   MQTT_STATUS_ERROR,
   MQTT_STATUS_INVALID_PARAMETER,
   MQTT_STATUS_BUFFER_OVERFLOW,
   MQTT_STATUS_CONNECTION_LOST
} MQTT_Status_t;


/****************************************************************************/
/* Function Prototypes                                                      */
/****************************************************************************/
MQTT_Status_t MQTT_Connect(const char* Copy_ClientId);
MQTT_Status_t MQTT_Publish(const char* Copy_TopicName, MQTT_QoS_t Copy_QoS, const uint8_t* Copy_Message, uint8_t Copy_MessageLength);
MQTT_Status_t MQTT_Subscribe(const char* Copy_TopicName);
MQTT_Status_t MQTT_SendPingRequest(void);
MQTT_Status_t MQTT_SendPubAck(uint16_t Copy_PacketId);
MQTT_Status_t MQTT_Disconnect(void);


#endif /* MQTT_H_ */
