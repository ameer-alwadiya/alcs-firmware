/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 13 Aug 2025                                                                     */
/* Description       : adc_private.h --> ADC Private Header File                                      */
/*******************************************************************************************************/

#ifndef ADC_PRIVATE_H_
#define ADC_PRIVATE_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

/*******************************************************************************************************/
/* ADC Base Addresses                                                                                  */
/*******************************************************************************************************/

#define ADC1_BASE_ADDRESS       0x40012000UL
#define ADC_COMMON_BASE_ADDRESS 0x40012300UL

/*******************************************************************************************************/
/* ADC Timeout Value                                                                                   */
/*******************************************************************************************************/

#define ADC_TIMEOUT_VALUE       100000UL

/*******************************************************************************************************/
/* ADC Register Structure                                                                              */
/*******************************************************************************************************/

typedef struct
{
    uint32_t SR;        /* ADC status register */
    uint32_t CR1;       /* ADC control register 1 */
    uint32_t CR2;       /* ADC control register 2 */
    uint32_t SMPR1;     /* ADC sample time register 1 */
    uint32_t SMPR2;     /* ADC sample time register 2 */
    uint32_t JOFR1;     /* ADC injected channel data offset register 1 */
    uint32_t JOFR2;     /* ADC injected channel data offset register 2 */
    uint32_t JOFR3;     /* ADC injected channel data offset register 3 */
    uint32_t JOFR4;     /* ADC injected channel data offset register 4 */
    uint32_t HTR;       /* ADC watchdog higher threshold register */
    uint32_t LTR;       /* ADC watchdog lower threshold register */
    uint32_t SQR1;      /* ADC regular sequence register 1 */
    uint32_t SQR2;      /* ADC regular sequence register 2 */
    uint32_t SQR3;      /* ADC regular sequence register 3 */
    uint32_t JSQR;      /* ADC injected sequence register */
    uint32_t JDR1;      /* ADC injected data register 1 */
    uint32_t JDR2;      /* ADC injected data register 2 */
    uint32_t JDR3;      /* ADC injected data register 3 */
    uint32_t JDR4;      /* ADC injected data register 4 */
    uint32_t DR;        /* ADC regular data register */
} ADC_Registers_t;

typedef struct
{
    uint32_t CSR;       /* ADC Common status register */
    uint32_t CCR;       /* ADC common control register */
    uint32_t CDR;       /* ADC common regular data register for dual and triple modes */
} ADC_Common_Registers_t;

/*******************************************************************************************************/
/* ADC Register Pointers                                                                               */
/*******************************************************************************************************/

#define ADC1_REG        ((volatile ADC_Registers_t*)(ADC1_BASE_ADDRESS))
#define ADC_COMMON_REG  ((volatile ADC_Common_Registers_t*)(ADC_COMMON_BASE_ADDRESS))

/*******************************************************************************************************/
/* ADC Status Register Bits                                                                            */
/*******************************************************************************************************/

#define ADC_SR_AWD_BIT          0   /* Analog watchdog flag */
#define ADC_SR_EOC_BIT          1   /* End of conversion */
#define ADC_SR_JEOC_BIT         2   /* Injected channel end of conversion */
#define ADC_SR_JSTRT_BIT        3   /* Injected channel start flag */
#define ADC_SR_STRT_BIT         4   /* Regular channel start flag */
#define ADC_SR_OVR_BIT          5   /* Overrun */

/*******************************************************************************************************/
/* ADC Control Register 1 Bits                                                                         */
/*******************************************************************************************************/

#define ADC_CR1_AWDCH_BIT       0   /* Analog watchdog channel select bits [4:0] */
#define ADC_CR1_EOCIE_BIT       5   /* Interrupt enable for EOC */
#define ADC_CR1_AWDIE_BIT       6   /* Analog watchdog interrupt enable */
#define ADC_CR1_JEOCIE_BIT      7   /* Interrupt enable for injected channels */
#define ADC_CR1_SCAN_BIT        8   /* Scan mode */
#define ADC_CR1_AWDSGL_BIT      9   /* Enable the watchdog on a single channel in scan mode */
#define ADC_CR1_JAUTO_BIT       10  /* Automatic injected group conversion */
#define ADC_CR1_DISCEN_BIT      11  /* Discontinuous mode on regular channels */
#define ADC_CR1_JDISCEN_BIT     12  /* Discontinuous mode on injected channels */
#define ADC_CR1_DISCNUM_BIT     13  /* Discontinuous mode channel count [2:0] */
#define ADC_CR1_JAWDEN_BIT      22  /* Analog watchdog enable on injected channels */
#define ADC_CR1_AWDEN_BIT       23  /* Analog watchdog enable on regular channels */
#define ADC_CR1_RES_BIT         24  /* Resolution [1:0] */
#define ADC_CR1_OVRIE_BIT       26  /* Overrun interrupt enable */

