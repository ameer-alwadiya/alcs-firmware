/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 13 Aug 2025                                                                     */
/* Description       : adc_program.c --> ADC Implementation File                                       */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include "adc_private.h"
#include "adc_config.h"
#include "adc_interface.h"
#include <stdio.h>
#include <stdint.h>

/*******************************************************************************************************/
/* Global Variables                                                                                    */
/*******************************************************************************************************/

static void(*ADC1_CallbackPtr)(void) = NULL;

/*******************************************************************************************************/
/* Helper Functions                                                                                    */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* ADC_GetRegisterPointer                                                                              */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Get ADC register pointer for given instance                                */
/* Function Input       -> ADC Instance                                                                */
/* Function Return      -> Register pointer or NULL if invalid                                         */
/*******************************************************************************************************/

static volatile ADC_Registers_t* ADC_GetRegisterPointer(ADC_Instance_t Copy_Instance)
{
    switch (Copy_Instance)
    {
        case ADC_INSTANCE_1:
            return ADC1_REG;
        default:
            return NULL;
    }
}

/*******************************************************************************************************/
/* ADC_SetSampleTime                                                                                   */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Set sample time for specified channel                                       */
/* Function Input       -> ADC Instance, Channel, Sample time                                          */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

static ADC_Status_t ADC_SetSampleTime(ADC_Instance_t Copy_Instance, ADC_Channel_t Copy_Channel, uint8_t Copy_u8SampleTime)
{
    volatile ADC_Registers_t* ADC_Reg;

    ADC_Reg = ADC_GetRegisterPointer(Copy_Instance);
    if (ADC_Reg == NULL)
    {
        return ADC_STATUS_INVALID_INSTANCE;
    }

    if (Copy_Channel > ADC_CHANNEL_VREF)
    {
        return ADC_STATUS_INVALID_CHANNEL;
    }

    /* Set sample time based on channel number */
    if (Copy_Channel <= 9)
    {
        /* Channels 0-9 use SMPR2 register */
        uint8_t bitPosition = Copy_Channel * 3;
        ADC_Reg->SMPR2 &= ~(7U << bitPosition);
        ADC_Reg->SMPR2 |= (Copy_u8SampleTime << bitPosition);
    }
    else
    {
        /* Channels 10-18 use SMPR1 register */
        uint8_t bitPosition = (Copy_Channel - 10) * 3;
        ADC_Reg->SMPR1 &= ~(7U << bitPosition);
        ADC_Reg->SMPR1 |= (Copy_u8SampleTime << bitPosition);
    }

    return ADC_STATUS_OK;
}

/*******************************************************************************************************/
/* Function Implementations                                                                            */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* ADC_Initialize                                                                                      */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Initialize ADC peripheral                                                   */
/* Function Input       -> ADC Instance                                                                */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ADC_Status_t ADC_Initialize(ADC_Instance_t Copy_Instance)
{
    volatile ADC_Registers_t* ADC_Reg;

    /* Get register pointer */
    ADC_Reg = ADC_GetRegisterPointer(Copy_Instance);
    if (ADC_Reg == NULL)
    {
        return ADC_STATUS_INVALID_INSTANCE;
    }

    /* Disable ADC first */
    ADC_Reg->CR2 &= ~(1U << ADC_CR2_ADON_BIT);

    /* Configure ADC Common Control Register (CCR) */
    /* Set prescaler to divide by 4 (PCLK2/4) */
    ADC_COMMON_REG->CCR &= ~(3U << ADC_CCR_ADCPRE_BIT);
    ADC_COMMON_REG->CCR |= (ADC1_PRESCALER << ADC_CCR_ADCPRE_BIT);

    /* Enable temperature sensor and VREFINT if configured */
#if (ADC_TEMPERATURE_SENSOR_ENABLE == 1) || (ADC_VREF_ENABLE == 1)
    ADC_COMMON_REG->CCR |= (1U << ADC_CCR_TSVREFE_BIT);
#endif

    /* Configure CR1 register */
    ADC_Reg->CR1 = 0; /* Reset CR1 register */

    /* Set resolution */
    ADC_Reg->CR1 |= (ADC1_RESOLUTION << ADC_CR1_RES_BIT);

    /* Enable end of conversion interrupt */
    ADC_Reg->CR1 |= (1U << ADC_CR1_EOCIE_BIT);

    /* Configure CR2 register */
    ADC_Reg->CR2 = 0; /* Reset CR2 register */

    /* Set data alignment */
#if (ADC1_DATA_ALIGNMENT == ADC_DATA_ALIGNMENT_LEFT)
    ADC_Reg->CR2 |= (1U << ADC_CR2_ALIGN_BIT);
#endif

    /* Configure trigger source and edge for software trigger */
    ADC_Reg->CR2 &= ~(15U << ADC_CR2_EXTSEL_BIT); /* Clear external trigger selection */
    ADC_Reg->CR2 &= ~(3U << ADC_CR2_EXTEN_BIT);   /* Disable external trigger */

    /* Set end of conversion flag for each conversion */
    ADC_Reg->CR2 |= (1U << ADC_CR2_EOCS_BIT);

    /* Set default sample time for all channels */
    for (uint8_t channel = 0; channel <= 18; channel++)
    {
        ADC_SetSampleTime(Copy_Instance, channel, ADC1_SAMPLE_TIME);
    }

    /* Initialize sequence registers */
    ADC_Reg->SQR1 = 0; /* 1 conversion */
    ADC_Reg->SQR2 = 0;
    ADC_Reg->SQR3 = 0;

    /* Clear any pending flags */
    ADC_Reg->SR = 0;

    return ADC_STATUS_OK;
}

