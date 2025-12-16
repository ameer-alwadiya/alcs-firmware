/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 15 Dec 2025                                                                     */
/* Description       : smart_light.c --> Smart Lighting Driver Implementation                          */
/*                     Three-channel color temperature LED control with automatic mode                 */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/
#include "smart_light.h"
#include "stk.h"

/*******************************************************************************************************/
/* Private Variables                                                                                   */
/*******************************************************************************************************/
static SmartLight_State_t g_LightState = {
    .mode = SMART_LIGHT_MODE_MANUAL,
    .channels = { .warm = 0, .neutral = 0, .cool = 0 },
    .brightness = 50,
    .last_lux = 0,
    .last_temp = 2500,  /* Default 25.00C */
    .last_update_time = 0
};

static uint8_t g_Initialized = 0;

/*******************************************************************************************************/
/* Private Function Prototypes                                                                         */
/*******************************************************************************************************/
static void SmartLight_ApplyChannels(SmartLight_Channels_t* Copy_pChannels);
static uint32_t SmartLight_Clamp(uint32_t Copy_Value, uint32_t Copy_Min, uint32_t Copy_Max);

/*******************************************************************************************************/
/* Private Functions                                                                                   */
/*******************************************************************************************************/

/**
 * @brief  Clamp a value between min and max
 */
static uint32_t SmartLight_Clamp(uint32_t Copy_Value, uint32_t Copy_Min, uint32_t Copy_Max)
{
    if (Copy_Value < Copy_Min) return Copy_Min;
    if (Copy_Value > Copy_Max) return Copy_Max;
    return Copy_Value;
}

/**
 * @brief  Apply PWM values to all three LED channels
 */
static void SmartLight_ApplyChannels(SmartLight_Channels_t* Copy_pChannels)
{
    TIM_PWM_SetDutyCycle(SMART_LIGHT_TIMER, SMART_LIGHT_WARM_CHANNEL, Copy_pChannels->warm);
    TIM_PWM_SetDutyCycle(SMART_LIGHT_TIMER, SMART_LIGHT_NEUTRAL_CHANNEL, Copy_pChannels->neutral);
    TIM_PWM_SetDutyCycle(SMART_LIGHT_TIMER, SMART_LIGHT_COOL_CHANNEL, Copy_pChannels->cool);
}

/*******************************************************************************************************/
/* Initialization Functions                                                                            */
/*******************************************************************************************************/

SmartLight_Status_t SmartLight_Initialize(void)
{
    /* Initialize all three PWM channels */
    TIM_PWM_Init(SMART_LIGHT_TIMER, SMART_LIGHT_WARM_CHANNEL,
                 SMART_LIGHT_PRESCALER, SMART_LIGHT_PERIOD);
    TIM_PWM_Init(SMART_LIGHT_TIMER, SMART_LIGHT_NEUTRAL_CHANNEL,
                 SMART_LIGHT_PRESCALER, SMART_LIGHT_PERIOD);
    TIM_PWM_Init(SMART_LIGHT_TIMER, SMART_LIGHT_COOL_CHANNEL,
                 SMART_LIGHT_PRESCALER, SMART_LIGHT_PERIOD);

    /* Set all channels to off initially */
    g_LightState.channels.warm = 0;
    g_LightState.channels.neutral = 0;
    g_LightState.channels.cool = 0;
    SmartLight_ApplyChannels(&g_LightState.channels);

    /* Initialize state */
    g_LightState.mode = SMART_LIGHT_MODE_MANUAL;
    g_LightState.brightness = 50;
    g_LightState.last_lux = 0;
    g_LightState.last_temp = 2500;
    g_LightState.last_update_time = STK_GetMillis();

    g_Initialized = 1;

    return SMART_LIGHT_STATUS_OK;
}

/*******************************************************************************************************/
/* Mode Control Functions                                                                              */
/*******************************************************************************************************/

SmartLight_Status_t SmartLight_SetMode(SmartLight_Mode_t Copy_Mode)
{
    if (!g_Initialized) {
        return SMART_LIGHT_STATUS_NOT_INITIALIZED;
    }

    if (Copy_Mode != SMART_LIGHT_MODE_MANUAL && Copy_Mode != SMART_LIGHT_MODE_AUTOMATIC) {
        return SMART_LIGHT_STATUS_INVALID_PARAM;
    }

    g_LightState.mode = Copy_Mode;

    /* If switching to automatic, trigger an immediate update */
    if (Copy_Mode == SMART_LIGHT_MODE_AUTOMATIC) {
        SmartLight_UpdateAutomatic();
    }

    return SMART_LIGHT_STATUS_OK;
}

SmartLight_Status_t SmartLight_GetMode(SmartLight_Mode_t* Copy_pMode)
{
    if (!g_Initialized) {
        return SMART_LIGHT_STATUS_NOT_INITIALIZED;
    }

    if (Copy_pMode == NULL) {
        return SMART_LIGHT_STATUS_INVALID_PARAM;
    }

    *Copy_pMode = g_LightState.mode;
    return SMART_LIGHT_STATUS_OK;
}

