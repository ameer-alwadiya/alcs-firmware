/***********************************************************************/
/*Author           : Ameer Alwadiya                                    */
/*Version          : V1.1.0                                            */
/*Date             : 12 Nov 2025                                       */
/*Description      : rcc.h        									   */
/*Module Features  :                                                   */
/*        01- RCC_EnablePeripheralClock                                */
/*        02- RCC_DisablePeripheralClock                               */
/*        03- RCC_InitializeSystemClock                                */
/*        04- RCC_EnableLSI (for IWDG)                                 */
/*        05- RCC_CheckResetSource                                     */
/***********************************************************************/

/***********************************************************************/
/*    Public API for RCC (Reset and Clock Control) module              */
/*        * Function prototypes and public macros                      */
/*        * Bus definitions and peripheral enable macros               */
/***********************************************************************/

/***********************************************************************/
/*             Include guard                                           */
/***********************************************************************/
#ifndef RCC_H_
#define RCC_H_

/***********************************************************************/
/*                           HAL Library                               */
/***********************************************************************/
#include <stdint.h>
#include <stdio.h>

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

/***********************************************************************/
/*                        LSI Frequency Configuration                  */
/***********************************************************************/
/**
 * Typical LSI frequency (used by IWDG)
 * Note: Actual frequency varies ±10% (28-36 kHz typical)
 */
#define RCC_LSI_FREQUENCY_HZ            32000U

/***********************************************************************/
/*                        Register Definitions                         */
/*---------------------------------------------------------------------*/
/*     - Private register definitions for internal use only            */
/*     - Memory-mapped register access                                 */
/*     - Hardware abstraction layer                                    */
/***********************************************************************/

/***********************************************************************/
/*                  RCC Base Address                                   */
/***********************************************************************/
#define  RCC_BASE_ADDRESS      0x40023800UL

/***********************************************************************/
/*                     RCC Register Memory Map                         */
/***********************************************************************/
#define    RCC_CR              (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x00UL)))
#define    RCC_PLLCFGR         (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x04UL)))
#define    RCC_CFGR            (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x08UL)))
#define    RCC_CIR             (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x0CUL)))
#define    RCC_AHB1RSTR        (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x10UL)))
#define    RCC_AHB2RSTR        (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x14UL)))
#define    RCC_APB1RSTR        (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x20UL)))
#define    RCC_APB2RSTR        (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x24UL)))
#define    RCC_AHB1ENR         (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x30UL)))
#define    RCC_AHB2ENR         (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x34UL)))
#define    RCC_APB1ENR         (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x40UL)))
#define    RCC_APB2ENR         (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x44UL)))
#define    RCC_AHB1LPENR       (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x50UL)))
#define    RCC_AHB2LPENR       (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x54UL)))
#define    RCC_APB1LPENR       (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x60UL)))
#define    RCC_APB2LPENR       (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x64UL)))
#define    RCC_BDCR            (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x70UL)))
#define    RCC_CSR             (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x74UL)))
#define    RCC_SSCGR           (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x80UL)))
#define    RCC_PLLI2SCFGR      (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x84UL)))
#define    RCC_DCKCFGR         (*((volatile uint32_t*)(RCC_BASE_ADDRESS + 0x8CUL)))

/***********************************************************************/
/*                     Clock Source Configuration Values               */
/***********************************************************************/

/* Clock Source Options */
#define RCC_CLOCK_SOURCE_HSI             0U
#define RCC_CLOCK_SOURCE_HSE             1U
#define RCC_CLOCK_SOURCE_PLL             2U

/* HSE Source Options */
#define RCC_HSE_SOURCE_CRYSTAL           0U
#define RCC_HSE_SOURCE_RC                1U

/* PLL Source Options */
#define RCC_PLL_SOURCE_HSE               0U
#define RCC_PLL_SOURCE_HSI               1U

