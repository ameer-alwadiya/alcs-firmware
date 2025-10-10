/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 17 Aug 2025                                                                     */
/* Description       : mq6_program.c --> MQ6 Gas Sensor Implementation File                          */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include "mq6_interface.h"
#include "mq6_config.h"
#include "adc_interface.h"  /* Assuming ADC driver exists */
#include "stk_interface.h"
#include <math.h>
#include <string.h>

/*******************************************************************************************************/
/* Private Variables                                                                                   */
/*******************************************************************************************************/

static MQ6_State_t g_MQ6_State = MQ6_STATE_UNINITIALIZED;
static MQ6_CalibrationData_t g_MQ6_CalibrationData = { 0 };
static uint32_t g_MQ6_PreheatStartTime;
static uint32_t g_MQ6_CalibrationStartTime;
extern volatile uint8_t g_u8ConversionComplete;   /* ADC conversion complete flag */
extern volatile uint16_t g_u16AdcValue;           /* ADC reading value */

#define ADC_CONVERSION_TIMEOUT      100000  /* ADC conversion timeout */

/*******************************************************************************************************/
/* Private Function Prototypes                                                                         */
/*******************************************************************************************************/

static MQ6_Status_t MQ6_ValidatePointer(void *Copy_Pointer);
static MQ6_Status_t MQ6_AverageReading(uint16_t* Copy_u16Readings, uint16_t* Copy_u16Average);
static MQ6_Status_t MQ6_ExecuteCalibration(uint16_t *Copy_u16Readings, uint16_t Copy_u16NumReadings);

/*******************************************************************************************************/
/* Function Implementations                                                                            */
/*******************************************************************************************************/

/**
 * @brief Initialize MQ6 sensor
 * @return MQ6_Status_t Status of initialization
 */
MQ6_Status_t MQ6_Initialize(void) {
	MQ6_Status_t Local_Status = MQ6_STATUS_OK;

	/* Initialize calibration data */
	g_MQ6_CalibrationData.ro_resistance = MQ6_RO_DEFAULT;
	g_MQ6_CalibrationData.is_calibrated = 0;
	g_MQ6_CalibrationData.calibration_time = 0;

	/* Set initial state */
	g_MQ6_State = MQ6_STATE_PREHEATING;
	g_MQ6_PreheatStartTime = STK_GetMillis();

	return Local_Status;
}

/**
 * @brief Start preheating process
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_StartPreheat(void) {
	if (g_MQ6_State == MQ6_STATE_UNINITIALIZED) {
		return MQ6_STATUS_NOT_INITIALIZED;
	}

	g_MQ6_State = MQ6_STATE_PREHEATING;
	g_MQ6_PreheatStartTime = STK_GetMillis();

	return MQ6_STATUS_OK;
}

/**
 * @brief Start calibration process with external readings array
 * @param Copy_u16Readings Pointer to array containing calibration readings
 * @param Copy_u16NumReadings Number of readings in the array (should be MQ6_CALIBRATION_SAMPLES)
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_StartCalibration(uint16_t *Copy_u16Readings, uint16_t Copy_u16NumReadings) {
    uint8_t Local_u8Preheated = 0;

    /* Check if sensor is preheated */
    if (MQ6_IsPreheated(&Local_u8Preheated) != MQ6_STATUS_OK) {
        return MQ6_STATUS_ERROR;
    }

    if (Local_u8Preheated == 0) {
        return MQ6_STATUS_NOT_READY;
    }

#if MQ6_ENABLE_CALIBRATION == 1
    g_MQ6_State = MQ6_STATE_CALIBRATING;
    g_MQ6_CalibrationStartTime = STK_GetMillis();

    /* Perform calibration with provided readings */
    return MQ6_ExecuteCalibration(Copy_u16Readings, Copy_u16NumReadings);
#else
    return MQ6_STATUS_ERROR;
#endif
}

