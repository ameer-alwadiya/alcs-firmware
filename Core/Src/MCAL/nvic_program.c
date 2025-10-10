/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 16 Jul 2025                                                                     */
/* Description       : nvic_program.c --> NVIC Driver Implementation                                  */
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
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include "nvic_private.h"
#include "nvic_interface.h"
#include "nvic_config.h"
#include "bit_math.h"

/*******************************************************************************************************/
/* Function Implementations                                                                            */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* NVIC_EnableInterrupt                                                                                */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Enable specific interrupt                                                   */
/* Function Input       -> interrupt_position: Interrupt position number (0-239)                      */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void NVIC_EnableInterrupt(uint8_t interrupt_position)
{
    if (interrupt_position < 240)
    {
        NVIC_REG->ISER[interrupt_position / 32] = (1U << (interrupt_position % 32));
    }
}

/*******************************************************************************************************/
/* NVIC_DisableInterrupt                                                                               */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Disable specific interrupt                                                  */
/* Function Input       -> interrupt_position: Interrupt position number (0-239)                      */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void NVIC_DisableInterrupt(uint8_t interrupt_position)
{
    if (interrupt_position < 240)
    {
        NVIC_REG->ICER[interrupt_position / 32] = (1U << (interrupt_position % 32));
    }
}

/*******************************************************************************************************/
/* NVIC_EnableInterruptPending                                                                         */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Set interrupt pending flag                                                  */
/* Function Input       -> interrupt_position: Interrupt position number (0-239)                      */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void NVIC_EnableInterruptPending(uint8_t interrupt_position)
{
    if (interrupt_position < 240)
    {
        NVIC_REG->ISPR[interrupt_position / 32] = (1U << (interrupt_position % 32));
    }
}

/*******************************************************************************************************/
/* NVIC_DisableInterruptPending                                                                        */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Clear interrupt pending flag                                                */
/* Function Input       -> interrupt_position: Interrupt position number (0-239)                      */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void NVIC_DisableInterruptPending(uint8_t interrupt_position)
{
    if (interrupt_position < 240)
    {
        NVIC_REG->ICPR[interrupt_position / 32] = (1U << (interrupt_position % 32));
    }
}

/*******************************************************************************************************/
/* NVIC_IsInterruptActive                                                                              */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Check if interrupt is active                                                */
/* Function Input       -> interrupt_position: Interrupt position number (0-239)                      */
/* Function Return      -> 1 if active, 0 if not active                                               */
/*******************************************************************************************************/

uint8_t NVIC_IsInterruptActive(uint8_t interrupt_position)
{
    uint8_t result = 0;

    if (interrupt_position < 240)
    {
        result = GET_BIT(NVIC_REG->IABR[interrupt_position / 32], (interrupt_position % 32));
    }

    return result;
}

/*******************************************************************************************************/
/* NVIC_SetInterruptPriority                                                                           */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Set interrupt priority based on group mode                                  */
/* Function Input       -> interrupt_position: Interrupt position number (0-239)                      */
/*                      -> group_mode: Group mode configuration                                        */
/*                      -> group_num: Group priority number                                            */
/*                      -> sub_group_num: Sub-group priority number                                    */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void NVIC_SetInterruptPriority(uint8_t interrupt_position, nvic_group_mode_t group_mode, uint8_t group_num, uint8_t sub_group_num)
{
    uint8_t priority_value = 0;

    if (interrupt_position < 240)
    {
        switch (group_mode)
        {
            case NVIC_GROUP_MODE_G0S16:
                priority_value = sub_group_num;
                break;
            case NVIC_GROUP_MODE_G16S0:
                priority_value = group_num;
                break;
            case NVIC_GROUP_MODE_G4S4:
                priority_value = (group_num << 2) | sub_group_num;
                break;
            case NVIC_GROUP_MODE_G2S8:
                priority_value = (group_num << 3) | sub_group_num;
                break;
            case NVIC_GROUP_MODE_G8S2:
                priority_value = (group_num << 1) | sub_group_num;
                break;
            default:
                priority_value = 0;
                break;
        }

        NVIC_REG->IPR[interrupt_position] = (priority_value << 4);
    }
}

/*******************************************************************************************************/
/* NVIC_SetInterruptGroupMode                                                                          */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Set interrupt group mode configuration                                      */
/* Function Input       -> group_mode: Group mode configuration                                        */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void NVIC_SetInterruptGroupMode(nvic_group_mode_t group_mode)
{
    /* Clear AIRCR register first */
    SCB_AIRCR = NVIC_VECTKEY;

    /* Set the group mode */
    SCB_AIRCR = NVIC_VECTKEY | ((uint32_t)group_mode << 8);
}
