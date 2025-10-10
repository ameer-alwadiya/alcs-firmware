/****************************************************************************/
/* Author            : Ameer Alwadiya                                       */
/* Version           : V2.0.0                                               */
/* Date              : 10 Aug 2025                                          */
/* Description       : mqtt_config.h --> MQTT Configuration Header File     */
/****************************************************************************/
#ifndef MQTT_CONFIG_H_
#define MQTT_CONFIG_H_


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


#endif /* MQTT_CONFIG_H_ */



