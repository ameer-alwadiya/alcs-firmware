/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 16 Jul 2025                                                                     */
/* Description       : gpio.c --> GPIO driver implementation                                           */
/* Module Features   :                                                                                 */
/*      01- GPIO_SetPinMode()                                                                          */
/*      02- GPIO_SetOutputType()                                                                       */
/*      03- GPIO_SetOutputSpeed()                                                                      */
/*      04- GPIO_SetPullState()                                                                        */
/*      05- GPIO_GetPinValue()                                                                         */
/*      06- GPIO_SetPinValue()                                                                         */
/*      07- GPIO_DirectSetReset()                                                                      */
/*      08- GPIO_PinLock()                                                                             */
/*      09- GPIO_SetPinAltFunction()                                                                   */
/*******************************************************************************************************/

/*******************************************************************************************************/
/*                                      Include Files                                                  */
/*******************************************************************************************************/

#include <stddef.h>
#include "gpio.h"
#include "common/bit_math.h"

/*******************************************************************************************************/
/*                                      Function Implementations                                       */
/*******************************************************************************************************/

/*******************************************************************************************************/
/*                                      01- GPIO_SetPinMode                                            */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Sets the mode of a GPIO pin (Input/Output/Alternate/Analog)                */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         mode: Pin mode (GPIO_MODE_INPUT/OUTPUT/ALTF/ANALOG)                        */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_SetPinMode(uint8_t port_name, uint8_t pin_num, uint8_t mode)
{
    /* Clear the two bits for the pin first */
    uint32_t clear_mask = ~(0x3UL << (2U * pin_num));

    switch(port_name)
    {
        case GPIO_PORT_A:
            GPIOA->MODER &= clear_mask;
            GPIOA->MODER |= (uint32_t)(mode << (2U * pin_num));
            break;

        case GPIO_PORT_B:
            GPIOB->MODER &= clear_mask;
            GPIOB->MODER |= (uint32_t)(mode << (2U * pin_num));
            break;

        case GPIO_PORT_C:
            GPIOC->MODER &= clear_mask;
            GPIOC->MODER |= (uint32_t)(mode << (2U * pin_num));
            break;

        case GPIO_PORT_D:
            GPIOD->MODER &= clear_mask;
            GPIOD->MODER |= (uint32_t)(mode << (2U * pin_num));
            break;

        case GPIO_PORT_E:
            GPIOE->MODER &= clear_mask;
            GPIOE->MODER |= (uint32_t)(mode << (2U * pin_num));
            break;

        case GPIO_PORT_H:
            GPIOH->MODER &= clear_mask;
            GPIOH->MODER |= (uint32_t)(mode << (2U * pin_num));
            break;

        default:
            /* Invalid port - do nothing */
            break;
    }
}

/*******************************************************************************************************/
/*                                      02- GPIO_SetOutputType                                         */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Sets the output type of a GPIO pin (Push-Pull/Open-Drain)                  */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         output_type: Output type (GPIO_OUTPUT_TYPE_PUSH_PULL/OPEN_DRAIN)           */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_SetOutputType(uint8_t port_name, uint8_t pin_num, uint8_t output_type)
{
    switch(port_name)
    {
        case GPIO_PORT_A:
            if(output_type == GPIO_OUTPUT_TYPE_OPEN_DRAIN)
                SET_BIT(GPIOA->OTYPER, pin_num);
            else
            	CLR_BIT(GPIOA->OTYPER, pin_num);
            break;

        case GPIO_PORT_B:
            if(output_type == GPIO_OUTPUT_TYPE_OPEN_DRAIN)
                SET_BIT(GPIOB->OTYPER, pin_num);
            else
            	CLR_BIT(GPIOB->OTYPER, pin_num);
            break;

        case GPIO_PORT_C:
            if(output_type == GPIO_OUTPUT_TYPE_OPEN_DRAIN)
                SET_BIT(GPIOC->OTYPER, pin_num);
            else
            	CLR_BIT(GPIOC->OTYPER, pin_num);
            break;

        case GPIO_PORT_D:
            if(output_type == GPIO_OUTPUT_TYPE_OPEN_DRAIN)
                SET_BIT(GPIOD->OTYPER, pin_num);
            else
            	CLR_BIT(GPIOD->OTYPER, pin_num);
            break;

        case GPIO_PORT_E:
            if(output_type == GPIO_OUTPUT_TYPE_OPEN_DRAIN)
                SET_BIT(GPIOE->OTYPER, pin_num);
            else
            	CLR_BIT(GPIOE->OTYPER, pin_num);
            break;

        case GPIO_PORT_H:
            if(output_type == GPIO_OUTPUT_TYPE_OPEN_DRAIN)
                SET_BIT(GPIOH->OTYPER, pin_num);
            else
            	CLR_BIT(GPIOH->OTYPER, pin_num);
            break;

        default:
            /* Invalid port - do nothing */
            break;
    }
}

