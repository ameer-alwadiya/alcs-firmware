/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 13 Aug 2025                                                                     */
/* Description       : adc_interface.h --> ADC Interface Header File                                  */
/*******************************************************************************************************/

#ifndef ADC_INTERFACE_H_
#define ADC_INTERFACE_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

/*******************************************************************************************************/
/* ADC Instance Definitions                                                                            */
/*******************************************************************************************************/

typedef enum
{
    ADC_INSTANCE_1 = 0
} ADC_Instance_t;

/*******************************************************************************************************/
/* ADC Channel Definitions                                                                             */
/*******************************************************************************************************/

typedef enum
{
    ADC_CHANNEL_0 = 0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
    ADC_CHANNEL_7,
    ADC_CHANNEL_8,
    ADC_CHANNEL_9,
    ADC_CHANNEL_10,
    ADC_CHANNEL_11,
    ADC_CHANNEL_12,
    ADC_CHANNEL_13,
    ADC_CHANNEL_14,
    ADC_CHANNEL_15,
    ADC_CHANNEL_TEMP,      /* Temperature sensor */
    ADC_CHANNEL_VREF       /* Internal reference voltage */
} ADC_Channel_t;

/*******************************************************************************************************/
/* ADC Conversion Mode                                                                                 */
/*******************************************************************************************************/

typedef enum
{
    ADC_MODE_SINGLE = 0,
    ADC_MODE_CONTINUOUS
} ADC_ConversionMode_t;

/*******************************************************************************************************/
/* ADC Trigger Source                                                                                  */
/*******************************************************************************************************/

typedef enum
{
    ADC_TRIGGER_SOFTWARE = 0,
    ADC_TRIGGER_TIM1_CC1,
    ADC_TRIGGER_TIM1_CC2,
    ADC_TRIGGER_TIM1_CC3,
    ADC_TRIGGER_TIM2_CC2,
    ADC_TRIGGER_TIM2_CC3,
    ADC_TRIGGER_TIM2_CC4,
    ADC_TRIGGER_TIM2_TRGO,
    ADC_TRIGGER_TIM3_CC1,
    ADC_TRIGGER_TIM3_TRGO,
    ADC_TRIGGER_TIM4_CC4,
    ADC_TRIGGER_TIM5_CC1,
    ADC_TRIGGER_TIM5_CC2,
    ADC_TRIGGER_TIM5_CC3,
    ADC_TRIGGER_EXTI_LINE11
} ADC_TriggerSource_t;

/*******************************************************************************************************/
/* Function Return Status                                                                              */
/*******************************************************************************************************/

typedef enum
{
    ADC_STATUS_OK = 0,
    ADC_STATUS_ERROR,
    ADC_STATUS_INVALID_INSTANCE,
    ADC_STATUS_INVALID_CHANNEL,
    ADC_STATUS_TIMEOUT,
    ADC_STATUS_CONVERSION_IN_PROGRESS
} ADC_Status_t;

/*******************************************************************************************************/
/* Function Prototypes                                                                                 */
/*******************************************************************************************************/

ADC_Status_t ADC_Initialize(ADC_Instance_t Copy_Instance);
ADC_Status_t ADC_Enable(ADC_Instance_t Copy_Instance);
ADC_Status_t ADC_Disable(ADC_Instance_t Copy_Instance);
ADC_Status_t ADC_StartConversion(ADC_Instance_t Copy_Instance, ADC_Channel_t Copy_Channel);
ADC_Status_t ADC_StartContinuousConversion(ADC_Instance_t Copy_Instance, ADC_Channel_t Copy_Channel);
ADC_Status_t ADC_StopConversion(ADC_Instance_t Copy_Instance);
ADC_Status_t ADC_ReadValue(ADC_Instance_t Copy_Instance, uint16_t* Copy_u16Value);
ADC_Status_t ADC_ReadValueBlocking(ADC_Instance_t Copy_Instance, ADC_Channel_t Copy_Channel, uint16_t* Copy_u16Value);
ADC_Status_t ADC_SetCallback(ADC_Instance_t Copy_Instance, void(*Copy_Fptr)(void));
ADC_Status_t ADC_IsConversionComplete(ADC_Instance_t Copy_Instance);
ADC_Status_t ADC_Calibrate(ADC_Instance_t Copy_Instance);

/* Temperature and VRef functions */
float ADC_ConvertToVoltage(uint16_t Copy_u16AdcValue, uint16_t Copy_u16VRefValue);
float ADC_ConvertToTemperature(uint16_t Copy_u16TempValue, uint16_t Copy_u16VRefValue);

#endif /* ADC_INTERFACE_H_ */