/*******************************************************************************************************/
/* ADC_Enable                                                                                          */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Enable ADC peripheral                                                       */
/* Function Input       -> ADC Instance                                                                */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ADC_Status_t ADC_Enable(ADC_Instance_t Copy_Instance)
{
    volatile ADC_Registers_t* ADC_Reg;

    /* Get register pointer */
    ADC_Reg = ADC_GetRegisterPointer(Copy_Instance);
    if (ADC_Reg == NULL)
    {
        return ADC_STATUS_INVALID_INSTANCE;
    }

    /* Enable ADC */
    ADC_Reg->CR2 |= (1U << ADC_CR2_ADON_BIT);

    return ADC_STATUS_OK;
}

/*******************************************************************************************************/
/* ADC_Disable                                                                                         */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Disable ADC peripheral                                                      */
/* Function Input       -> ADC Instance                                                                */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ADC_Status_t ADC_Disable(ADC_Instance_t Copy_Instance)
{
    volatile ADC_Registers_t* ADC_Reg;

    /* Get register pointer */
    ADC_Reg = ADC_GetRegisterPointer(Copy_Instance);
    if (ADC_Reg == NULL)
    {
        return ADC_STATUS_INVALID_INSTANCE;
    }

    /* Disable ADC */
    ADC_Reg->CR2 &= ~(1U << ADC_CR2_ADON_BIT);

    return ADC_STATUS_OK;
}

/*******************************************************************************************************/
/* ADC_StartConversion                                                                                 */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Start single ADC conversion on specified channel                            */
/* Function Input       -> ADC Instance, Channel                                                       */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ADC_Status_t ADC_StartConversion(ADC_Instance_t Copy_Instance, ADC_Channel_t Copy_Channel)
{
    volatile ADC_Registers_t* ADC_Reg;
    uint8_t actualChannel;

    /* Get register pointer */
    ADC_Reg = ADC_GetRegisterPointer(Copy_Instance);
    if (ADC_Reg == NULL)
    {
        return ADC_STATUS_INVALID_INSTANCE;
    }

    /* Validate channel */
    if (Copy_Channel > ADC_CHANNEL_VREF)
    {
        return ADC_STATUS_INVALID_CHANNEL;
    }

    /* Map special channels to actual channel numbers */
    if (Copy_Channel == ADC_CHANNEL_TEMP)
    {
        actualChannel = 16; /* Temperature sensor is on channel 16 */
    }
    else if (Copy_Channel == ADC_CHANNEL_VREF)
    {
        actualChannel = 17; /* VREFINT is on channel 17 */
    }
    else
    {
        actualChannel = Copy_Channel;
    }

    /* Configure sequence register for single conversion */
    ADC_Reg->SQR1 = 0; /* Length = 1 conversion (L[3:0] = 0000) */
    ADC_Reg->SQR3 = actualChannel; /* First conversion channel */

    /* Clear all status flags */
    ADC_Reg->SR = 0;

    /* Start conversion */
    ADC_Reg->CR2 |= (1U << ADC_CR2_SWSTART_BIT);

    return ADC_STATUS_OK;
}

