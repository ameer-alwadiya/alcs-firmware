/***********************************************************************/
/*Author           : Ameer Alwadiya                                    */
/*Version          : V1.0.0                                            */
/*Date             : 15 Jul 2025                                       */
/*Description      : rcc_program.c --> Function Implementation         */
/*Module Features  :                                                   */
/*        01- RCC_EnablePeripheralClock                                */
/*        02- RCC_DisablePeripheralClock                               */
/*        03- RCC_InitializeSystemClock                                */
/***********************************************************************/

/***********************************************************************/
/*                           HAL Library                               */
/***********************************************************************/
#include <stdint.h>
#include <stdio.h>

/***********************************************************************/
/*                            Own Header Files                         */
/***********************************************************************/
#include "rcc_interface.h"
#include "rcc_private.h"
#include "rcc_config.h"

/***********************************************************************/
/*                        Function Implementation                      */
/***********************************************************************/

/**
 * @brief   Enable peripheral clock
 * @param   busType: The Bus of The Peripheral [RCC_BUS_AHB1, RCC_BUS_AHB2,
 *                   RCC_BUS_APB1, RCC_BUS_APB2]
 * @param   peripheralBit: The bit position of Peripheral in the Enable Register
 * @retval  None
 */
void RCC_EnablePeripheralClock(RCC_BusType_t busType, uint8_t peripheralBit)
{
    switch(busType)
    {
        case RCC_BUS_AHB1:
            RCC_AHB1ENR |= (1U << peripheralBit);
            break;
        case RCC_BUS_AHB2:
            RCC_AHB2ENR |= (1U << peripheralBit);
            break;
        case RCC_BUS_APB1:
            RCC_APB1ENR |= (1U << peripheralBit);
            break;
        case RCC_BUS_APB2:
            RCC_APB2ENR |= (1U << peripheralBit);
            break;
        default:
            /* Invalid bus type - do nothing */
            break;
    }
}

/**
 * @brief   Disable peripheral clock
 * @param   busType: The Bus of The Peripheral [RCC_BUS_AHB1, RCC_BUS_AHB2,
 *                   RCC_BUS_APB1, RCC_BUS_APB2]
 * @param   peripheralBit: The bit position of Peripheral in the Enable Register
 * @retval  None
 */
void RCC_DisablePeripheralClock(RCC_BusType_t busType, uint8_t peripheralBit)
{
    switch(busType)
    {
        case RCC_BUS_AHB1:
            RCC_AHB1ENR &= ~(1U << peripheralBit);
            break;
        case RCC_BUS_AHB2:
            RCC_AHB2ENR &= ~(1U << peripheralBit);
            break;
        case RCC_BUS_APB1:
            RCC_APB1ENR &= ~(1U << peripheralBit);
            break;
        case RCC_BUS_APB2:
            RCC_APB2ENR &= ~(1U << peripheralBit);
            break;
        default:
            /* Invalid bus type - do nothing */
            break;
    }
}

/**
 * @brief   Initialize system clock
 * @param   None
 * @retval  None
 */
void RCC_InitializeSystemClock(void)
{
    #if RCC_SYSTEM_CLOCK_SOURCE == RCC_CLOCK_SOURCE_HSI
        /* HSI Clock Source Configuration */
        /* 1- Enable HSI */
        RCC_CR |= (1U << RCC_CR_HSION_BIT);

        /* 2- Wait for HSI to be ready */
        while(!(RCC_CR & (1U << RCC_CR_HSIRDY_BIT)));

        /* 3- Set System Clock Source to HSI */
        RCC_CFGR &= ~(1U << RCC_CFGR_SW_BIT0);
        RCC_CFGR &= ~(1U << RCC_CFGR_SW_BIT1);

        /* 4- Wait for system clock switch to complete */
        while((RCC_CFGR & (3U << RCC_CFGR_SWS_BIT0)) != (0U << RCC_CFGR_SWS_BIT0));

    #elif RCC_SYSTEM_CLOCK_SOURCE == RCC_CLOCK_SOURCE_HSE

        #if RCC_HSE_SOURCE_TYPE == RCC_HSE_SOURCE_CRYSTAL
            /* HSE Crystal Clock Source Configuration */
            /* 1- Enable HSE */
            RCC_CR |= (1U << RCC_CR_HSEON_BIT);

            /* 2- Disable HSE bypass */
            RCC_CR &= ~(1U << RCC_CR_HSEBYP_BIT);

            /* 3- Wait for HSE to be ready */
            while(!(RCC_CR & (1U << RCC_CR_HSERDY_BIT)));

            /* 4- Set System Clock Source to HSE */
            RCC_CFGR |= (1U << RCC_CFGR_SW_BIT0);
            RCC_CFGR &= ~(1U << RCC_CFGR_SW_BIT1);

            /* 5- Wait for system clock switch to complete */
            while((RCC_CFGR & (3U << RCC_CFGR_SWS_BIT0)) != (1U << RCC_CFGR_SWS_BIT0));

        #elif RCC_HSE_SOURCE_TYPE == RCC_HSE_SOURCE_RC
            /* HSE RC Clock Source Configuration */
            /* 1- Enable HSE */
            RCC_CR |= (1U << RCC_CR_HSEON_BIT);

            /* 2- Enable HSE bypass */
            RCC_CR |= (1U << RCC_CR_HSEBYP_BIT);

            /* 3- Wait for HSE to be ready */
            while(!(RCC_CR & (1U << RCC_CR_HSERDY_BIT)));

            /* 4- Set System Clock Source to HSE */
            RCC_CFGR |= (1U << RCC_CFGR_SW_BIT0);
            RCC_CFGR &= ~(1U << RCC_CFGR_SW_BIT1);

            /* 5- Wait for system clock switch to complete */
            while((RCC_CFGR & (3U << RCC_CFGR_SWS_BIT0)) != (1U << RCC_CFGR_SWS_BIT0));

        #else
            #error "Invalid HSE source configuration"
        #endif

    #elif RCC_SYSTEM_CLOCK_SOURCE == RCC_CLOCK_SOURCE_PLL
        /* PLL configuration to be implemented */
        #error "PLL configuration not implemented yet"

    #else
        #error "Invalid option for clock source"
    #endif
}