/*******************************************************************************************************/
/* Manual Control Functions                                                                            */
/*******************************************************************************************************/

SmartLight_Status_t SmartLight_SetChannels(uint32_t Copy_Warm, uint32_t Copy_Neutral, uint32_t Copy_Cool)
{
    if (!g_Initialized) {
        return SMART_LIGHT_STATUS_NOT_INITIALIZED;
    }

    /* Switch to manual mode */
    g_LightState.mode = SMART_LIGHT_MODE_MANUAL;

    /* Clamp and set channel values */
    g_LightState.channels.warm = SmartLight_Clamp(Copy_Warm, SMART_LIGHT_MIN_DUTY, SMART_LIGHT_MAX_DUTY);
    g_LightState.channels.neutral = SmartLight_Clamp(Copy_Neutral, SMART_LIGHT_MIN_DUTY, SMART_LIGHT_MAX_DUTY);
    g_LightState.channels.cool = SmartLight_Clamp(Copy_Cool, SMART_LIGHT_MIN_DUTY, SMART_LIGHT_MAX_DUTY);

    /* Apply to hardware */
    SmartLight_ApplyChannels(&g_LightState.channels);

    return SMART_LIGHT_STATUS_OK;
}

SmartLight_Status_t SmartLight_SetPreset(SmartLight_Preset_t Copy_Preset, uint32_t Copy_Brightness)
{
    if (!g_Initialized) {
        return SMART_LIGHT_STATUS_NOT_INITIALIZED;
    }

    if (Copy_Brightness > 100) {
        return SMART_LIGHT_STATUS_INVALID_PARAM;
    }

    /* Calculate duty from brightness percentage */
    uint32_t duty = (Copy_Brightness * SMART_LIGHT_MAX_DUTY) / 100;
    uint32_t warm = 0, neutral = 0, cool = 0;

    switch (Copy_Preset) {
        case SMART_LIGHT_PRESET_WARM:
            warm = duty;
            break;
        case SMART_LIGHT_PRESET_NEUTRAL:
            neutral = duty;
            break;
        case SMART_LIGHT_PRESET_COOL:
            cool = duty;
            break;
        case SMART_LIGHT_PRESET_MIXED:
            warm = duty / 3;
            neutral = duty / 3;
            cool = duty / 3;
            break;
        case SMART_LIGHT_PRESET_OFF:
        default:
            warm = 0;
            neutral = 0;
            cool = 0;
            break;
    }

    return SmartLight_SetChannels(warm, neutral, cool);
}

SmartLight_Status_t SmartLight_TurnOff(void)
{
    return SmartLight_SetChannels(0, 0, 0);
}

/*******************************************************************************************************/
/* Automatic Control Functions                                                                         */
/*******************************************************************************************************/

void SmartLight_CalculateAutoColor(uint32_t Copy_Lux, int32_t Copy_TempC100,
                                    uint32_t* Copy_pWarm, uint32_t* Copy_pNeutral, uint32_t* Copy_pCool)
{
    /* Base color mix from ambient light */
    uint32_t w = 0, n = 0, c = 0;

    if (Copy_Lux < SMART_LIGHT_LUX_VERY_DARK) {
        /* Very dark - full warm for comfort */
        w = 100; n = 0; c = 0;
    } else if (Copy_Lux < SMART_LIGHT_LUX_DIM) {
        /* Dim evening - mostly warm */
        w = 80; n = 20; c = 0;
    } else if (Copy_Lux < SMART_LIGHT_LUX_TWILIGHT) {
        /* Twilight - balanced warm/neutral */
        w = 50; n = 50; c = 0;
    } else if (Copy_Lux < SMART_LIGHT_LUX_DUSK) {
        /* Dusk/dawn - mix of all */
        w = 30; n = 50; c = 20;
    } else if (Copy_Lux < SMART_LIGHT_LUX_INDOOR) {
        /* Indoor daylight - neutral dominant */
        w = 10; n = 50; c = 40;
    } else if (Copy_Lux < SMART_LIGHT_LUX_BRIGHT) {
        /* Bright daylight - cooler */
        w = 0; n = 30; c = 70;
    } else {
        /* Very bright - mostly cool */
        w = 0; n = 10; c = 90;
    }

    /* Adjust based on temperature (Copy_TempC100 is in 0.01C units) */
    int32_t temp_c = Copy_TempC100 / 100;

    if (temp_c > SMART_LIGHT_TEMP_HOT) {
        /* Hot environment - shift towards cooler colors */
        uint32_t shift = (temp_c > 35) ? 30 : ((uint32_t)(temp_c - SMART_LIGHT_TEMP_HOT) * 4);
        if (w > shift) w -= shift; else w = 0;
        c += shift;
        if (c > 100) c = 100;
    } else if (temp_c < SMART_LIGHT_TEMP_COLD) {
        /* Cold environment - shift towards warmer colors */
        uint32_t shift = (temp_c < 10) ? 30 : ((uint32_t)(SMART_LIGHT_TEMP_COLD - temp_c) * 4);
        if (c > shift) c -= shift; else c = 0;
        w += shift;
        if (w > 100) w = 100;
    }

    /* Normalize to ensure total is 100% */
    uint32_t total = w + n + c;
    if (total > 0) {
        *Copy_pWarm = (w * 100) / total;
        *Copy_pNeutral = (n * 100) / total;
        *Copy_pCool = (c * 100) / total;
    } else {
        *Copy_pWarm = 33;
        *Copy_pNeutral = 34;
        *Copy_pCool = 33;
    }
}

