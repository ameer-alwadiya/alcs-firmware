 /***********************************************************************/
/* Author           : Ameer Alwadiya                                   */
/* Version          : V1.0.0                                           */
/* Date             : 16 Jul 2025                                      */
/* Description      : stk_program.c --> Function Implementation        */
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
/*        11- STK_DelayElapsed                                          */
/***********************************************************************/


/***********************************************************************/
/*                           HAL Library                               */
/***********************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>   // For bool type support


/***********************************************************************/
/*                           Own Header Files                          */
/***********************************************************************/
#include "stk.h"


/***********************************************************************/
/*                        Global Variables                             */
/***********************************************************************/
volatile uint32_t STK_msTicks = 0;


/**
* @brief   Initialize SysTick timer for 1ms tick interrupts
* @param   None
* @retval  None
* @note    Sets SysTick to generate interrupts every 1 ms using AHB/8 clock
*/
void STK_Initialize(void)
{
   /* Set reload register for 1ms tick */
   STK->LOAD = (SYSCLK_FREQ_HZ / 8 / 1000) - 1; // Assuming AHB/8 clock source


   /* Clear current value register */
   STK->VAL = 0;


   /* Enable SysTick: clock source = AHB/8, enable interrupt, enable counter */
   STK->CTRL = STK_CTRL_TICKINT_MASK | STK_CTRL_ENABLE_MASK;
   /* Note: Clock source bit cleared for AHB/8 */
}


/**
* @brief   Start SysTick timer with preload value
* @param   preloadValue: Value to load into reload register (0 to 0xFFFFFF)
* @retval  None
*/
void STK_Start(uint32_t preloadValue)
{
#if STK_PARAMETER_CHECK == STK_PARAM_CHECK_ENABLE
   if (preloadValue > STK_MAX_RELOAD_VALUE)
   {
       preloadValue = STK_MAX_RELOAD_VALUE;
   }
#endif
   STK->LOAD = preloadValue & STK_MAX_RELOAD_VALUE;
   STK->VAL = 0UL;
   STK->CTRL |= STK_CTRL_ENABLE_MASK;
}


/**
* @brief   Blocking delay in milliseconds using SysTick
* @param   delayMs: Delay time in milliseconds
* @retval  None
*/
void STK_DelayMs(uint32_t delayMs)
{
#if STK_PARAMETER_CHECK == STK_PARAM_CHECK_ENABLE
   if (delayMs == 0UL)
   {
       return;
   }
#endif


   uint32_t start = STK_GetMillis();
   while ((STK_GetMillis() - start) < delayMs)
   {
       /* blocking wait */
   }

}


/**
* @brief   Blocking delay in microseconds using SysTick
* @param   delayUs: Delay time in microseconds (minimum 2us)
* @retval  None
*/
void STK_DelayUs(uint32_t delayUs)
{
#if STK_PARAMETER_CHECK == STK_PARAM_CHECK_ENABLE
   if (delayUs < STK_TICKS_PER_MICROSECOND)
   {
       return;
   }
#endif


   /* Disable interrupt for delay operation */
   STK_ControlInterrupt(STK_INTERRUPT_DISABLE);


   uint32_t ticksNeeded = delayUs / STK_TICKS_PER_MICROSECOND;


   STK_Start(ticksNeeded);


   while (STK_ReadFlag() == STK_FLAG_CLEAR)
   {
       /* Waiting for timer to expire */
   }


   STK_Stop();


   /* Re-enable interrupt */
   STK_ControlInterrupt(STK_INTERRUPT_ENABLE);
}


/**
* @brief   Software busy-wait delay (sudo delay)
* @param   count: Approximate loop count for delay length
* @retval  None
* @note    Simple blocking delay using empty loop
*/
void STK_SudoDelay(uint32_t count)
{
   volatile uint32_t i;
   for (i = 0; i < count * 2000; i++)
   {
       __asm("nop");  // Optional: prevent optimization of empty loop
   }
}


/**
* @brief   Read SysTick count flag
* @param   None
* @retval  STK_FlagStatus_t: Flag status
*/
STK_FlagStatus_t STK_ReadFlag(void)
{
   if ((STK->CTRL & STK_CTRL_COUNTFLAG_MASK) != 0UL)
   {
       return STK_FLAG_SET;
   }
   return STK_FLAG_CLEAR;
}


/**
* @brief   Control SysTick interrupt state
* @param   interruptState: Interrupt state
* @retval  None
*/
void STK_ControlInterrupt(STK_InterruptState_t interruptState)
{
   STK->CTRL &= ~STK_CTRL_TICKINT_MASK;
   STK->CTRL |= ((uint32_t)interruptState << STK_CTRL_TICKINT_BIT);
}


/**
* @brief   Stop SysTick timer
* @param   None
* @retval  None
*/
void STK_Stop(void)
{
   STK->CTRL &= ~STK_CTRL_ENABLE_MASK;
   STK->VAL = 0UL;
}


/**
* @brief   Get remaining SysTick count value
* @param   None
* @retval  uint32_t: Current count value
*/
uint32_t STK_GetRemainingCount(void)
{
   return (STK->VAL & STK_MAX_CURRENT_VALUE);
}


/**
* @brief   Get elapsed milliseconds since timer start
* @param   None
* @retval  uint32_t: Milliseconds count
*/
uint32_t STK_GetMillis(void)
{
   return STK_msTicks;
}


/**
* @brief   Check if a specified delay has elapsed (non-blocking)
* @param   timestamp: Pointer to previous timestamp (updated on elapsed)
* @param   delay_ms: Delay duration in milliseconds
* @retval  bool: true if delay elapsed, false otherwise
*/
bool STK_DelayElapsed(uint32_t *timestamp, uint32_t delay_ms)
{
   if ((STK_GetMillis() - *timestamp) >= delay_ms)
   {
       *timestamp = STK_GetMillis();
       return true;
   }
   return false;
}


/**
* @brief   SysTick interrupt handler increments millisecond tick counter
* @param   None
* @retval  None
*/
void SysTick_Handler(void)
{
   STK_msTicks++;
}