/*******************************************************************************************************/
/* ADC_StartContinuousConversion                                                                       */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Start continuous ADC conversion on specified channel                        */
/* Function Input       -> ADC Instance, Channel                                                       */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ADC_Status_t ADC_StartContinuousConversion(ADC_Instance_t Copy_Instance, ADC_Channel_t Copy_Channel)
{
    volatile ADC_Registers_t* ADC_Reg;
    uint8_t actualChannel;

    /* Get register pointer */
    ADC_Reg = ADC_GetRegisterPointer(Copy_Instance);
    if (ADC_Reg == NULL)
    {
        return ADC_STATUS_INVALID_INSTANCE;
    }

    /* Validate channel */
    if (Copy_Channel > ADC_CHANNEL_VREF)
    {
        return ADC_STATUS_INVALID_CHANNEL;
    }

    /* Map special channels to actual channel numbers */
    if (Copy_Channel == ADC_CHANNEL_TEMP)
    {
        actualChannel = 16; /* Temperature sensor is on channel 16 */
    }
    else if (Copy_Channel == ADC_CHANNEL_VREF)
    {
        actualChannel = 17; /* VREFINT is on channel 17 */
    }
    else
    {
        actualChannel = Copy_Channel;
    }

    /* Enable continuous conversion mode */
    ADC_Reg->CR2 |= (1U << ADC_CR2_CONT_BIT);

    /* Configure sequence register for single conversion */
    ADC_Reg->SQR1 = 0; /* Length = 1 conversion (L[3:0] = 0000) */
    ADC_Reg->SQR3 = actualChannel; /* First conversion channel */

    /* Clear EOC flag */
    ADC_Reg->SR &= ~(1U << ADC_SR_EOC_BIT);

    /* Start conversion */
    ADC_Reg->CR2 |= (1U << ADC_CR2_SWSTART_BIT);

    return ADC_STATUS_OK;
}

/*******************************************************************************************************/
/* ADC_StopConversion                                                                                  */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Stop ADC conversion                                                         */
/* Function Input       -> ADC Instance                                                                */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ADC_Status_t ADC_StopConversion(ADC_Instance_t Copy_Instance)
{
    volatile ADC_Registers_t* ADC_Reg;

    /* Get register pointer */
    ADC_Reg = ADC_GetRegisterPointer(Copy_Instance);
    if (ADC_Reg == NULL)
    {
        return ADC_STATUS_INVALID_INSTANCE;
    }

    /* Disable continuous conversion mode */
    ADC_Reg->CR2 &= ~(1U << ADC_CR2_CONT_BIT);

    return ADC_STATUS_OK;
}

/*******************************************************************************************************/
/* ADC_ReadValue                                                                                       */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Read ADC conversion result (non-blocking)                                   */
/* Function Input       -> ADC Instance, Pointer to store result                                       */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ADC_Status_t ADC_ReadValue(ADC_Instance_t Copy_Instance, uint16_t* Copy_u16Value)
{
    volatile ADC_Registers_t* ADC_Reg;

    /* Validate input */
    if (Copy_u16Value == NULL)
    {
        return ADC_STATUS_ERROR;
    }

    /* Get register pointer */
    ADC_Reg = ADC_GetRegisterPointer(Copy_Instance);
    if (ADC_Reg == NULL)
    {
        return ADC_STATUS_INVALID_INSTANCE;
    }

    /* Check if conversion is complete */
    if (((ADC_Reg->SR >> ADC_SR_EOC_BIT) & 1) == 0)
    {
        return ADC_STATUS_CONVERSION_IN_PROGRESS;
    }

    /* Read conversion result (this automatically clears EOC flag) */
    *Copy_u16Value = (uint16_t)(ADC_Reg->DR & 0xFFF);

    return ADC_STATUS_OK;
}

/*******************************************************************************************************/
/* ADC_ReadValueBlocking                                                                               */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Read ADC conversion result (blocking with timeout)                          */
/* Function Input       -> ADC Instance, Channel, Pointer to store result                             */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ADC_Status_t ADC_ReadValueBlocking(ADC_Instance_t Copy_Instance, ADC_Channel_t Copy_Channel, uint16_t* Copy_u16Value)
{
    ADC_Status_t status;
    uint32_t timeout;
    volatile ADC_Registers_t* ADC_Reg;

    /* Validate input */
    if (Copy_u16Value == NULL)
    {
        return ADC_STATUS_ERROR;
    }

    /* Get register pointer */
    ADC_Reg = ADC_GetRegisterPointer(Copy_Instance);
    if (ADC_Reg == NULL)
    {
        return ADC_STATUS_INVALID_INSTANCE;
    }

    /* Make sure continuous mode is disabled for single conversion */
    ADC_Reg->CR2 &= ~(1U << ADC_CR2_CONT_BIT);

    /* Start conversion */
    status = ADC_StartConversion(Copy_Instance, Copy_Channel);
    if (status != ADC_STATUS_OK)
    {
        return status;
    }

    /* Wait for conversion to complete with timeout */
    timeout = ADC_TIMEOUT_VALUE;
    while (((ADC_Reg->SR >> ADC_SR_EOC_BIT) & 1) == 0)
    {
        if (--timeout == 0)
        {
            return ADC_STATUS_TIMEOUT;
        }
    }

    /* Read conversion result */
    *Copy_u16Value = (uint16_t)ADC_Reg->DR;

    return ADC_STATUS_OK;
}

/*******************************************************************************************************/
/* ADC_IsConversionComplete                                                                            */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Check if ADC conversion is complete                                         */
/* Function Input       -> ADC Instance                                                                */
/* Function Return      -> Status (OK if complete, CONVERSION_IN_PROGRESS if not)                     */
/*******************************************************************************************************/

