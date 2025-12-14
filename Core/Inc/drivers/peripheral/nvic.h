/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 16 Jul 2025                                                                     */
/* Description       : nvic.h --> NVIC Driver (Merged)                                                */
/* Module Features   :                                                                                 */
/*      01- NVIC_EnableInterrupt                                                                       */
/*      02- NVIC_DisableInterrupt                                                                      */
/*      03- NVIC_EnableInterruptPending                                                                */
/*      04- NVIC_DisableInterruptPending                                                               */
/*      05- NVIC_IsInterruptActive                                                                     */
/*      06- NVIC_SetInterruptPriority                                                                  */
/*      07- NVIC_SetInterruptGroupMode                                                                 */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* Include Guard                                                                                       */
/*******************************************************************************************************/

#ifndef NVIC_H_
#define NVIC_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

/*******************************************************************************************************/
/* Configuration Options                                                                               */
/*******************************************************************************************************/

/* Default interrupt group mode configuration */
#define NVIC_DEFAULT_GROUP_MODE    NVIC_GROUP_MODE_G16S0

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
/* Type Definitions                                                                                    */
/*******************************************************************************************************/

typedef enum
{
    NVIC_GROUP_MODE_G16S0 = 3,      /* 16 Group priorities, 0 Sub-group priorities */
    NVIC_GROUP_MODE_G8S2,           /* 8 Group priorities, 2 Sub-group priorities */
    NVIC_GROUP_MODE_G4S4,           /* 4 Group priorities, 4 Sub-group priorities */
    NVIC_GROUP_MODE_G2S8,           /* 2 Group priorities, 8 Sub-group priorities */
    NVIC_GROUP_MODE_G0S16           /* 0 Group priorities, 16 Sub-group priorities */
} nvic_group_mode_t;

/*******************************************************************************************************/
/* Function Prototypes                                                                                 */
/*******************************************************************************************************/

void NVIC_EnableInterrupt(uint8_t interrupt_position);
void NVIC_DisableInterrupt(uint8_t interrupt_position);
void NVIC_EnableInterruptPending(uint8_t interrupt_position);
void NVIC_DisableInterruptPending(uint8_t interrupt_position);
uint8_t NVIC_IsInterruptActive(uint8_t interrupt_position);
void NVIC_SetInterruptPriority(uint8_t interrupt_position, nvic_group_mode_t group_mode, uint8_t group_num, uint8_t sub_group_num);
void NVIC_SetInterruptGroupMode(nvic_group_mode_t group_mode);

#endif /* NVIC_H_ */
