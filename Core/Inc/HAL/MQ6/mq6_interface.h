/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.1.0                                                                          */
/* Date              : 17 Aug 2025                                                                     */
/* Description       : mq6_interface.h --> MQ6 Gas Sensor Interface Header File                        */
/* Modifications     : Modified reading functions to accept external readings array                    */
/*******************************************************************************************************/

#ifndef MQ6_INTERFACE_H_
#define MQ6_INTERFACE_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

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

#endif /* MQ6_INTERFACE_H_ */
