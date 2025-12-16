/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 15 Dec 2025                                                                     */
/* Description       : smart_light.h --> Smart Lighting Driver Header File                             */
/*                     Three-channel color temperature LED control with automatic mode                 */
/*******************************************************************************************************/

#ifndef SMART_LIGHT_H_
#define SMART_LIGHT_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/
#include <stdint.h>
#include "tim.h"
#include "bmp280.h"
#include "tsl2561.h"

/*******************************************************************************************************/
/* Smart Light Configuration Parameters                                                                */
/*******************************************************************************************************/

/* Timer Configuration */
#define SMART_LIGHT_TIMER               TIM2
#define SMART_LIGHT_PRESCALER           16
#define SMART_LIGHT_PERIOD              1000    /* 0-1000 duty cycle resolution */

/* PWM Channel Mapping */
#define SMART_LIGHT_WARM_CHANNEL        TIM_CHANNEL_1   /* PA5 - Warm White (3000K) */
#define SMART_LIGHT_NEUTRAL_CHANNEL     TIM_CHANNEL_2   /* PA6 - Neutral White (4000K) */
#define SMART_LIGHT_COOL_CHANNEL        TIM_CHANNEL_3   /* PA7 - Cool White (6000K) */

/* Duty Cycle Limits */
#define SMART_LIGHT_MIN_DUTY            0
#define SMART_LIGHT_MAX_DUTY            1000

/* Auto Mode Update Interval */
#define SMART_LIGHT_AUTO_UPDATE_MS      2000    /* Update automatic lighting every 2 seconds */

/* Light Level Thresholds (lux) for automatic color selection */
#define SMART_LIGHT_LUX_VERY_DARK       50
#define SMART_LIGHT_LUX_DIM             200
#define SMART_LIGHT_LUX_TWILIGHT        500
#define SMART_LIGHT_LUX_DUSK            1000
#define SMART_LIGHT_LUX_INDOOR          5000
#define SMART_LIGHT_LUX_BRIGHT          10000

/* Temperature Thresholds (Celsius) for automatic color adjustment */
#define SMART_LIGHT_TEMP_COLD           18      /* Below this: shift to warmer */
#define SMART_LIGHT_TEMP_HOT            28      /* Above this: shift to cooler */

/*******************************************************************************************************/
/* Smart Light Operating Modes                                                                         */
/*******************************************************************************************************/
typedef enum
{
    SMART_LIGHT_MODE_MANUAL = 0,    /* Manual control via MQTT/API */
    SMART_LIGHT_MODE_AUTOMATIC = 1  /* Automatic based on TSL2561 and BMP280 */
} SmartLight_Mode_t;

/*******************************************************************************************************/
/* Smart Light Channel Values Structure                                                                */
/*******************************************************************************************************/
typedef struct
{
    uint32_t warm;      /* Warm white (3000K) duty cycle: 0-1000 */
    uint32_t neutral;   /* Neutral white (4000K) duty cycle: 0-1000 */
    uint32_t cool;      /* Cool white (6000K) duty cycle: 0-1000 */
} SmartLight_Channels_t;

/*******************************************************************************************************/
/* Smart Light Control State Structure                                                                 */
/*******************************************************************************************************/
typedef struct
{
    SmartLight_Mode_t mode;         /* Current operating mode */
    SmartLight_Channels_t channels; /* Current channel duty cycles */
    uint32_t brightness;            /* Overall brightness percentage: 0-100 */
    uint32_t last_lux;              /* Last lux reading for auto mode */
    int32_t last_temp;              /* Last temperature (0.01C units) for auto mode */
    uint32_t last_update_time;      /* Timestamp of last auto update */
} SmartLight_State_t;

/*******************************************************************************************************/
/* Smart Light Status Codes                                                                            */
/*******************************************************************************************************/
typedef enum
{
    SMART_LIGHT_STATUS_OK = 0,
    SMART_LIGHT_STATUS_ERROR,
    SMART_LIGHT_STATUS_INVALID_PARAM,
    SMART_LIGHT_STATUS_NOT_INITIALIZED,
    SMART_LIGHT_STATUS_SENSOR_ERROR
} SmartLight_Status_t;

/*******************************************************************************************************/
/* Smart Light Color Temperature Presets                                                               */
/*******************************************************************************************************/
typedef enum
{
    SMART_LIGHT_PRESET_WARM = 0,    /* Full warm white (3000K) */
    SMART_LIGHT_PRESET_NEUTRAL,     /* Full neutral white (4000K) */
    SMART_LIGHT_PRESET_COOL,        /* Full cool white (6000K) */
    SMART_LIGHT_PRESET_MIXED,       /* Equal mix of all */
    SMART_LIGHT_PRESET_OFF          /* All channels off */
} SmartLight_Preset_t;