/**
 * @brief Reset MQ6 sensor
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_Reset(void) {
	g_MQ6_State = MQ6_STATE_UNINITIALIZED;
	g_MQ6_PreheatStartTime = 0;
	g_MQ6_CalibrationStartTime = 0;

	/* Reset calibration data */
	g_MQ6_CalibrationData.ro_resistance = MQ6_RO_DEFAULT;
	g_MQ6_CalibrationData.is_calibrated = 0;
	g_MQ6_CalibrationData.calibration_time = 0;

	return MQ6_Initialize();
}

/**
 * @brief Get current sensor state
 * @return MQ6_State_t Current state
 */
MQ6_State_t MQ6_GetState(void) {
	uint8_t Local_u8Preheated = 0;

	/* Update state based on timing */
	if (g_MQ6_State == MQ6_STATE_PREHEATING) {
		if (MQ6_IsPreheated(&Local_u8Preheated) == MQ6_STATUS_OK
				&& Local_u8Preheated == 1) {
			if (g_MQ6_CalibrationData.is_calibrated == 1) {
				g_MQ6_State = MQ6_STATE_READY;
			} else {
				/* Calibration required */
				g_MQ6_State = MQ6_STATE_PREHEATING; /* Stay in preheating until calibrated */
			}
		}
	}

	return g_MQ6_State;
}

/**
 * @brief Read raw ADC value with external readings array
 * @param Copy_u16Readings Pointer to array containing MQ6_READING_SAMPLES readings
 * @param Copy_u16RawValue Pointer to store raw value
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_ReadRaw(uint16_t *Copy_u16Readings, uint16_t *Copy_u16RawValue) {
    if (MQ6_ValidatePointer(Copy_u16RawValue) != MQ6_STATUS_OK) {
        return MQ6_STATUS_INVALID_PARAMETER;
    }

    if (MQ6_ValidatePointer(Copy_u16Readings) != MQ6_STATUS_OK) {
        return MQ6_STATUS_INVALID_PARAMETER;
    }

    if (g_MQ6_State == MQ6_STATE_UNINITIALIZED) {
        return MQ6_STATUS_NOT_INITIALIZED;
    }

    /* Calculate average from provided readings */
    return MQ6_AverageReading(Copy_u16Readings, Copy_u16RawValue);
}

