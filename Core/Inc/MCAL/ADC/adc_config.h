/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 13 Aug 2025                                                                     */
/* Description       : adc_config.h --> ADC Configuration Header File                                 */
/*******************************************************************************************************/

#ifndef ADC_CONFIG_H_
#define ADC_CONFIG_H_

/*******************************************************************************************************/
/* ADC Configuration Parameters                                                                        */
/*******************************************************************************************************/

/* ADC1 Configuration */
#define ADC1_RESOLUTION             ADC_RESOLUTION_12_BIT
#define ADC1_SAMPLE_TIME            ADC_SAMPLE_TIME_84_CYCLES
#define ADC1_DATA_ALIGNMENT         ADC_DATA_ALIGNMENT_RIGHT
#define ADC1_CONVERSION_MODE        ADC_CONVERSION_MODE_SINGLE
#define ADC1_TRIGGER_SOURCE         ADC_TRIGGER_SOURCE_SOFTWARE
#define ADC1_TRIGGER_EDGE           ADC_TRIGGER_EDGE_NONE
#define ADC1_PRESCALER              ADC_PRESCALER_DIV4

/*******************************************************************************************************/
/* ADC Data Alignment Options                                                                          */
/*******************************************************************************************************/

#define ADC_DATA_ALIGNMENT_RIGHT    0
#define ADC_DATA_ALIGNMENT_LEFT     1

/*******************************************************************************************************/
/* ADC Conversion Mode Options                                                                         */
/*******************************************************************************************************/

#define ADC_CONVERSION_MODE_SINGLE     0
#define ADC_CONVERSION_MODE_CONTINUOUS 1

/*******************************************************************************************************/
/* ADC Trigger Source Options                                                                          */
/*******************************************************************************************************/

#define ADC_TRIGGER_SOURCE_SOFTWARE    0
#define ADC_TRIGGER_SOURCE_TIM1_CC1    1
#define ADC_TRIGGER_SOURCE_TIM1_CC2    2
#define ADC_TRIGGER_SOURCE_TIM1_CC3    3
#define ADC_TRIGGER_SOURCE_TIM2_CC2    4
#define ADC_TRIGGER_SOURCE_TIM2_CC3    5
#define ADC_TRIGGER_SOURCE_TIM2_CC4    6
#define ADC_TRIGGER_SOURCE_TIM2_TRGO   7
#define ADC_TRIGGER_SOURCE_TIM3_CC1    8
#define ADC_TRIGGER_SOURCE_TIM3_TRGO   9
#define ADC_TRIGGER_SOURCE_TIM4_CC4    10
#define ADC_TRIGGER_SOURCE_TIM5_CC1    11
#define ADC_TRIGGER_SOURCE_TIM5_CC2    12
#define ADC_TRIGGER_SOURCE_TIM5_CC3    13
#define ADC_TRIGGER_SOURCE_EXTI11      15

/*******************************************************************************************************/
/* ADC Trigger Edge Options                                                                            */
/*******************************************************************************************************/

#define ADC_TRIGGER_EDGE_NONE       0
#define ADC_TRIGGER_EDGE_RISING     1
#define ADC_TRIGGER_EDGE_FALLING    2
#define ADC_TRIGGER_EDGE_BOTH       3

/*******************************************************************************************************/
/* ADC Enable/Disable Temperature Sensor and VRef                                                      */
/*******************************************************************************************************/

#define ADC_TEMP_VREF_ENABLE        1
#define ADC_TEMP_VREF_DISABLE       0

/* Configure whether to enable temperature sensor and internal voltage reference */
#define ADC_TEMPERATURE_SENSOR_ENABLE   ADC_TEMP_VREF_DISABLE
#define ADC_VREF_ENABLE                 ADC_TEMP_VREF_DISABLE

#endif /* ADC_CONFIG_H_ */
