/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 08 Aug 2025                                                                     */
/* Description       : esp01_config.h --> ESP01 Configuration Header File                             */
/*******************************************************************************************************/

#ifndef ESP01_CONFIG_H_
#define ESP01_CONFIG_H_

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

#endif /* ESP01_CONFIG_H_ */
