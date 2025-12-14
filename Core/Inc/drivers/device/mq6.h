/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.1.0                                                                          */
/* Date              : 17 Aug 2025                                                                     */
/* Description       : mq6.h --> MQ6 Gas Sensor Driver Header File                                    */
/* Modifications     : Merged interface, config, and private headers into single file                 */
/*******************************************************************************************************/

#ifndef MQ6_H_
#define MQ6_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

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

/*******************************************************************************************************/
/* MQ6 Sensor State                                                                                    */
/*******************************************************************************************************/

typedef enum
{
    MQ6_STATE_UNINITIALIZED = 0,
    MQ6_STATE_PREHEATING,
    MQ6_STATE_CALIBRATING,
    MQ6_STATE_READY,
    MQ6_STATE_ERROR
} MQ6_State_t;

/*******************************************************************************************************/
/* Function Return Status                                                                              */
/*******************************************************************************************************/

typedef enum
{
    MQ6_STATUS_OK = 0,
    MQ6_STATUS_ERROR,
    MQ6_STATUS_NOT_INITIALIZED,
    MQ6_STATUS_NOT_READY,
    MQ6_STATUS_INVALID_PARAMETER,
    MQ6_STATUS_ADC_ERROR,
    MQ6_STATUS_TIMEOUT,
    MQ6_STATUS_CALIBRATION_REQUIRED,
    MQ6_STATUS_OUT_OF_RANGE,
    MQ6_STATUS_NULL_POINTER_ERROR
} MQ6_Status_t;

/*******************************************************************************************************/
/* MQ6 Reading Structure                                                                               */
/*******************************************************************************************************/

typedef struct
{
    uint16_t raw_adc;           /* Raw ADC reading (0-4095 for 12-bit) */
    uint16_t voltage_mv;        /* Voltage in millivolts */
    uint32_t resistance_ohm;    /* Sensor resistance in ohms */
    float rs_ro_ratio;          /* Rs/Ro ratio */
    uint16_t gas_ppm;           /* Gas concentration in PPM */
    uint32_t timestamp;         /* Reading timestamp */
} MQ6_Reading_t;

/*******************************************************************************************************/
/* MQ6 Calibration Data                                                                                */
/*******************************************************************************************************/

typedef struct
{
    uint32_t ro_resistance;     /* Resistance in clean air (Ro) */
    uint8_t is_calibrated;      /* Calibration status flag */
    uint32_t calibration_time;  /* Time when calibrated */
} MQ6_CalibrationData_t;

/*******************************************************************************************************/
/* Function Prototypes                                                                                 */
/*******************************************************************************************************/

/* Initialization and Control Functions */
MQ6_Status_t MQ6_Initialize(void);
MQ6_Status_t MQ6_StartPreheat(void);
MQ6_Status_t MQ6_StartCalibration(uint16_t *Copy_u16Readings, uint16_t Copy_u16NumReadings);
MQ6_Status_t MQ6_Reset(void);
MQ6_State_t MQ6_GetState(void);

/* Reading Functions - Modified to accept external readings array */
MQ6_Status_t MQ6_ReadRaw(uint16_t* Copy_u16Readings, uint16_t* Copy_u16RawValue);
MQ6_Status_t MQ6_ReadVoltage(uint16_t* Copy_u16Readings, uint16_t* Copy_u16VoltageValue);
MQ6_Status_t MQ6_ReadResistance(uint16_t* Copy_u16Readings, uint32_t* Copy_u32ResistanceValue);
MQ6_Status_t MQ6_ReadRatio(uint16_t* Copy_u16Readings, float* Copy_fRatioValue);
MQ6_Status_t MQ6_ReadPPM(uint16_t* Copy_u16Readings, uint16_t* Copy_u16PPMValue);
MQ6_Status_t MQ6_ReadComplete(uint16_t* Copy_u16Readings, MQ6_Reading_t* Copy_Reading);

/* Calibration Functions */
MQ6_Status_t MQ6_SetRoValue(uint32_t Copy_u32RoValue);
MQ6_Status_t MQ6_GetRoValue(uint32_t* Copy_u32RoValue);
MQ6_Status_t MQ6_GetCalibrationData(MQ6_CalibrationData_t* Copy_CalibrationData);
MQ6_Status_t MQ6_IsCalibrationRequired(uint8_t* Copy_u8Required);

/* Utility Functions */
MQ6_Status_t MQ6_IsReady(uint8_t* Copy_u8Ready);
MQ6_Status_t MQ6_IsPreheated(uint8_t* Copy_u8Preheated);
uint32_t MQ6_GetPreheatTime(void);
uint32_t MQ6_GetCalibrationTime(void);

/* Conversion Functions */
uint16_t MQ6_ConvertAdcToVoltage(uint16_t Copy_u16AdcValue);
uint32_t MQ6_ConvertVoltageToResistance(uint16_t Copy_u16VoltageValue);
float MQ6_ConvertResistanceToRatio(uint32_t Copy_u32ResistanceValue, uint32_t Copy_u32RoValue);
uint16_t MQ6_ConvertRatioToPPM(float Copy_fRatioValue);

uint8_t PerformMQ6Calibration(void);
uint8_t ReadADCValues(uint16_t* Copy_AdcArray, uint8_t Copy_SampleCount);

#endif /* MQ6_H_ */
