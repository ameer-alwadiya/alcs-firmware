 /***********************************************************************/
/* Author           : Ameer Alwadiya                                   */
/* Version          : V1.0.0                                           */
/* Date             : 16 Jul 2025                                      */
/* Description      : stk_interface.h --> Function Prototypes          */
/* Module Features  :                                                  */
/*        01- STK_Initialize                                            */
/*        02- STK_Start                                                 */
/*        03- STK_DelayMs                                               */
/*        04- STK_DelayUs                                               */
/*        05- STK_SudoDelay                                             */
/*        06- STK_ReadFlag                                              */
/*        07- STK_ControlInterrupt                                      */
/*        08- STK_Stop                                                  */
/*        09- STK_GetRemainingCount                                     */
/*        10- STK_GetMillis                                             */
/***********************************************************************/




/***********************************************************************/
/*    Public API for STK (SysTick Timer) module                        */
/*        * Function prototypes and public macros                      */
/*        * Interrupt control definitions                              */
/***********************************************************************/




/***********************************************************************/
/*                           Include Guard                             */
/***********************************************************************/
#ifndef STK_INTERFACE_H_
#define STK_INTERFACE_H_




/***********************************************************************/
/*                           HAL Library                               */
/***********************************************************************/
#include <stdint.h>
#include <stdio.h>




/***********************************************************************/
/*                           Global Variables                          */
/***********************************************************************/
extern volatile uint8_t STK_DelayComplete;




/***********************************************************************/
/*                           Interrupt Control                         */
/***********************************************************************/
typedef enum
{
  STK_INTERRUPT_DISABLE = 0,
  STK_INTERRUPT_ENABLE  = 1
} STK_InterruptState_t;




/***********************************************************************/
/*                           Flag Status                               */
/***********************************************************************/
typedef enum
{
  STK_FLAG_CLEAR = 0,
  STK_FLAG_SET   = 1
} STK_FlagStatus_t;




/***********************************************************************/
/*                        Function Prototypes                          */
/***********************************************************************/
/**
* @brief   Initialize SysTick timer
* @param   None
* @retval  None
* @note    Disables SysTick interrupt, stops timer, sets clock source to AHB/8
*/
void STK_Initialize(void);




/**
* @brief   Start SysTick timer with preload value
* @param   preloadValue: Value to load into reload register (0 to 0xFFFFFF)
* @retval  None
* @note    Loads reload value, clears current value, and enables SysTick
*/
void STK_Start(uint32_t preloadValue);




/**
* @brief   Create millisecond delay using SysTick
* @param   delayMs: Delay time in milliseconds
* @retval  None
* @note    Blocking delay function using polling
*/
void STK_DelayMs(uint32_t delayMs);




/**
* @brief   Create microsecond delay using SysTick
* @param   delayUs: Delay time in microseconds (minimum 2us)
* @retval  None
* @note    Blocking delay function using polling
*/
void STK_DelayUs(uint32_t delayUs);




/**
* @brief   Software busy-wait delay (sudo delay)
* @param   count: Approximate loop count for delay length
* @retval  None
* @note    Simple blocking delay using empty loop
*/
void STK_SudoDelay(uint32_t count);




/**
* @brief   Read SysTick count flag
* @param   None
* @retval  STK_FlagStatus_t: Flag status (STK_FLAG_CLEAR or STK_FLAG_SET)
* @note    Flag is automatically cleared when read
*/
STK_FlagStatus_t STK_ReadFlag(void);




/**
* @brief   Control SysTick interrupt state
* @param   interruptState: Interrupt state (STK_INTERRUPT_ENABLE or STK_INTERRUPT_DISABLE)
* @retval  None
*/
void STK_ControlInterrupt(STK_InterruptState_t interruptState);




/**
* @brief   Stop SysTick timer
* @param   None
* @retval  None
*/
void STK_Stop(void);




/**
* @brief   Get remaining count value
* @param   None
* @retval  uint32_t: Current count value
*/
uint32_t STK_GetRemainingCount(void);




/**
* @brief   Get elapsed milliseconds since timer start
* @param   None
* @retval  uint32_t: Milliseconds count
*/
uint32_t STK_GetMillis(void);




#endif /* STK_INTERFACE_H_ */