/**
 * @brief Read voltage value with external readings array
 * @param Copy_u16Readings Pointer to array containing MQ6_READING_SAMPLES readings
 * @param Copy_u16VoltageValue Pointer to store voltage in mV
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_ReadVoltage(uint16_t *Copy_u16Readings, uint16_t *Copy_u16VoltageValue) {
    uint16_t Local_u16RawValue = 0;
    MQ6_Status_t Local_Status;

    if (MQ6_ValidatePointer(Copy_u16VoltageValue) != MQ6_STATUS_OK) {
        return MQ6_STATUS_INVALID_PARAMETER;
    }

    Local_Status = MQ6_ReadRaw(Copy_u16Readings, &Local_u16RawValue);
    printf("Local_u16RawValue %u /r/n/r/n", Local_u16RawValue);

    if (Local_Status == MQ6_STATUS_OK) {
        *Copy_u16VoltageValue = MQ6_ConvertAdcToVoltage(Local_u16RawValue);
    }

    return Local_Status;
}

/**
 * @brief Read sensor resistance with external readings array
 * @param Copy_u16Readings Pointer to array containing MQ6_READING_SAMPLES readings
 * @param Copy_u32ResistanceValue Pointer to store resistance in ohms
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_ReadResistance(uint16_t *Copy_u16Readings, uint32_t *Copy_u32ResistanceValue) {
    uint16_t Local_u16VoltageValue = 0;
    MQ6_Status_t Local_Status;

    if (MQ6_ValidatePointer(Copy_u32ResistanceValue) != MQ6_STATUS_OK) {
        return MQ6_STATUS_INVALID_PARAMETER;
    }

    Local_Status = MQ6_ReadVoltage(Copy_u16Readings, &Local_u16VoltageValue);
    printf("Local_u16VoltageValue %u /r/n/r/n", Local_u16VoltageValue);

    if (Local_Status == MQ6_STATUS_OK) {
        *Copy_u32ResistanceValue = MQ6_ConvertVoltageToResistance(Local_u16VoltageValue);
    }

    return Local_Status;
}

/**
 * @brief Read Rs/Ro ratio with external readings array
 * @param Copy_u16Readings Pointer to array containing MQ6_READING_SAMPLES readings
 * @param Copy_fRatioValue Pointer to store ratio
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_ReadRatio(uint16_t *Copy_u16Readings, float *Copy_fRatioValue) {
    uint32_t Local_u32ResistanceValue = 0;
    MQ6_Status_t Local_Status;

    if (MQ6_ValidatePointer(Copy_fRatioValue) != MQ6_STATUS_OK) {
        return MQ6_STATUS_INVALID_PARAMETER;
    }

    if (g_MQ6_CalibrationData.is_calibrated == 0) {
        return MQ6_STATUS_CALIBRATION_REQUIRED;
    }

    Local_Status = MQ6_ReadResistance(Copy_u16Readings, &Local_u32ResistanceValue);
    if (Local_Status == MQ6_STATUS_OK) {
        *Copy_fRatioValue = MQ6_ConvertResistanceToRatio(
            Local_u32ResistanceValue, g_MQ6_CalibrationData.ro_resistance);
    }

    return Local_Status;
}

/**
 * @brief Read gas concentration in PPM with external readings array
 * @param Copy_u16Readings Pointer to array containing MQ6_READING_SAMPLES readings
 * @param Copy_u16PPMValue Pointer to store PPM value
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_ReadPPM(uint16_t *Copy_u16Readings, uint16_t *Copy_u16PPMValue) {
    float Local_fRatioValue = 0.0f;
    MQ6_Status_t Local_Status;
    uint16_t Local_u16PPM = 0;

    if (MQ6_ValidatePointer(Copy_u16PPMValue) != MQ6_STATUS_OK) {
        return MQ6_STATUS_INVALID_PARAMETER;
    }

    if (g_MQ6_State != MQ6_STATE_READY) {
        return MQ6_STATUS_NOT_READY;
    }

    Local_Status = MQ6_ReadRatio(Copy_u16Readings, &Local_fRatioValue);
    if (Local_Status == MQ6_STATUS_OK) {
        Local_u16PPM = MQ6_ConvertRatioToPPM(Local_fRatioValue);

        /* Check if reading is within valid range */
        if (Local_u16PPM < MQ6_MIN_PPM || Local_u16PPM > MQ6_MAX_PPM) {
            return MQ6_STATUS_OUT_OF_RANGE;
        }

        *Copy_u16PPMValue = Local_u16PPM;
    }

    return Local_Status;
}

/**
 * @brief Read complete sensor data with external readings array
 * @param Copy_u16Readings Pointer to array containing MQ6_READING_SAMPLES readings
 * @param Copy_Reading Pointer to reading structure
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_ReadComplete(uint16_t *Copy_u16Readings, MQ6_Reading_t *Copy_Reading) {
    MQ6_Status_t Local_Status;

    if (MQ6_ValidatePointer(Copy_Reading) != MQ6_STATUS_OK) {
        return MQ6_STATUS_INVALID_PARAMETER;
    }

    /* Clear structure */
    memset(Copy_Reading, 0, sizeof(MQ6_Reading_t));

    /* Read raw ADC */
    Local_Status = MQ6_ReadRaw(Copy_u16Readings, &Copy_Reading->raw_adc);
    if (Local_Status != MQ6_STATUS_OK) {
        return Local_Status;
    }

    /* Convert to voltage */
    Copy_Reading->voltage_mv = MQ6_ConvertAdcToVoltage(Copy_Reading->raw_adc);

    /* Convert to resistance */
    Copy_Reading->resistance_ohm = MQ6_ConvertVoltageToResistance(
        Copy_Reading->voltage_mv);

    /* Calculate ratio if calibrated */
    if (g_MQ6_CalibrationData.is_calibrated == 1) {
        Copy_Reading->rs_ro_ratio = MQ6_ConvertResistanceToRatio(
            Copy_Reading->resistance_ohm,
            g_MQ6_CalibrationData.ro_resistance);

        /* Convert to PPM if ready */
        if (g_MQ6_State == MQ6_STATE_READY) {
            Copy_Reading->gas_ppm = MQ6_ConvertRatioToPPM(
                Copy_Reading->rs_ro_ratio);
        }
    }

    /* Set timestamp */
    Copy_Reading->timestamp = STK_GetMillis();

    return MQ6_STATUS_OK;
}

