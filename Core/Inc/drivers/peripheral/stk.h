/***********************************************************************/
/* Author           : Ameer Alwadiya                                   */
/* Version          : V1.0.0                                           */
/* Date             : 16 Jul 2025                                      */
/* Description      : stk.h --> STK (SysTick Timer) Driver             */
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
/*                           Include Guard                             */
/***********************************************************************/
#ifndef STK_INTERFACE_H_
#define STK_INTERFACE_H_




/***********************************************************************/
/*                           HAL Library                               */
/***********************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>




/***********************************************************************/
/*                        Configuration Section                        */
/***********************************************************************/

/***********************************************************************/
/*                        Clock Source Configuration                   */
/***********************************************************************/
/**
 * Select the SysTick Clock Source from the Following Options:
 *      1) STK_CLOCK_SOURCE_AHB_DIV8    - AHB/8 (External clock)
 *      2) STK_CLOCK_SOURCE_AHB         - AHB (Processor clock)
 */
#define STK_CLOCK_SOURCE_SELECT         STK_CLOCK_SOURCE_AHB_DIV8

/***********************************************************************/
/*                        Timing Configuration                         */
/***********************************************************************/
/**
 * System Clock Frequency in Hz (used for timing calculations)
 * Default: 16MHz (HSI)
 */
#define STK_SYSTEM_CLOCK_FREQUENCY_HZ   16000000UL

/***********************************************************************/
/*                        Interrupt Configuration                      */
/***********************************************************************/
/**
 * Default interrupt state after initialization
 *      1) STK_INTERRUPT_ENABLE
 *      2) STK_INTERRUPT_DISABLE
 */
#define STK_DEFAULT_INTERRUPT_STATE     STK_INTERRUPT_DISABLE

/***********************************************************************/
/*                        Safety Configuration                         */
/***********************************************************************/
/**
 * Enable parameter validation in functions
 *      1) STK_PARAM_CHECK_ENABLE
 *      2) STK_PARAM_CHECK_DISABLE
 */
#define STK_PARAMETER_CHECK             STK_PARAM_CHECK_ENABLE

/* Parameter check options */
#define STK_PARAM_CHECK_ENABLE          1U
#define STK_PARAM_CHECK_DISABLE         0U

/***********************************************************************/
/*                        Debug Configuration                          */
/***********************************************************************/
/**
 * Enable debug mode for additional checks
 *      1) STK_DEBUG_ENABLE
 *      2) STK_DEBUG_DISABLE
 */
#define STK_DEBUG_MODE                  STK_DEBUG_DISABLE

/* Debug mode options */
#define STK_DEBUG_ENABLE                1U
#define STK_DEBUG_DISABLE               0U




/***********************************************************************/
/*                    Register Definitions Section                     */
/***********************************************************************/

/***********************************************************************/
/*                        SysTick Base Address                         */
/***********************************************************************/
#define STK_BASE_ADDRESS                 0xE000E010UL


/***********************************************************************/
/*                     SysTick Register Structure                      */
/***********************************************************************/
typedef struct
{
   volatile uint32_t CTRL;    /* SysTick Control and Status Register  */
   volatile uint32_t LOAD;    /* SysTick Reload Value Register        */
   volatile uint32_t VAL;     /* SysTick Current Value Register       */
   volatile uint32_t CALIB;   /* SysTick Calibration Register         */
} STK_RegisterMap_t;


/***********************************************************************/
/*                     SysTick Register Access                         */
/***********************************************************************/
#define STK  ((volatile STK_RegisterMap_t*)STK_BASE_ADDRESS)


/***********************************************************************/
/*            SysTick Control Register Bit Definitions                 */
/***********************************************************************/
#define STK_CTRL_ENABLE_BIT              0U      /* Counter enable bit */
#define STK_CTRL_TICKINT_BIT             1U      /* Interrupt enable bit */
#define STK_CTRL_CLKSOURCE_BIT           2U      /* Clock source selection bit */
#define STK_CTRL_COUNTFLAG_BIT           16U     /* Count flag bit */


/***********************************************************************/
/*                    SysTick Clock Source Options                     */
/***********************************************************************/
#define STK_CLOCK_SOURCE_AHB_DIV8        0U      /* AHB/8 */
#define STK_CLOCK_SOURCE_AHB             1U      /* AHB */


/***********************************************************************/
/*                     SysTick Maximum Values                          */
/***********************************************************************/
#define STK_MAX_RELOAD_VALUE             0x00FFFFFFUL    /* 24-bit reload value */
#define STK_MAX_CURRENT_VALUE            0x00FFFFFFUL    /* 24-bit current value */


/***********************************************************************/
/*                     SysTick Timing Constants                        */
/***********************************************************************/
/* Assuming HSI = 16MHz, AHB = 16MHz, SysTick Clock = AHB/8 = 2MHz */
#define STK_CLOCK_FREQUENCY_HZ           2000000UL       /* 2MHz */
#define SYSCLK_FREQ_HZ  			     16000000UL
#define STK_TICKS_PER_MICROSECOND        2UL             /* 2 ticks per microsecond */
#define STK_TICKS_PER_MILLISECOND        2000UL          /* 2000 ticks per millisecond */


/***********************************************************************/
/*                        Register Bit Masks                           */
/***********************************************************************/
#define STK_CTRL_ENABLE_MASK             (1UL << STK_CTRL_ENABLE_BIT)
#define STK_CTRL_TICKINT_MASK            (1UL << STK_CTRL_TICKINT_BIT)
#define STK_CTRL_CLKSOURCE_MASK          (1UL << STK_CTRL_CLKSOURCE_BIT)
#define STK_CTRL_COUNTFLAG_MASK          (1UL << STK_CTRL_COUNTFLAG_BIT)




/***********************************************************************/
/*                    Public Types and API Section                     */
/***********************************************************************/

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




/**
* @brief   Check if a specified delay has elapsed (non-blocking)
* @param   timestamp: Pointer to previous timestamp (updated on elapsed)
* @param   delay_ms: Delay duration in milliseconds
* @retval  bool: true if delay elapsed, false otherwise
*/
bool STK_DelayElapsed(uint32_t *timestamp, uint32_t delay_ms);




#endif /* STK_INTERFACE_H_ */