/*******************************************************************************************************/
/*                                      03- GPIO_SetOutputSpeed                                        */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Sets the output speed of a GPIO pin                                         */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         speed_mode: Speed setting (GPIO_OUTPUT_SPEED_LOW/MEDIUM/HIGH/VERY_HIGH)    */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_SetOutputSpeed(uint8_t port_name, uint8_t pin_num, uint8_t speed_mode)
{
    /* Clear the two bits for the pin first */
    uint32_t clear_mask = ~(0x3UL << (2U * pin_num));

    switch(port_name)
    {
        case GPIO_PORT_A:
            GPIOA->OSPEEDR &= clear_mask;
            GPIOA->OSPEEDR |= (uint32_t)(speed_mode << (2U * pin_num));
            break;

        case GPIO_PORT_B:
            GPIOB->OSPEEDR &= clear_mask;
            GPIOB->OSPEEDR |= (uint32_t)(speed_mode << (2U * pin_num));
            break;

        case GPIO_PORT_C:
            GPIOC->OSPEEDR &= clear_mask;
            GPIOC->OSPEEDR |= (uint32_t)(speed_mode << (2U * pin_num));
            break;

        case GPIO_PORT_D:
            GPIOD->OSPEEDR &= clear_mask;
            GPIOD->OSPEEDR |= (uint32_t)(speed_mode << (2U * pin_num));
            break;

        case GPIO_PORT_E:
            GPIOE->OSPEEDR &= clear_mask;
            GPIOE->OSPEEDR |= (uint32_t)(speed_mode << (2U * pin_num));
            break;

        case GPIO_PORT_H:
            GPIOH->OSPEEDR &= clear_mask;
            GPIOH->OSPEEDR |= (uint32_t)(speed_mode << (2U * pin_num));
            break;

        default:
            /* Invalid port - do nothing */
            break;
    }
}

/*******************************************************************************************************/
/*                                      04- GPIO_SetPullState                                          */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Sets the pull-up/pull-down resistor state of a GPIO pin                    */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         pull_type: Pull resistor type (GPIO_PULL_FLOATING/UP/DOWN)                 */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_SetPullState(uint8_t port_name, uint8_t pin_num, uint8_t pull_type)
{
    /* Clear the two bits for the pin first */
    uint32_t clear_mask = ~(0x3UL << (2U * pin_num));

    switch(port_name)
    {
        case GPIO_PORT_A:
            GPIOA->PUPDR &= clear_mask;
            GPIOA->PUPDR |= (uint32_t)(pull_type << (2U * pin_num));
            break;

        case GPIO_PORT_B:
            GPIOB->PUPDR &= clear_mask;
            GPIOB->PUPDR |= (uint32_t)(pull_type << (2U * pin_num));
            break;

        case GPIO_PORT_C:
            GPIOC->PUPDR &= clear_mask;
            GPIOC->PUPDR |= (uint32_t)(pull_type << (2U * pin_num));
            break;

        case GPIO_PORT_D:
            GPIOD->PUPDR &= clear_mask;
            GPIOD->PUPDR |= (uint32_t)(pull_type << (2U * pin_num));
            break;

        case GPIO_PORT_E:
            GPIOE->PUPDR &= clear_mask;
            GPIOE->PUPDR |= (uint32_t)(pull_type << (2U * pin_num));
            break;

        case GPIO_PORT_H:
            GPIOH->PUPDR &= clear_mask;
            GPIOH->PUPDR |= (uint32_t)(pull_type << (2U * pin_num));
            break;

        default:
            /* Invalid port - do nothing */
            break;
    }
}

/*******************************************************************************************************/
/*                                      05- GPIO_GetPinValue                                           */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Reads the value of a GPIO pin from Input Data Register                     */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/* Function Return      -> Pin state value (GPIO_PIN_HIGH/GPIO_PIN_LOW)                               */
/*******************************************************************************************************/