/**
 * @brief Set Ro value manually
 * @param Copy_u32RoValue Ro resistance value
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_SetRoValue(uint32_t Copy_u32RoValue) {
	if (Copy_u32RoValue == 0) {
		return MQ6_STATUS_INVALID_PARAMETER;
	}

	g_MQ6_CalibrationData.ro_resistance = Copy_u32RoValue;
	g_MQ6_CalibrationData.is_calibrated = 1;
	g_MQ6_CalibrationData.calibration_time = STK_GetMillis();

	return MQ6_STATUS_OK;
}

/**
 * @brief Get current Ro value
 * @param Copy_u32RoValue Pointer to store Ro value
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_GetRoValue(uint32_t *Copy_u32RoValue) {
	if (MQ6_ValidatePointer(Copy_u32RoValue) != MQ6_STATUS_OK) {
		return MQ6_STATUS_INVALID_PARAMETER;
	}

	*Copy_u32RoValue = g_MQ6_CalibrationData.ro_resistance;

	return MQ6_STATUS_OK;
}

/**
 * @brief Get calibration data
 * @param Copy_CalibrationData Pointer to calibration data structure
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_GetCalibrationData(MQ6_CalibrationData_t *Copy_CalibrationData) {
	if (MQ6_ValidatePointer(Copy_CalibrationData) != MQ6_STATUS_OK) {
		return MQ6_STATUS_INVALID_PARAMETER;
	}

	memcpy(Copy_CalibrationData, &g_MQ6_CalibrationData,
			sizeof(MQ6_CalibrationData_t));

	return MQ6_STATUS_OK;
}

/**
 * @brief Check if calibration is required
 * @param Copy_u8Required Pointer to store result
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_IsCalibrationRequired(uint8_t *Copy_u8Required) {
	if (MQ6_ValidatePointer(Copy_u8Required) != MQ6_STATUS_OK) {
		return MQ6_STATUS_INVALID_PARAMETER;
	}

	*Copy_u8Required = (g_MQ6_CalibrationData.is_calibrated == 0) ? 1 : 0;

	return MQ6_STATUS_OK;
}

/**
 * @brief Check if sensor is ready
 * @param Copy_u8Ready Pointer to store result
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_IsReady(uint8_t *Copy_u8Ready) {
	if (MQ6_ValidatePointer(Copy_u8Ready) != MQ6_STATUS_OK) {
		return MQ6_STATUS_INVALID_PARAMETER;
	}

	*Copy_u8Ready = (MQ6_GetState() == MQ6_STATE_READY) ? 1 : 0;

	return MQ6_STATUS_OK;
}

/**
 * @brief Check if sensor is preheated
 * @param Copy_u8Preheated Pointer to store result
 * @return MQ6_Status_t Status of operation
 */
MQ6_Status_t MQ6_IsPreheated(uint8_t *Copy_u8Preheated) {
	uint32_t Local_u32ElapsedTime;

	if (MQ6_ValidatePointer(Copy_u8Preheated) != MQ6_STATUS_OK) {
		return MQ6_STATUS_INVALID_PARAMETER;
	}

	if (g_MQ6_PreheatStartTime == 0) {
		*Copy_u8Preheated = 0;
		return MQ6_STATUS_OK;
	}

	Local_u32ElapsedTime = STK_GetMillis() - g_MQ6_PreheatStartTime;
	*Copy_u8Preheated = (Local_u32ElapsedTime >= MQ6_PREHEAT_TIME_MS) ? 1 : 0;

	return MQ6_STATUS_OK;
}

/**
 * @brief Get remaining preheat time
 * @return uint32_t Remaining time in milliseconds
 */
