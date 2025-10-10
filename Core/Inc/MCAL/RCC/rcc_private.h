/***********************************************************************/
/*Author           : Ameer Alwadiya                                    */
/*Version          : V1.0.0                                            */
/*Date             : 15 Jul 2025                                       */
/*Description      : rcc_private.h --> Register Definitions            */
/***********************************************************************/

/***********************************************************************/
/*             Include guard                                            */
/***********************************************************************/
#ifndef RCC_PRIVATE_H_
#define RCC_PRIVATE_H_

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
/*                     RCC Configuration Register Bit Definitions     */
/***********************************************************************/
#define RCC_CFGR_SW_BIT0                 0U
#define RCC_CFGR_SW_BIT1                 1U
#define RCC_CFGR_SWS_BIT0                2U
#define RCC_CFGR_SWS_BIT1                3U

#endif /*RCC_PRIVATE_H_*/