/*******************************************************************************************************/
/* Function Prototypes - Initialization                                                                */
/*******************************************************************************************************/

/**
 * @brief  Initialize the smart lighting system
 * @note   Initializes PWM channels for all three color temperatures
 * @retval SmartLight_Status_t: Status of initialization
 */
SmartLight_Status_t SmartLight_Initialize(void);

/*******************************************************************************************************/
/* Function Prototypes - Mode Control                                                                  */
/*******************************************************************************************************/

/**
 * @brief  Set the lighting control mode
 * @param  Copy_Mode: SMART_LIGHT_MODE_MANUAL or SMART_LIGHT_MODE_AUTOMATIC
 * @retval SmartLight_Status_t: Status of operation
 */
SmartLight_Status_t SmartLight_SetMode(SmartLight_Mode_t Copy_Mode);

/**
 * @brief  Get the current lighting control mode
 * @param  Copy_pMode: Pointer to store the current mode
 * @retval SmartLight_Status_t: Status of operation
 */
SmartLight_Status_t SmartLight_GetMode(SmartLight_Mode_t* Copy_pMode);

/*******************************************************************************************************/
/* Function Prototypes - Manual Control                                                                */
/*******************************************************************************************************/

/**
 * @brief  Set channel values manually (switches to manual mode)
 * @param  Copy_Warm: Warm white duty cycle (0-1000)
 * @param  Copy_Neutral: Neutral white duty cycle (0-1000)
 * @param  Copy_Cool: Cool white duty cycle (0-1000)
 * @retval SmartLight_Status_t: Status of operation
 */
SmartLight_Status_t SmartLight_SetChannels(uint32_t Copy_Warm, uint32_t Copy_Neutral, uint32_t Copy_Cool);

/**
 * @brief  Set a color temperature preset
 * @param  Copy_Preset: The preset to apply
 * @param  Copy_Brightness: Brightness percentage (0-100)
 * @retval SmartLight_Status_t: Status of operation
 */
SmartLight_Status_t SmartLight_SetPreset(SmartLight_Preset_t Copy_Preset, uint32_t Copy_Brightness);

/**
 * @brief  Turn off all LED channels
 * @retval SmartLight_Status_t: Status of operation
 */
SmartLight_Status_t SmartLight_TurnOff(void);

/*******************************************************************************************************/
/* Function Prototypes - Automatic Control                                                             */
/*******************************************************************************************************/

/**
 * @brief  Update automatic lighting based on sensor readings
 * @note   Call this periodically when in automatic mode
 * @retval SmartLight_Status_t: Status of operation
 */
SmartLight_Status_t SmartLight_UpdateAutomatic(void);

/**
 * @brief  Periodic task for automatic lighting control
 * @note   Should be called from main loop. Handles timing internally.
 */
void SmartLight_AutoTask(void);

/*******************************************************************************************************/
/* Function Prototypes - Status and Information                                                        */
/*******************************************************************************************************/

/**
 * @brief  Get the current light state
 * @param  Copy_pState: Pointer to store the current state
 * @retval SmartLight_Status_t: Status of operation
 */
SmartLight_Status_t SmartLight_GetState(SmartLight_State_t* Copy_pState);

/**
 * @brief  Get current channel values
 * @param  Copy_pChannels: Pointer to store channel values
 * @retval SmartLight_Status_t: Status of operation
 */
SmartLight_Status_t SmartLight_GetChannels(SmartLight_Channels_t* Copy_pChannels);

/*******************************************************************************************************/
/* Function Prototypes - Algorithm Helpers (exposed for testing/customization)                         */
/*******************************************************************************************************/

/**
 * @brief  Calculate color temperature mix based on ambient conditions
 * @param  Copy_Lux: Ambient light level in lux
 * @param  Copy_TempC100: Temperature in 0.01C units (e.g., 2500 = 25.00C)
 * @param  Copy_pWarm: Pointer to store warm percentage (0-100)
 * @param  Copy_pNeutral: Pointer to store neutral percentage (0-100)
 * @param  Copy_pCool: Pointer to store cool percentage (0-100)
 */
void SmartLight_CalculateAutoColor(uint32_t Copy_Lux, int32_t Copy_TempC100,
                                    uint32_t* Copy_pWarm, uint32_t* Copy_pNeutral, uint32_t* Copy_pCool);

/**
 * @brief  Calculate brightness based on ambient light level
 * @param  Copy_Lux: Ambient light level in lux
 * @retval uint32_t: Recommended brightness percentage (0-100)
 */
uint32_t SmartLight_CalculateAutoBrightness(uint32_t Copy_Lux);

#endif /* SMART_LIGHT_H_ */
