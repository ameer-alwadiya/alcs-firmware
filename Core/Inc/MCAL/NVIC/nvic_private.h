/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 16 Jul 2025                                                                     */
/* Description       : nvic_private.h --> NVIC Private Definitions                                    */
/* Features          : Register Definitions                                                            */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* Include Guard                                                                                       */
/*******************************************************************************************************/

#ifndef NVIC_PRIVATE_H_
#define NVIC_PRIVATE_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

/*******************************************************************************************************/
/* Base Address Definitions                                                                            */
/*******************************************************************************************************/

#define NVIC_BASE_ADDRESS    0xE000E100U

/*******************************************************************************************************/
/* Register Definitions                                                                                */
/*******************************************************************************************************/

/* NVIC register map structure */
typedef struct
{
    uint32_t ISER[8];              /* Interrupt Set Enable Registers */
    uint32_t RESERVED0[24];        /* Reserved */
    uint32_t ICER[8];              /* Interrupt Clear Enable Registers */
    uint32_t RESERVED1[24];        /* Reserved */
    uint32_t ISPR[8];              /* Interrupt Set Pending Registers */
    uint32_t RESERVED2[24];        /* Reserved */
    uint32_t ICPR[8];              /* Interrupt Clear Pending Registers */
    uint32_t RESERVED3[24];        /* Reserved */
    uint32_t IABR[8];              /* Interrupt Active Bit Registers */
    uint32_t RESERVED4[56];        /* Reserved */
    uint8_t  IPR[240];             /* Interrupt Priority Registers */
    uint32_t RESERVED5[580];       /* Reserved */
    uint32_t STIR;                 /* Software Trigger Interrupt Register */
} nvic_registers_t;

/*******************************************************************************************************/
/* Register Access Macros                                                                              */
/*******************************************************************************************************/

#define NVIC_REG    ((volatile nvic_registers_t*)(NVIC_BASE_ADDRESS))

/* System Control Block - Application Interrupt and Reset Control Register */
#define SCB_AIRCR   (*(volatile uint32_t*) 0xE000ED0CU)

/* NVIC Vector Key for AIRCR register */
#define NVIC_VECTKEY     0x05FA0000U

/*******************************************************************************************************/
/* Private Function Prototypes                                                                         */
/*******************************************************************************************************/

#endif /* NVIC_PRIVATE_H_ */
