/***********************************************************************/
/*Author           : Ameer Alwadiya                                    */
/*Version          : V1.0.0                                            */
/*Date             : 16 Jul 2025                                       */
/*Description      : stk_config.h --> Configuration                    */
/***********************************************************************/

/***********************************************************************/
/*             Include guard                                            */
/***********************************************************************/
#ifndef STK_CONFIG_H_
#define STK_CONFIG_H_

/***********************************************************************/
/* Configuration options for the STK (SysTick Timer) module            */
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

#endif /*STK_CONFIG_H_*/