/*******************************************************************************************************/
/* ADC Control Register 2 Bits                                                                         */
/*******************************************************************************************************/

#define ADC_CR2_ADON_BIT        0   /* A/D Converter ON / OFF */
#define ADC_CR2_CONT_BIT        1   /* Continuous conversion */
#define ADC_CR2_DMA_BIT         8   /* Direct memory access mode */
#define ADC_CR2_DDS_BIT         9   /* DMA disable selection */
#define ADC_CR2_EOCS_BIT        10  /* End of conversion selection */
#define ADC_CR2_ALIGN_BIT       11  /* Data alignment */
#define ADC_CR2_JEXTSEL_BIT     16  /* External event select for injected group [3:0] */
#define ADC_CR2_JEXTEN_BIT      20  /* External trigger enable for injected channels [1:0] */
#define ADC_CR2_JSWSTART_BIT    22  /* Start conversion of injected channels */
#define ADC_CR2_EXTSEL_BIT      24  /* External event select for regular group [3:0] */
#define ADC_CR2_EXTEN_BIT       28  /* External trigger enable for regular channels [1:0] */
#define ADC_CR2_SWSTART_BIT     30  /* Start conversion of regular channels */

/*******************************************************************************************************/
/* ADC Common Control Register Bits                                                                    */
/*******************************************************************************************************/

#define ADC_CCR_MULTI_BIT       0   /* Multi ADC mode selection [4:0] */
#define ADC_CCR_DELAY_BIT       8   /* Delay between 2 sampling phases [3:0] */
#define ADC_CCR_DDS_BIT         13  /* DMA disable selection */
#define ADC_CCR_DMA_BIT         14  /* Direct memory access mode for multi ADC mode [1:0] */
#define ADC_CCR_ADCPRE_BIT      16  /* ADC prescaler [1:0] */
#define ADC_CCR_VBATE_BIT       22  /* VBAT enable */
#define ADC_CCR_TSVREFE_BIT     23  /* Temperature sensor and VREFINT enable */

/*******************************************************************************************************/
/* ADC Sample Time Values                                                                              */
/*******************************************************************************************************/

#define ADC_SAMPLE_TIME_3_CYCLES    0x0
#define ADC_SAMPLE_TIME_15_CYCLES   0x1
#define ADC_SAMPLE_TIME_28_CYCLES   0x2
#define ADC_SAMPLE_TIME_56_CYCLES   0x3
#define ADC_SAMPLE_TIME_84_CYCLES   0x4
#define ADC_SAMPLE_TIME_112_CYCLES  0x5
#define ADC_SAMPLE_TIME_144_CYCLES  0x6
#define ADC_SAMPLE_TIME_480_CYCLES  0x7

/*******************************************************************************************************/
/* ADC Resolution Values                                                                               */
/*******************************************************************************************************/

#define ADC_RESOLUTION_12_BIT       0x0
#define ADC_RESOLUTION_10_BIT       0x1
#define ADC_RESOLUTION_8_BIT        0x2
#define ADC_RESOLUTION_6_BIT        0x3

/*******************************************************************************************************/
/* ADC Prescaler Values                                                                                */
/*******************************************************************************************************/

#define ADC_PRESCALER_DIV2          0x0
#define ADC_PRESCALER_DIV4          0x1
#define ADC_PRESCALER_DIV6          0x2
#define ADC_PRESCALER_DIV8          0x3

/*******************************************************************************************************/
/* ADC Temperature Sensor Constants                                                                    */
/*******************************************************************************************************/

#define ADC_TEMP_V25                1430    /* Temperature sensor voltage at 25°C (mV) */
#define ADC_TEMP_AVG_SLOPE          4300    /* Temperature sensor average slope (uV/°C) */
#define ADC_VREFINT_VOLTAGE         1210    /* Internal reference voltage (mV) */

#endif /* ADC_PRIVATE_H_ */
