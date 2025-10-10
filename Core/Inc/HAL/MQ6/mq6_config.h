/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 17 Aug 2025                                                                     */
/* Description       : mq6_config.h --> MQ6 Gas Sensor Configuration Header File                     */
/*******************************************************************************************************/

#ifndef MQ6_CONFIG_H_
#define MQ6_CONFIG_H_

/*******************************************************************************************************/
/* MQ6 Configuration Parameters                                                                        */
/*******************************************************************************************************/

/* MQ6 Hardware Configuration */
#define MQ6_ADC_INSTANCE            ADC_INSTANCE_1      /* ADC instance to use */
#define MQ6_ADC_CHANNEL             ADC_CHANNEL_5       /* ADC channel connected to MQ6 */
#define MQ6_SUPPLY_VOLTAGE          3300                /* Supply voltage in millivolts (3.3V) */
#define MQ6_REFERENCE_VOLTAGE       3300                /* ADC reference voltage in millivolts */

/*******************************************************************************************************/
/* MQ6 Calibration Configuration                                                                       */
/*******************************************************************************************************/

/* Sensor resistance in clean air (Ro) - needs calibration */
#define MQ6_RO_DEFAULT              10000               /* Default Ro value in ohms (10kÎ©) */
#define MQ6_LOAD_RESISTANCE         10000               /* Load resistor value in ohms (10kÎ©) */

/* Calibration timing */
#define MQ6_PREHEAT_TIME_MS         30000               /* Preheat time: 30 seconds */
#define MQ6_CALIBRATION_TIME_MS     300000              /* Calibration time: 5 minutes */
#define MQ6_CALIBRATION_SAMPLES     3                  /* Number of samples for calibration*/

/*******************************************************************************************************/
/* MQ6 Reading Configuration                                                                           */
/*******************************************************************************************************/

#define MQ6_READING_SAMPLES         10                  /* Number of samples to average */
#define MQ6_READING_DELAY_MS        50                  /* Delay between samples in ms */

/*******************************************************************************************************/
/* MQ6 Conversion Constants                                                                            */
/*******************************************************************************************************/

/* Rs/Ro ratio to PPM conversion constants (approximate values for demonstration) */
/* These values should be calibrated based on your specific sensor and gas type */
#define MQ6_CONVERSION_FACTOR_A     116.6020682         /* Curve fitting parameter A */
#define MQ6_CONVERSION_FACTOR_B     -2.769034857        /* Curve fitting parameter B */

/* Minimum and maximum detectable concentrations */
#define MQ6_MIN_PPM                 200                 /* Minimum detectable PPM */
#define MQ6_MAX_PPM                 10000               /* Maximum detectable PPM */

/*******************************************************************************************************/
/* MQ6 Enable/Disable Features                                                                         */
/*******************************************************************************************************/

#define MQ6_ENABLE_CALIBRATION      1                   /* Enable automatic calibration */
#define MQ6_ENABLE_TEMPERATURE_COMP 0                   /* Enable temperature compensation */
#define MQ6_ENABLE_HUMIDITY_COMP    0                   /* Enable humidity compensation */

#endif /* MQ6_CONFIG_H_ */