uint8_t GPIO_GetPinValue(uint8_t port_name, uint8_t pin_num)
{
    uint8_t pin_value = 0;

    switch(port_name)
    {
        case GPIO_PORT_A:
            pin_value = GET_BIT(GPIOA->IDR, pin_num);
            break;

        case GPIO_PORT_B:
            pin_value = GET_BIT(GPIOB->IDR, pin_num);
            break;

        case GPIO_PORT_C:
            pin_value = GET_BIT(GPIOC->IDR, pin_num);
            break;

        case GPIO_PORT_D:
            pin_value = GET_BIT(GPIOD->IDR, pin_num);
            break;

        case GPIO_PORT_E:
            pin_value = GET_BIT(GPIOE->IDR, pin_num);
            break;

        case GPIO_PORT_H:
            pin_value = GET_BIT(GPIOH->IDR, pin_num);
            break;

        default:
            /* Invalid port - return 0 */
            pin_value = 0;
            break;
    }

    return pin_value;
}

/*******************************************************************************************************/
/*                                      06- GPIO_SetPinValue                                           */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Sets the value of a GPIO pin in Output Data Register                       */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         pin_value: Pin value (GPIO_PIN_HIGH/GPIO_PIN_LOW)                          */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_SetPinValue(uint8_t port_name, uint8_t pin_num, uint8_t pin_value)
{
    if(pin_value == GPIO_PIN_HIGH)
    {
        switch(port_name)
        {
            case GPIO_PORT_A:
                SET_BIT(GPIOA->ODR, pin_num);
                break;

            case GPIO_PORT_B:
                SET_BIT(GPIOB->ODR, pin_num);
                break;

            case GPIO_PORT_C:
                SET_BIT(GPIOC->ODR, pin_num);
                break;

            case GPIO_PORT_D:
                SET_BIT(GPIOD->ODR, pin_num);
                break;

            case GPIO_PORT_E:
                SET_BIT(GPIOE->ODR, pin_num);
                break;

            case GPIO_PORT_H:
                SET_BIT(GPIOH->ODR, pin_num);
                break;

            default:
                /* Invalid port - do nothing */
                break;
        }
    }
    else
    {
        switch(port_name)
        {
            case GPIO_PORT_A:
                CLR_BIT(GPIOA->ODR, pin_num);
                break;

            case GPIO_PORT_B:
            	CLR_BIT(GPIOB->ODR, pin_num);
                break;

            case GPIO_PORT_C:
            	CLR_BIT(GPIOC->ODR, pin_num);
                break;

            case GPIO_PORT_D:
            	CLR_BIT(GPIOD->ODR, pin_num);
                break;

            case GPIO_PORT_E:
            	CLR_BIT(GPIOE->ODR, pin_num);
                break;

            case GPIO_PORT_H:
            	CLR_BIT(GPIOH->ODR, pin_num);
                break;

            default:
                /* Invalid port - do nothing */
                break;
        }
    }
}

/*******************************************************************************************************/
/*                                      07- GPIO_DirectSetReset                                        */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Directly sets or resets a GPIO pin using Bit Set/Reset Register            */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         pin_value: Pin value (GPIO_PIN_HIGH/GPIO_PIN_LOW)                          */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_DirectSetReset(uint8_t port_name, uint8_t pin_num, uint8_t pin_value)
{
    uint8_t bit_position = pin_num;

    /* For reset operation, use upper 16 bits of BSRR register */
    if(pin_value == GPIO_PIN_LOW)
    {
        bit_position += 16;
    }

    switch(port_name)
    {
        case GPIO_PORT_A:
            GPIOA->BSRR = (1UL << bit_position);
            break;

        case GPIO_PORT_B:
            GPIOB->BSRR = (1UL << bit_position);
            break;

        case GPIO_PORT_C:
            GPIOC->BSRR = (1UL << bit_position);
            break;

        case GPIO_PORT_D:
            GPIOD->BSRR = (1UL << bit_position);
            break;

        case GPIO_PORT_E:
            GPIOE->BSRR = (1UL << bit_position);
            break;

        case GPIO_PORT_H:
            GPIOH->BSRR = (1UL << bit_position);
            break;

        default:
            /* Invalid port - do nothing */
            break;
    }
}