/***********************************************************************/
/*                     RCC Control Register Bit Definitions           */
/***********************************************************************/
#define RCC_CR_HSION_BIT                 0U
#define RCC_CR_HSIRDY_BIT                1U
#define RCC_CR_HSEON_BIT                 16U
#define RCC_CR_HSERDY_BIT                17U
#define RCC_CR_HSEBYP_BIT                18U
#define RCC_CR_PLLON_BIT                 24U
#define RCC_CR_PLLRDY_BIT                25U

/***********************************************************************/
/*                     RCC Configuration Register Bit Definitions      */
/***********************************************************************/
#define RCC_CFGR_SW_BIT0                 0U
#define RCC_CFGR_SW_BIT1                 1U
#define RCC_CFGR_SWS_BIT0                2U
#define RCC_CFGR_SWS_BIT1                3U

/***********************************************************************/
/*         RCC Control/Status Register (CSR) Bit Definitions           */
/***********************************************************************/
#define RCC_CSR_LSION_BIT                0U   /* LSI oscillator enable */
#define RCC_CSR_LSIRDY_BIT               1U   /* LSI oscillator ready */
#define RCC_CSR_RMVF_BIT                 24U  /* Remove reset flag */
#define RCC_CSR_BORRSTF_BIT              25U  /* BOR reset flag */
#define RCC_CSR_PINRSTF_BIT              26U  /* PIN reset flag */
#define RCC_CSR_PORRSTF_BIT              27U  /* POR/PDR reset flag */
#define RCC_CSR_SFTRSTF_BIT              28U  /* Software reset flag */
#define RCC_CSR_IWDGRSTF_BIT             29U  /* IWDG reset flag */
#define RCC_CSR_WWDGRSTF_BIT             30U  /* WWDG reset flag */
#define RCC_CSR_LPWRRSTF_BIT             31U  /* Low-power reset flag */

/***********************************************************************/
/*                           Bus Types                                 */
/***********************************************************************/
typedef enum
{
    RCC_BUS_AHB1 = 0,
    RCC_BUS_AHB2,
    RCC_BUS_APB1,
    RCC_BUS_APB2
} RCC_BusType_t;

/***********************************************************************/
/*                         Reset Source Types                          */
/***********************************************************************/
typedef enum
{
    RCC_RESET_UNKNOWN = 0,
    RCC_RESET_LOWPOWER,
    RCC_RESET_WWDG,
    RCC_RESET_IWDG,
    RCC_RESET_SOFTWARE,
    RCC_RESET_POR,
    RCC_RESET_PIN,
    RCC_RESET_BOR
} RCC_ResetSource_t;

/***********************************************************************/
/*                         Status Types                                */
/***********************************************************************/
typedef enum
{
    RCC_STATUS_OK = 0,
    RCC_STATUS_ERROR,
    RCC_STATUS_TIMEOUT
} RCC_Status_t;

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

/**
 * @brief   Enable LSI clock (for IWDG)
 * @param   None
 * @retval  RCC_Status_t Status of operation
 * @note    LSI is typically ~32kHz with ±10% tolerance
 */
RCC_Status_t RCC_EnableLSI(void);

/**
 * @brief   Check if LSI is ready
 * @param   None
 * @retval  uint8_t 1 = ready, 0 = not ready
 */
uint8_t RCC_IsLSIReady(void);

/**
 * @brief   Get reset source from last reset
 * @param   None
 * @retval  RCC_ResetSource_t Reset source
 * @note    Call before RCC_ClearResetFlags()
 */
RCC_ResetSource_t RCC_GetResetSource(void);

/**
 * @brief   Check if reset was caused by IWDG
 * @param   None
 * @retval  uint8_t 1 = IWDG reset, 0 = other reset
 */
uint8_t RCC_WasResetByIWDG(void);

/**
 * @brief   Clear all reset flags
 * @param   None
 * @retval  None
 */
void RCC_ClearResetFlags(void);

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

#endif /*RCC_H_*/
