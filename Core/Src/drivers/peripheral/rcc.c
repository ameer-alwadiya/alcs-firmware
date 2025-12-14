/***********************************************************************/
/*Author           : Ameer Alwadiya                                    */
/*Version          : V1.1.0                                            */
/*Date             : 12 Nov 2025                                       */
/*Description      : rcc.c --> Function Implementation                 */
/*Module Features  :                                                   */
/*        01- RCC_EnablePeripheralClock                                */
/*        02- RCC_DisablePeripheralClock                               */
/*        03- RCC_InitializeSystemClock                                */
/*        04- RCC_EnableLSI (for IWDG)                                 */
/*        05- RCC_GetResetSource                                       */
/*        06- RCC_ClearResetFlags                                      */
/***********************************************************************/

/***********************************************************************/
/*                            Own Header Files                         */
/***********************************************************************/
#include "rcc.h"

/***********************************************************************/
/*                           Private Macros                            */
/***********************************************************************/
#define RCC_LSI_TIMEOUT_MS              100U   /* 100ms timeout for LSI ready */

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

/**
 * @brief   Enable LSI clock (for IWDG)
 * @param   None
 * @retval  RCC_Status_t Status of operation
 */
RCC_Status_t RCC_EnableLSI(void)
{
    uint32_t timeout = RCC_LSI_TIMEOUT_MS * 10000U;  /* Approximate loop count */

    /* Enable LSI oscillator */
    RCC_CSR |= (1U << RCC_CSR_LSION_BIT);

    /* Wait for LSI to be ready */
    while (!(RCC_CSR & (1U << RCC_CSR_LSIRDY_BIT)) && (timeout > 0))
    {
        timeout--;
    }

    if (timeout == 0)
    {
        return RCC_STATUS_TIMEOUT;
    }

    return RCC_STATUS_OK;
}

/**
 * @brief   Check if LSI is ready
 * @param   None
 * @retval  uint8_t 1 = ready, 0 = not ready
 */
uint8_t RCC_IsLSIReady(void)
{
    return (RCC_CSR & (1U << RCC_CSR_LSIRDY_BIT)) ? 1U : 0U;
}

/**
 * @brief   Get reset source from last reset
 * @param   None
 * @retval  RCC_ResetSource_t Reset source
 */
RCC_ResetSource_t RCC_GetResetSource(void)
{
    /* Check reset flags in priority order */
    if (RCC_CSR & (1U << RCC_CSR_LPWRRSTF_BIT))
    {
        return RCC_RESET_LOWPOWER;
    }
    else if (RCC_CSR & (1U << RCC_CSR_WWDGRSTF_BIT))
    {
        return RCC_RESET_WWDG;
    }
    else if (RCC_CSR & (1U << RCC_CSR_IWDGRSTF_BIT))
    {
        return RCC_RESET_IWDG;
    }
    else if (RCC_CSR & (1U << RCC_CSR_SFTRSTF_BIT))
    {
        return RCC_RESET_SOFTWARE;
    }
    else if (RCC_CSR & (1U << RCC_CSR_PORRSTF_BIT))
    {
        return RCC_RESET_POR;
    }
    else if (RCC_CSR & (1U << RCC_CSR_PINRSTF_BIT))
    {
        return RCC_RESET_PIN;
    }
    else if (RCC_CSR & (1U << RCC_CSR_BORRSTF_BIT))
    {
        return RCC_RESET_BOR;
    }

    return RCC_RESET_UNKNOWN;
}

/**
 * @brief   Check if reset was caused by IWDG
 * @param   None
 * @retval  uint8_t 1 = IWDG reset, 0 = other reset
 */
uint8_t RCC_WasResetByIWDG(void)
{
    return (RCC_CSR & (1U << RCC_CSR_IWDGRSTF_BIT)) ? 1U : 0U;
}

/**
 * @brief   Clear all reset flags
 * @param   None
 * @retval  None
 */
void RCC_ClearResetFlags(void)
{
    /* Set RMVF bit to clear all reset flags */
    RCC_CSR |= (1U << RCC_CSR_RMVF_BIT);
}