uint32_t MQ6_GetPreheatTime(void) {
	uint32_t Local_u32ElapsedTime;

	if (g_MQ6_PreheatStartTime == 0) {
		return MQ6_PREHEAT_TIME_MS;
	}

	Local_u32ElapsedTime = STK_GetMillis() - g_MQ6_PreheatStartTime;

	if (Local_u32ElapsedTime >= MQ6_PREHEAT_TIME_MS) {
		return 0;
	}

	return (MQ6_PREHEAT_TIME_MS - Local_u32ElapsedTime);
}

/**
 * @brief Get remaining calibration time
 * @return uint32_t Remaining time in milliseconds
 */
uint32_t MQ6_GetCalibrationTime(void) {
	uint32_t Local_u32ElapsedTime;

	if (g_MQ6_CalibrationStartTime == 0) {
		return MQ6_CALIBRATION_TIME_MS;
	}

	Local_u32ElapsedTime = STK_GetMillis() - g_MQ6_CalibrationStartTime;

	if (Local_u32ElapsedTime >= MQ6_CALIBRATION_TIME_MS) {
		return 0;
	}

	return (MQ6_CALIBRATION_TIME_MS - Local_u32ElapsedTime);
}

/**
 * @brief Convert ADC reading to voltage
 * @param Copy_u16AdcValue ADC value (0-4095 for 12-bit)
 * @return uint16_t Voltage in millivolts
 */
uint16_t MQ6_ConvertAdcToVoltage(uint16_t Copy_u16AdcValue) {

	return (uint16_t) ((Copy_u16AdcValue * MQ6_REFERENCE_VOLTAGE) / 4095);
}

/**
 * @brief Convert voltage to sensor resistance
 * @param Copy_u16VoltageValue Voltage in millivolts
 * @return uint32_t Resistance in ohms
 */
uint32_t MQ6_ConvertVoltageToResistance(uint16_t Copy_u16VoltageValue) {
    /* Prevent division by zero and handle invalid readings */
    if (Copy_u16VoltageValue == 0 || Copy_u16VoltageValue >= MQ6_SUPPLY_VOLTAGE) {
        return 0; /* Return 0 as an indicator of an invalid resistance reading */
    }

    /* Rs = RL * (Vc - Vout) / Vout */
    // 3300
    // 348
    // MQ6_LOAD_RESISTANCE         10000
    uint32_t Local_u32Resistance = (uint32_t)MQ6_LOAD_RESISTANCE
                                 * (MQ6_SUPPLY_VOLTAGE - Copy_u16VoltageValue)
                                 / Copy_u16VoltageValue;

    return Local_u32Resistance;
}

/**
 * @brief Convert resistance to Rs/Ro ratio
 * @param Copy_u32ResistanceValue Current resistance
 * @param Copy_u32RoValue Reference resistance (Ro)
 * @return float Rs/Ro ratio
 */
float MQ6_ConvertResistanceToRatio(uint32_t Copy_u32ResistanceValue,
		uint32_t Copy_u32RoValue) {
	if (Copy_u32RoValue == 0) {
		return 0.0f;
	}

	return (float) Copy_u32ResistanceValue / (float) Copy_u32RoValue;
}

/**
 * @brief Convert Rs/Ro ratio to PPM
 * @param Copy_fRatioValue Rs/Ro ratio
 * @return uint16_t Gas concentration in PPM
 */
uint16_t MQ6_ConvertRatioToPPM(float Copy_fRatioValue) {
	float Local_fPPM;

	if (Copy_fRatioValue <= 0.0f) {
		return 0;
	}

	/* PPM = A * (Rs/Ro)^B */
	Local_fPPM = MQ6_CONVERSION_FACTOR_A
			* powf(Copy_fRatioValue, MQ6_CONVERSION_FACTOR_B);

	/* Ensure result is within valid range */
	if (Local_fPPM < 0.0f) {
		Local_fPPM = 0.0f;
	} else if (Local_fPPM > 65535.0f) {
		Local_fPPM = 65535.0f;
	}

	return (uint16_t) Local_fPPM;
}

/****************************************************************************/
/* PerformMQ6Calibration                                                    */
/* Function Description -> Perform MQ6 sensor calibration                  */
/* Function Input       -> None                                             */
/* Function Return      -> 1 if successful, 0 if failed/in progress        */
/****************************************************************************/

