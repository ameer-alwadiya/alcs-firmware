/****************************************************************************/
/* Author   : Ameer Alwadiya                                                */
/* Version  : V2.1.0                                                        */
/* Date     : 12 Aug 2025                                                   */
/* Desc     : mqtt_interface.h --> MQTT Interface Header File               */
/****************************************************************************/
#ifndef MQTT_INTERFACE_H_
#define MQTT_INTERFACE_H_
/****************************************************************************/
/* Includes                                                                 */
/****************************************************************************/
#include <stdint.h>
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
MQTT_Status_t MQTT_Disconnect(void);  // Disconnect function


#endif /* MQTT_INTERFACE_H_ */





