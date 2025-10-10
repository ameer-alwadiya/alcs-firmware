/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.1                                                                          */
/* Date              : 01 Aug 2025                                                                     */
/* Description       : usart_config.h --> USART Configuration Header File                             */
/*******************************************************************************************************/

#ifndef USART_CONFIG_H_
#define USART_CONFIG_H_

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

#endif /* USART_CONFIG_H_ */