uint32_t SmartLight_CalculateAutoBrightness(uint32_t Copy_Lux)
{
    if (Copy_Lux < 10) {
        return 30;   /* Very dark - moderate brightness to not blind */
    } else if (Copy_Lux < 50) {
        return 50;   /* Dark - medium brightness */
    } else if (Copy_Lux < 200) {
        return 70;   /* Dim - higher brightness */
    } else if (Copy_Lux < 500) {
        return 85;   /* Twilight - near full */
    } else if (Copy_Lux < 1000) {
        return 95;   /* Dusk - almost full */
    } else {
        return 100;  /* Bright ambient - full brightness */
    }
}

SmartLight_Status_t SmartLight_UpdateAutomatic(void)
{
    if (!g_Initialized) {
        return SMART_LIGHT_STATUS_NOT_INITIALIZED;
    }

    if (g_LightState.mode != SMART_LIGHT_MODE_AUTOMATIC) {
        return SMART_LIGHT_STATUS_OK;  /* Not an error, just not in auto mode */
    }

    TSL2561_Data_t tsl_data;
    BMP280_Data_t bmp_data;
    uint32_t lux = g_LightState.last_lux;
    int32_t temp = g_LightState.last_temp;

    /* Read light sensor */
    TSL2561_Status_t tsl_status = TSL2561_ReadSensorData(&tsl_data);
    if (tsl_status == TSL2561_STATUS_OK || tsl_status == TSL2561_STATUS_OVERFLOW) {
        lux = tsl_data.lux;
        g_LightState.last_lux = lux;
    }

    /* Read temperature sensor */
    if (BMP280_ReadSensorData(&bmp_data) == BMP280_STATUS_OK) {
        temp = bmp_data.temperature;
        g_LightState.last_temp = temp;
    }

    /* Calculate color mix */
    uint32_t warm_pct, neutral_pct, cool_pct;
    SmartLight_CalculateAutoColor(lux, temp, &warm_pct, &neutral_pct, &cool_pct);

    /* Calculate brightness */
    uint32_t brightness = SmartLight_CalculateAutoBrightness(lux);
    g_LightState.brightness = brightness;

    /* Apply brightness scaling to color percentages */
    /* Each channel gets: (color_percent * brightness * 10) / 100 */
    /* Result is 0-1000 duty cycle */
    g_LightState.channels.warm = (warm_pct * brightness * 10) / 100;
    g_LightState.channels.neutral = (neutral_pct * brightness * 10) / 100;
    g_LightState.channels.cool = (cool_pct * brightness * 10) / 100;

    /* Apply to hardware */
    SmartLight_ApplyChannels(&g_LightState.channels);

    /* Update timestamp */
    g_LightState.last_update_time = STK_GetMillis();

    return SMART_LIGHT_STATUS_OK;
}

void SmartLight_AutoTask(void)
{
    static uint32_t last_auto_update = 0;

    if (!g_Initialized) {
        return;
    }

    if (g_LightState.mode != SMART_LIGHT_MODE_AUTOMATIC) {
        return;
    }

    if (STK_DelayElapsed(&last_auto_update, SMART_LIGHT_AUTO_UPDATE_MS)) {
        SmartLight_UpdateAutomatic();
    }
}

/*******************************************************************************************************/
/* Status and Information Functions                                                                    */
/*******************************************************************************************************/

SmartLight_Status_t SmartLight_GetState(SmartLight_State_t* Copy_pState)
{
    if (!g_Initialized) {
        return SMART_LIGHT_STATUS_NOT_INITIALIZED;
    }

    if (Copy_pState == NULL) {
        return SMART_LIGHT_STATUS_INVALID_PARAM;
    }

    *Copy_pState = g_LightState;
    return SMART_LIGHT_STATUS_OK;
}

SmartLight_Status_t SmartLight_GetChannels(SmartLight_Channels_t* Copy_pChannels)
{
    if (!g_Initialized) {
        return SMART_LIGHT_STATUS_NOT_INITIALIZED;
    }

    if (Copy_pChannels == NULL) {
        return SMART_LIGHT_STATUS_INVALID_PARAM;
    }

    *Copy_pChannels = g_LightState.channels;
    return SMART_LIGHT_STATUS_OK;
}
