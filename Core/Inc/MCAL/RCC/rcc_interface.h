/***********************************************************************/
/*Author           : Ameer Alwadiya                                    */
/*Version          : V1.0.0                                            */
/*Date             : 15 Jul 2025                                       */
/*Description      : rcc_interface.h --> Function Prototypes           */
/*Module Features  :                                                   */
/*        01- RCC_EnablePeripheralClock                                */
/*        02- RCC_DisablePeripheralClock                               */
/*        03- RCC_InitializeSystemClock                                */
/***********************************************************************/

/***********************************************************************/
/*    Public API for RCC (Reset and Clock Control) module             */
/*        * Function prototypes and public macros                      */
/*        * Bus definitions and peripheral enable macros               */
/***********************************************************************/

/***********************************************************************/
/*             Include guard                                            */
/***********************************************************************/
#ifndef RCC_INTERFACE_H_
#define RCC_INTERFACE_H_

/***********************************************************************/
/*                           HAL Library                               */
/***********************************************************************/
#include <stdint.h>
#include <stdio.h>

/***********************************************************************/
/*                           Bus Types                                  */
/***********************************************************************/
typedef enum
{
    RCC_BUS_AHB1 = 0,
    RCC_BUS_AHB2,
    RCC_BUS_APB1,
    RCC_BUS_APB2
} RCC_BusType_t;

/***********************************************************************/
/*                        Function Prototypes                          */
/***********************************************************************/

/**
 * @brief   Enable peripheral clock
 * @param   busType: The Bus of The Peripheral [RCC_BUS_AHB1, RCC_BUS_AHB2,
 *                   RCC_BUS_APB1, RCC_BUS_APB2]
 * @param   peripheralBit: The bit position of Peripheral in the Enable Register
 * @retval  None
 */
void RCC_EnablePeripheralClock(RCC_BusType_t busType, uint8_t peripheralBit);

/**
 * @brief   Disable peripheral clock
 * @param   busType: The Bus of The Peripheral [RCC_BUS_AHB1, RCC_BUS_AHB2,
 *                   RCC_BUS_APB1, RCC_BUS_APB2]
 * @param   peripheralBit: The bit position of Peripheral in the Enable Register
 * @retval  None
 */
void RCC_DisablePeripheralClock(RCC_BusType_t busType, uint8_t peripheralBit);

/**
 * @brief   Initialize system clock
 * @param   None
 * @retval  None
 */
void RCC_InitializeSystemClock(void);

/***********************************************************************/
/*                     AHB1 Peripheral Clock Enable Bits              */
/***********************************************************************/
#define RCC_AHB1_GPIOA_BIT     0U
#define RCC_AHB1_GPIOB_BIT     1U
#define RCC_AHB1_GPIOC_BIT     2U
#define RCC_AHB1_GPIOD_BIT     3U
#define RCC_AHB1_GPIOE_BIT     4U
#define RCC_AHB1_GPIOH_BIT     7U
#define RCC_AHB1_CRC_BIT       12U
#define RCC_AHB1_DMA1_BIT      21U
#define RCC_AHB1_DMA2_BIT      22U

/***********************************************************************/
/*                     AHB2 Peripheral Clock Enable Bits              */
/***********************************************************************/
#define RCC_AHB2_OTGFS_BIT     7U

/***********************************************************************/
/*                     APB1 Peripheral Clock Enable Bits              */
/***********************************************************************/
#define RCC_APB1_TIM2_BIT      0U
#define RCC_APB1_TIM3_BIT      1U
#define RCC_APB1_TIM4_BIT      2U
#define RCC_APB1_TIM5_BIT      3U
#define RCC_APB1_WWDG_BIT      11U
#define RCC_APB1_SPI2_BIT      14U
#define RCC_APB1_SPI3_BIT      15U
#define RCC_APB1_USART2_BIT    17U
#define RCC_APB1_I2C1_BIT      21U
#define RCC_APB1_I2C2_BIT      22U
#define RCC_APB1_I2C3_BIT      23U
#define RCC_APB1_PWR_BIT       28U

/***********************************************************************/
/*                     APB2 Peripheral Clock Enable Bits              */
/***********************************************************************/
#define RCC_APB2_TIM1_BIT      0U
#define RCC_APB2_USART1_BIT    4U
#define RCC_APB2_USART6_BIT    5U
#define RCC_APB2_ADC1_BIT      8U
#define RCC_APB2_SDIO_BIT      11U
#define RCC_APB2_SPI1_BIT      12U
#define RCC_APB2_SPI4_BIT      13U
#define RCC_APB2_SYSCFG_BIT    14U
#define RCC_APB2_TIM9_BIT      16U
#define RCC_APB2_TIM10_BIT     17U
#define RCC_APB2_TIM11_BIT     18U

#endif /*RCC_INTERFACE_H_*/