/*It's designed to be called repeatedly and uses a state machine approach to
 * collect multiple ADC samples over time before calculating the sensor's baseline
 * resistance (Ro).*/

uint8_t PerformMQ6Calibration(void)
{
  static uint16_t calibration_readings[MQ6_CALIBRATION_SAMPLES]; /* MQ6_CALIBRATION_SAMPLES */
  static uint8_t calibration_in_progress = 0;
  static uint8_t sample_index = 0;
  uint16_t adc_readings[MQ6_READING_SAMPLES]; /* Array for multiple ADC samples */
  uint16_t averaged_sample;
  MQ6_Status_t status;

  if (!calibration_in_progress) {
    printf("[MQ6] Starting calibration process...\r\n");
    calibration_in_progress = 1;
    sample_index = 0;
    return 0; /* Calibration just started */
  }

  /* Collect calibration samples */
  if (sample_index < MQ6_CALIBRATION_SAMPLES) {
    /* Take MQ6_READING_SAMPLES ADC readings for this calibration sample */
    if (ReadADCValues(adc_readings, MQ6_READING_SAMPLES)) {
      /* Average the multiple ADC readings into a single clean sample */
      if (MQ6_AverageReading(adc_readings, &averaged_sample) == MQ6_STATUS_OK) {
        calibration_readings[sample_index] = averaged_sample;
        sample_index++;
        printf("[MQ6] Calibration sample %d/%d: %u (avg of %d readings)\r\n",
               sample_index, MQ6_CALIBRATION_SAMPLES, averaged_sample, MQ6_READING_SAMPLES);
      } else {
        printf("[MQ6] Failed to average calibration sample %d\r\n", sample_index);
      }
    } else {
      printf("[MQ6] Failed to read calibration sample %d\r\n", sample_index);
    }
    return 0; /* Still collecting samples */
  }

  /* All samples collected, perform calibration */
  printf("[MQ6] All samples collected. Performing calibration...\r\n");
  status = MQ6_StartCalibration(calibration_readings, MQ6_CALIBRATION_SAMPLES);

  calibration_in_progress = 0; /* Reset for next time */
  sample_index = 0;

  if (status == MQ6_STATUS_OK) {
    uint32_t ro_value;
    MQ6_GetRoValue(&ro_value);
    printf("[MQ6] Calibration successful! Ro = %lu ohms\r\n", ro_value);
    return 1; /* Success */
  } else {
    printf("[MQ6] Calibration failed! Status: %d\r\n", status);
    return 0; /* Failed */
  }
}


/****************************************************************************/
/* ReadADCValues                                                            */
/* Function Description -> Read multiple ADC values for averaging          */
/* Function Input       -> Array to store values, number of samples        */
/* Function Return      -> 1 if successful, 0 if failed                    */
/****************************************************************************/
uint8_t ReadADCValues(uint16_t* Copy_AdcArray, uint8_t Copy_SampleCount)
{
  ADC_Status_t adcStatus;
  uint32_t timeout;
  uint8_t i;

  if (Copy_AdcArray == NULL || Copy_SampleCount == 0) {
    return 0;
  }

  for (i = 0; i < Copy_SampleCount; i++) {
    /* Reset completion flag */
    g_u8ConversionComplete = 0;

    /* Start ADC conversion on Channel 5 (PA5) */
    adcStatus = ADC_StartConversion(ADC_INSTANCE_1, ADC_CHANNEL_5);
    if (adcStatus != ADC_STATUS_OK) {
      printf("Failed to start ADC conversion %d, status: %d\r\n", i, adcStatus);
      return 0;
    }

    /* Wait for conversion completion with timeout */
    timeout = ADC_CONVERSION_TIMEOUT;
    while (g_u8ConversionComplete == 0 && timeout > 0) {
      timeout--;
    }

    if (g_u8ConversionComplete) {
      Copy_AdcArray[i] = g_u16AdcValue;
      /* Small delay between readings */
      STK_DelayMs(50);
    } else {
      printf("ADC conversion %d timeout!\r\n", i);
      return 0;
    }
  }

  return 1; /* Success */
}

