/***********************************************************************/
/*Author           : Ameer Alwadiya                                    */
/*Version          : V1.0.0                                            */
/*Date             : 15 Jul 2025                                       */
/*Description      : rcc_config.h --> Configuration                    */
/***********************************************************************/

/***********************************************************************/
/*             Include guard                                            */
/***********************************************************************/
#ifndef RCC_CONFIG_H_
#define RCC_CONFIG_H_

/***********************************************************************/
/* Configuration options for the RCC (Reset and Clock Control) module  */
/***********************************************************************/

/***********************************************************************/
/*                        Clock Source Configuration                   */
/***********************************************************************/
/**
 * Select the Clock Source from the Following Options:
 *      1) RCC_CLOCK_SOURCE_HSI
 *      2) RCC_CLOCK_SOURCE_HSE
 *      3) RCC_CLOCK_SOURCE_PLL
 */
#define RCC_SYSTEM_CLOCK_SOURCE         RCC_CLOCK_SOURCE_HSI

/***********************************************************************/
/*                        HSE Source Configuration                     */
/***********************************************************************/
/**
 * Select the Source of HSE from the Following Options:
 *      1) RCC_HSE_SOURCE_CRYSTAL
 *      2) RCC_HSE_SOURCE_RC
 */
#define RCC_HSE_SOURCE_TYPE             RCC_HSE_SOURCE_CRYSTAL

/***********************************************************************/
/*                        PLL Source Configuration                     */
/***********************************************************************/
/**
 * Select the Source of PLL from the Following Options:
 *      1) RCC_PLL_SOURCE_HSE
 *      2) RCC_PLL_SOURCE_HSI
 */
#define RCC_PLL_INPUT_SOURCE            RCC_PLL_SOURCE_HSE

/***********************************************************************/
/*                        HSE Frequency Configuration                  */
/***********************************************************************/
/**
 * Enter the External Clock Frequency between 4 - 26 MHz
 */
#define RCC_HSE_FREQUENCY_MHZ           25U

#endif /*RCC_CONFIG_H_*/
