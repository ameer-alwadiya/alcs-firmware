/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 16 Jul 2025                                                                     */
/* Description       : nvic_interface.h --> NVIC Driver Interface                                     */
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

#ifndef NVIC_INTERFACE_H_
#define NVIC_INTERFACE_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

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

#endif /* NVIC_INTERFACE_H_ */