/*******************************************************************************************************/
/* Private Function Implementations                                                                    */
/*******************************************************************************************************/

/**
 * @brief Validate pointer parameter
 * @param Copy_Pointer Pointer to validate
 * @return MQ6_Status_t Status of validation
 */
static MQ6_Status_t MQ6_ValidatePointer(void *Copy_Pointer) {
	return (Copy_Pointer == NULL) ? MQ6_STATUS_INVALID_PARAMETER : MQ6_STATUS_OK;
}

/**
 * Calculate average from pre-collected readings array
 * @param Copy_u16Readings: Pointer to array containing MQ6_READING_SAMPLES readings
 * @param Copy_u16Average: Pointer to store the calculated average
 * @return MQ6_Status_t: Status of the operation
 */
static MQ6_Status_t MQ6_AverageReading(uint16_t *Copy_u16Readings, uint16_t *Copy_u16Average) {
    uint32_t Local_u32Sum = 0;
    uint8_t Local_u8Index;

    /* Validate input pointer */
    if (Copy_u16Readings == NULL || Copy_u16Average == NULL) {
        return MQ6_STATUS_NULL_POINTER_ERROR;
    }

    /* Calculate sum of all readings */
    for (Local_u8Index = 0; Local_u8Index < MQ6_READING_SAMPLES; Local_u8Index++) {
        Local_u32Sum += Copy_u16Readings[Local_u8Index];
    }

    /* Calculate average */
    *Copy_u16Average = (uint16_t)(Local_u32Sum / MQ6_READING_SAMPLES);

    return MQ6_STATUS_OK;
}

/**
 * @brief Perform sensor calibration with external readings array
 * @param Copy_u16Readings Pointer to array containing calibration readings
 * @param Copy_u16NumReadings Number of readings in the array (should be MQ6_CALIBRATION_SAMPLES)
 * @return MQ6_Status_t Status of operation
 */
/* CORRECTED and SIMPLIFIED MQ6_ExecuteCalibration */
static MQ6_Status_t MQ6_ExecuteCalibration(uint16_t *Copy_u16Readings, uint16_t Copy_u16NumReadings) {
#if MQ6_ENABLE_CALIBRATION == 1
    uint32_t Local_u32ResistanceSum = 0;
    uint32_t Local_u32Resistance;
    uint16_t Local_u16Voltage;
    uint8_t Local_u8Index;

    /* Validate input parameters */
    if (MQ6_ValidatePointer(Copy_u16Readings) != MQ6_STATUS_OK || Copy_u16NumReadings != MQ6_CALIBRATION_SAMPLES) {
        return MQ6_STATUS_INVALID_PARAMETER;
    }

    /* Process multiple ADC readings for calibration */
    for (Local_u8Index = 0; Local_u8Index < Copy_u16NumReadings; Local_u8Index++) {
        /* Directly convert the ADC reading from the provided array */
        Local_u16Voltage = MQ6_ConvertAdcToVoltage(Copy_u16Readings[Local_u8Index]);

        Local_u32Resistance = MQ6_ConvertVoltageToResistance(Local_u16Voltage);

        if (Local_u32Resistance == 0) {
            /* Handle potential error from conversion (e.g., division by zero) */
            return MQ6_STATUS_ERROR;
        }

        Local_u32ResistanceSum += Local_u32Resistance;
    }
    printf("Voltage %d, Ro: %d, Num of Reading: %d\r\n", Local_u16Voltage, Local_u32Resistance,Copy_u16NumReadings);

    /* Calculate average resistance (Ro) */
    g_MQ6_CalibrationData.ro_resistance = Local_u32ResistanceSum / Copy_u16NumReadings;
    g_MQ6_CalibrationData.is_calibrated = 1;
    g_MQ6_CalibrationData.calibration_time = STK_GetMillis();

    /* Update state to ready */
    g_MQ6_State = MQ6_STATE_READY;

    return MQ6_STATUS_OK;
#else
    return MQ6_STATUS_ERROR;
#endif
}