/*******************************************************************************************************/
/*                                      08- GPIO_PinLock                                               */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Locks the configuration of a GPIO pin until next system reset              */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_PinLock(uint8_t port_name, uint8_t pin_num)
{
    volatile uint32_t *lckr_reg = NULL;
    uint32_t temp_reg;

    /* Get pointer to the appropriate LCKR register */
    switch(port_name)
    {
        case GPIO_PORT_A:
            lckr_reg = &(GPIOA->LCKR);
            break;

        case GPIO_PORT_B:
            lckr_reg = &(GPIOB->LCKR);
            break;

        case GPIO_PORT_C:
            lckr_reg = &(GPIOC->LCKR);
            break;

        case GPIO_PORT_D:
            lckr_reg = &(GPIOD->LCKR);
            break;

        case GPIO_PORT_E:
            lckr_reg = &(GPIOE->LCKR);
            break;

        case GPIO_PORT_H:
            lckr_reg = &(GPIOH->LCKR);
            break;

        default:
            /* Invalid port - return without doing anything */
            return;
    }

    /* Lock sequence as per STM32 reference manual */
    /* 1. Set the pin bit and LCKK bit */
    temp_reg = (1UL << LOCK_PIN_BIT) | (1UL << pin_num);
    *lckr_reg = temp_reg;

    /* 2. Clear LCKK bit */
    temp_reg = (1UL << pin_num);
    *lckr_reg = temp_reg;

    /* 3. Set LCKK bit again */
    temp_reg = (1UL << LOCK_PIN_BIT) | (1UL << pin_num);
    *lckr_reg = temp_reg;

    /* 4. Read to confirm lock is active */
    temp_reg = *lckr_reg;
    (void)temp_reg; /* Suppress unused variable warning */
}

/*******************************************************************************************************/
/*                                      09- GPIO_SetPinAltFunction                                     */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Sets the alternate function of a GPIO pin                                   */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         alt_function: Alternate function (GPIO_ALTFN_0 to GPIO_ALTFN_15)           */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_SetPinAltFunction(uint8_t port_name, uint8_t pin_num, uint8_t alt_function)
{
    volatile uint32_t *afr_reg;
    uint8_t reg_offset;
    uint32_t clear_mask;

    /* Determine which AFR register to use and calculate bit position */
    if(pin_num < 8)
    {
        /* Use AFRL register for pins 0-7 */
        reg_offset = pin_num * 4;
    }
    else
    {
        /* Use AFRH register for pins 8-15 */
        reg_offset = (pin_num - 8) * 4;
    }

    clear_mask = ~(0xFUL << reg_offset);

    switch(port_name)
    {
        case GPIO_PORT_A:
            afr_reg = (pin_num < 8) ? &(GPIOA->AFRL) : &(GPIOA->AFRH);
            *afr_reg &= clear_mask;
            *afr_reg |= (uint32_t)(alt_function << reg_offset);
            break;

        case GPIO_PORT_B:
            afr_reg = (pin_num < 8) ? &(GPIOB->AFRL) : &(GPIOB->AFRH);
            *afr_reg &= clear_mask;
            *afr_reg |= (uint32_t)(alt_function << reg_offset);
            break;

        case GPIO_PORT_C:
            afr_reg = (pin_num < 8) ? &(GPIOC->AFRL) : &(GPIOC->AFRH);
            *afr_reg &= clear_mask;
            *afr_reg |= (uint32_t)(alt_function << reg_offset);
            break;

        case GPIO_PORT_D:
            afr_reg = (pin_num < 8) ? &(GPIOD->AFRL) : &(GPIOD->AFRH);
            *afr_reg &= clear_mask;
            *afr_reg |= (uint32_t)(alt_function << reg_offset);
            break;

        case GPIO_PORT_E:
            afr_reg = (pin_num < 8) ? &(GPIOE->AFRL) : &(GPIOE->AFRH);
            *afr_reg &= clear_mask;
            *afr_reg |= (uint32_t)(alt_function << reg_offset);
            break;

        case GPIO_PORT_H:
            afr_reg = (pin_num < 8) ? &(GPIOH->AFRL) : &(GPIOH->AFRH);
            *afr_reg &= clear_mask;
            *afr_reg |= (uint32_t)(alt_function << reg_offset);
            break;

        default:
            /* Invalid port - do nothing */
            break;
    }
}
