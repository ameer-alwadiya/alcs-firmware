/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.1                                                                          */
/* Date              : 01 Aug 2025                                                                     */
/* Description       : usart_interface.h --> USART Interface Header File                              */
/*******************************************************************************************************/

#ifndef USART_INTERFACE_H_
#define USART_INTERFACE_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

/*******************************************************************************************************/
/* USART Instance Definitions                                                                          */
/*******************************************************************************************************/

typedef enum
{
    USART_INSTANCE_1 = 0,
    USART_INSTANCE_2
} USART_Instance_t;

/*******************************************************************************************************/
/* Function Return Status                                                                              */
/*******************************************************************************************************/

typedef enum
{
    USART_STATUS_OK = 0,
    USART_STATUS_ERROR,
    USART_STATUS_INVALID_INSTANCE,
    USART_STATUS_TIMEOUT
} USART_Status_t;

/*******************************************************************************************************/
/* Function Prototypes                                                                                 */
/*******************************************************************************************************/

USART_Status_t USART_Initialize(USART_Instance_t Copy_Instance);
USART_Status_t USART_SendData(USART_Instance_t Copy_Instance, uint8_t* Copy_u8Data, uint8_t Copy_u8Length);
uint8_t USART_ReadData(USART_Instance_t Copy_Instance);
USART_Status_t USART_Enable(USART_Instance_t Copy_Instance);
USART_Status_t USART_Disable(USART_Instance_t Copy_Instance);
USART_Status_t USART_SetCallback(USART_Instance_t Copy_Instance, void(*Copy_Fptr)(void));
USART_Status_t USART_WriteByte(USART_Instance_t Copy_Instance, uint8_t Copy_u8Data);

/* Printf support function */
//int __io_putchar(int ch);

#endif /* USART_INTERFACE_H_ */

//#ifndef __UART_H__
//#define __UART_H__
//
//
//#include <stdint.h>
//#include "stm32f4xx.h"
//
//
//void uart2_tx_init(void);
//
//
//#endif




