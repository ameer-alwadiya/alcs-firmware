/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.2                                                                          */
/* Date              : 01 Aug 2025                                                                     */
/* Description       : usart.h --> USART Driver (Merged Header)                                       */
/*******************************************************************************************************/

#ifndef USART_H_
#define USART_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

/*******************************************************************************************************/
/* USART Configuration Parameters                                                                      */
/*******************************************************************************************************/

/* USART1 Configuration */
#define USART1_BAUD_RATE            115200UL
#define USART1_WORD_LENGTH          USART_WORD_LENGTH_8
#define USART1_STOP_BITS            USART_STOP_BITS_1
#define USART1_PARITY               USART_PARITY_NONE
#define USART1_OVERSAMPLING         USART_OVERSAMPLING_16

/* USART2 Configuration */
#define USART2_BAUD_RATE            115200UL
#define USART2_WORD_LENGTH          USART_WORD_LENGTH_8
#define USART2_STOP_BITS            USART_STOP_BITS_1
#define USART2_PARITY               USART_PARITY_NONE
#define USART2_OVERSAMPLING         USART_OVERSAMPLING_16

/* USART Parity Options */
#define USART_PARITY_NONE           0
#define USART_PARITY_EVEN           1
#define USART_PARITY_ODD            2

/* USART Stop Bits Options */
#define USART_STOP_BITS_1           0
#define USART_STOP_BITS_2           1

/* USART Word Length Options */
#define USART_WORD_LENGTH_8         0
#define USART_WORD_LENGTH_9         1

/* USART Oversampling Options */
#define USART_OVERSAMPLING_16       0
#define USART_OVERSAMPLING_8        1

/*******************************************************************************************************/
/* USART Base Addresses                                                                                */
/*******************************************************************************************************/

#define USART1_BASE_ADDRESS     0x40011000UL
#define USART2_BASE_ADDRESS     0x40004400UL

/*******************************************************************************************************/
/* USART Timeout Value                                                                                 */
/*******************************************************************************************************/

#define USART_TIMEOUT_VALUE     100000UL

/*******************************************************************************************************/
/* USART Register Structure                                                                            */
/*******************************************************************************************************/

typedef struct
{
    uint32_t SBK        :1;
    uint32_t RWU        :1;
    uint32_t RE         :1;
    uint32_t TE         :1;
    uint32_t IDLEIE     :1;
    uint32_t RXNEIE     :1;
    uint32_t TCIE       :1;
    uint32_t TXEIE      :1;
    uint32_t PEIE       :1;
    uint32_t PS         :1;
    uint32_t PCE        :1;
    uint32_t WAKE       :1;
    uint32_t M          :1;
    uint32_t UE         :1;
    uint32_t RESERVE    :1;
    uint32_t OVER8      :1;
    uint32_t RESERVED2  :16;
} USART_CR1_t;

typedef struct
{
    uint32_t SR;
    uint32_t DR;
    uint32_t BRR;
    USART_CR1_t CR1;
    uint32_t CR2;
    uint32_t CR3;
    uint32_t GTPR;
} USART_Registers_t;

/*******************************************************************************************************/
/* USART Register Pointers                                                                             */
/*******************************************************************************************************/

#define USART1_REG   ((volatile USART_Registers_t*)(USART1_BASE_ADDRESS))
#define USART2_REG   ((volatile USART_Registers_t*)(USART2_BASE_ADDRESS))

/*******************************************************************************************************/
/* USART Status Register Bits                                                                          */
/*******************************************************************************************************/

#define USART_SR_RXNE_BIT       5
#define USART_SR_TC_BIT         6
#define USART_SR_TXE_BIT        7

/*******************************************************************************************************/
/* USART Control Register 2 Bits                                                                       */
/*******************************************************************************************************/

#define USART_CR2_STOP_BIT12    12
#define USART_CR2_STOP_BIT13    13

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
int __io_putchar(int ch);

#endif /* USART_H_ */