ADC_Status_t ADC_IsConversionComplete(ADC_Instance_t Copy_Instance)
{
    volatile ADC_Registers_t* ADC_Reg;

    /* Get register pointer */
    ADC_Reg = ADC_GetRegisterPointer(Copy_Instance);
    if (ADC_Reg == NULL)
    {
        return ADC_STATUS_INVALID_INSTANCE;
    }

    /* Check EOC flag */
    if (((ADC_Reg->SR >> ADC_SR_EOC_BIT) & 1) == 1)
    {
        return ADC_STATUS_OK;
    }
    else
    {
        return ADC_STATUS_CONVERSION_IN_PROGRESS;
    }
}

/*******************************************************************************************************/
/* ADC_SetCallback                                                                                     */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Set callback function for ADC interrupt                                     */
/* Function Input       -> ADC Instance, Function pointer                                              */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ADC_Status_t ADC_SetCallback(ADC_Instance_t Copy_Instance, void(*Copy_Fptr)(void))
{
    if (Copy_Fptr == NULL)
    {
        return ADC_STATUS_ERROR;
    }

    switch (Copy_Instance)
    {
        case ADC_INSTANCE_1:
            ADC1_CallbackPtr = Copy_Fptr;
            break;
        default:
            return ADC_STATUS_INVALID_INSTANCE;
    }

    return ADC_STATUS_OK;
}

/*******************************************************************************************************/
/* ADC_Calibrate                                                                                       */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Calibrate ADC (not available on STM32F4, returns OK for compatibility)     */
/* Function Input       -> ADC Instance                                                                */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

ADC_Status_t ADC_Calibrate(ADC_Instance_t Copy_Instance)
{
    volatile ADC_Registers_t* ADC_Reg;

    /* Get register pointer */
    ADC_Reg = ADC_GetRegisterPointer(Copy_Instance);
    if (ADC_Reg == NULL)
    {
        return ADC_STATUS_INVALID_INSTANCE;
    }

    /* STM32F4 doesn't have ADC calibration like STM32F1 */
    /* This function is provided for API compatibility */
    /* ADC is automatically calibrated on power-up */

    return ADC_STATUS_OK;
}

/*******************************************************************************************************/
/* Temperature and Voltage Conversion Functions                                                        */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* ADC_ConvertToVoltage                                                                                */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Convert ADC value to voltage in millivolts                                 */
/* Function Input       -> ADC value, VRef ADC value                                                   */
/* Function Return      -> Voltage in millivolts                                                      */
/*******************************************************************************************************/

float ADC_ConvertToVoltage(uint16_t Copy_u16AdcValue, uint16_t Copy_u16VRefValue)
{
    float voltage;

    /* Calculate voltage using VREFINT */
    /* Voltage = (ADC_Value * VREFINT_Voltage) / (VRef_ADC_Value) */
    voltage = ((float)Copy_u16AdcValue * ADC_VREFINT_VOLTAGE) / (float)Copy_u16VRefValue;

    return voltage;
}

/*******************************************************************************************************/
/* ADC_ConvertToTemperature                                                                            */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Convert temperature sensor ADC value to temperature in Celsius             */
/* Function Input       -> Temperature ADC value, VRef ADC value                                       */
/* Function Return      -> Temperature in Celsius                                                     */
/*******************************************************************************************************/

float ADC_ConvertToTemperature(uint16_t Copy_u16TempValue, uint16_t Copy_u16VRefValue)
{
    float temperature;
    float tempVoltage;

    /* Convert ADC value to voltage in millivolts */
    tempVoltage = ADC_ConvertToVoltage(Copy_u16TempValue, Copy_u16VRefValue);

    /* Calculate temperature using the formula from datasheet */
    /* Temperature = (V25 - Vsense) / Avg_Slope + 25 */
    /* Where V25 = 1.43V (1430mV) and Avg_Slope = 4.3mV/°C (4300uV/°C) */
    temperature = ((float)ADC_TEMP_V25 - tempVoltage) / ((float)ADC_TEMP_AVG_SLOPE / 1000.0f) + 25.0f;

    return temperature;
}

/*******************************************************************************************************/
/* Interrupt Service Routines                                                                          */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* ADC_IRQHandler                                                                                      */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> ADC interrupt handler                                                       */
/* Function Input       -> None                                                                        */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void ADC_IRQHandler(void)
{
    /* Check if EOC interrupt occurred */
    if (((ADC1_REG->SR >> ADC_SR_EOC_BIT) & 1) == 1)
    {
        if (ADC1_CallbackPtr != NULL)
        {
            ADC1_CallbackPtr();
        }

        /* Note: EOC flag is automatically cleared when DR is read in the callback */
    }
}
