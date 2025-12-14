/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 12 Dec 2025                                                                     */
/* Description       : tsl2561.c --> TSL2561 Light Sensor Implementation File                          */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/
#include "tsl2561.h"
#include "i2c.h"
#include "stk.h"
#include <stdio.h>

/*******************************************************************************************************/
/* Global Variables                                                                                    */
/*******************************************************************************************************/
static TSL2561_Device_t g_TSL2561_Device = {0};

/*******************************************************************************************************/
/* Lux Calculation Constants (from TSL2561 datasheet)                                                  */
/*******************************************************************************************************/

/* Scale factors for different integration times */
#define TSL2561_LUX_SCALE           14      /* Scale by 2^14 */
#define TSL2561_RATIO_SCALE         9       /* Scale ratio by 2^9 */

/* Integration time scaling factors */
#define TSL2561_CH_SCALE            10      /* Scale channel values by 2^10 */
#define TSL2561_CHSCALE_TINT0       0x7517  /* 322/11 * 2^TSL2561_CH_SCALE for 13.7ms */
#define TSL2561_CHSCALE_TINT1       0x0FE7  /* 322/81 * 2^TSL2561_CH_SCALE for 101ms */

/* T, FN, and CL package coefficients */
#define TSL2561_K1T                 0x0040  /* 0.125 * 2^RATIO_SCALE */
#define TSL2561_B1T                 0x01F2  /* 0.0304 * 2^LUX_SCALE */
#define TSL2561_M1T                 0x01BE  /* 0.0272 * 2^LUX_SCALE */
#define TSL2561_K2T                 0x0080  /* 0.250 * 2^RATIO_SCALE */
#define TSL2561_B2T                 0x0214  /* 0.0325 * 2^LUX_SCALE */
#define TSL2561_M2T                 0x02D1  /* 0.0440 * 2^LUX_SCALE */
#define TSL2561_K3T                 0x00C0  /* 0.375 * 2^RATIO_SCALE */
#define TSL2561_B3T                 0x023F  /* 0.0351 * 2^LUX_SCALE */
#define TSL2561_M3T                 0x037B  /* 0.0544 * 2^LUX_SCALE */
#define TSL2561_K4T                 0x0100  /* 0.50 * 2^RATIO_SCALE */
#define TSL2561_B4T                 0x0270  /* 0.0381 * 2^LUX_SCALE */
#define TSL2561_M4T                 0x03FE  /* 0.0624 * 2^LUX_SCALE */
#define TSL2561_K5T                 0x0138  /* 0.61 * 2^RATIO_SCALE */
#define TSL2561_B5T                 0x016F  /* 0.0224 * 2^LUX_SCALE */
#define TSL2561_M5T                 0x01FC  /* 0.0310 * 2^LUX_SCALE */
#define TSL2561_K6T                 0x019A  /* 0.80 * 2^RATIO_SCALE */
#define TSL2561_B6T                 0x00D2  /* 0.0128 * 2^LUX_SCALE */
#define TSL2561_M6T                 0x00FB  /* 0.0153 * 2^LUX_SCALE */
#define TSL2561_K7T                 0x029A  /* 1.3 * 2^RATIO_SCALE */
#define TSL2561_B7T                 0x0018  /* 0.00146 * 2^LUX_SCALE */
#define TSL2561_M7T                 0x0012  /* 0.00112 * 2^LUX_SCALE */
#define TSL2561_K8T                 0x029A  /* 1.3 * 2^RATIO_SCALE */
#define TSL2561_B8T                 0x0000  /* 0.000 * 2^LUX_SCALE */
#define TSL2561_M8T                 0x0000  /* 0.000 * 2^LUX_SCALE */

/* CS package coefficients */
#define TSL2561_K1C                 0x0043  /* 0.130 * 2^RATIO_SCALE */
#define TSL2561_B1C                 0x0204  /* 0.0315 * 2^LUX_SCALE */
#define TSL2561_M1C                 0x01AD  /* 0.0262 * 2^LUX_SCALE */
#define TSL2561_K2C                 0x0085  /* 0.260 * 2^RATIO_SCALE */
#define TSL2561_B2C                 0x0228  /* 0.0337 * 2^LUX_SCALE */
#define TSL2561_M2C                 0x02C1  /* 0.0430 * 2^LUX_SCALE */
#define TSL2561_K3C                 0x00C8  /* 0.390 * 2^RATIO_SCALE */
#define TSL2561_B3C                 0x0253  /* 0.0363 * 2^LUX_SCALE */
#define TSL2561_M3C                 0x0363  /* 0.0529 * 2^LUX_SCALE */
#define TSL2561_K4C                 0x010A  /* 0.520 * 2^RATIO_SCALE */
#define TSL2561_B4C                 0x0282  /* 0.0392 * 2^LUX_SCALE */
#define TSL2561_M4C                 0x03DF  /* 0.0605 * 2^LUX_SCALE */
#define TSL2561_K5C                 0x014D  /* 0.65 * 2^RATIO_SCALE */
#define TSL2561_B5C                 0x0177  /* 0.0229 * 2^LUX_SCALE */
#define TSL2561_M5C                 0x01DD  /* 0.0291 * 2^LUX_SCALE */
#define TSL2561_K6C                 0x019A  /* 0.80 * 2^RATIO_SCALE */
#define TSL2561_B6C                 0x0101  /* 0.0157 * 2^LUX_SCALE */
#define TSL2561_M6C                 0x0127  /* 0.0180 * 2^LUX_SCALE */
#define TSL2561_K7C                 0x029A  /* 1.3 * 2^RATIO_SCALE */
#define TSL2561_B7C                 0x0037  /* 0.00338 * 2^LUX_SCALE */
#define TSL2561_M7C                 0x002B  /* 0.00260 * 2^LUX_SCALE */
#define TSL2561_K8C                 0x029A  /* 1.3 * 2^RATIO_SCALE */
#define TSL2561_B8C                 0x0000  /* 0.000 * 2^LUX_SCALE */
#define TSL2561_M8C                 0x0000  /* 0.000 * 2^LUX_SCALE */

/*******************************************************************************************************/
/* Internal Helper Functions (Register Access)                                                         */
/*******************************************************************************************************/
static TSL2561_Status_t TSL2561_WriteRegister(uint8_t Copy_u8RegAddr, uint8_t Copy_u8Value)
{
    uint8_t data[2];
    data[0] = TSL2561_COMMAND_BIT | Copy_u8RegAddr;
    data[1] = Copy_u8Value;

    if (!g_TSL2561_Device.is_initialized) {
        return TSL2561_STATUS_NOT_INITIALIZED;
    }

    if (I2C_Master_Transmit(g_TSL2561_Device.i2c_instance, g_TSL2561_Device.i2c_address,
                            data, 2, TSL2561_I2C_TIMEOUT) != I2C_STATUS_OK) {
        return TSL2561_STATUS_COMM_ERROR;
    }
    return TSL2561_STATUS_OK;
}

static TSL2561_Status_t TSL2561_ReadRegister(uint8_t Copy_u8RegAddr, uint8_t* Copy_pu8Value)
{
    uint8_t cmd = TSL2561_COMMAND_BIT | Copy_u8RegAddr;

    if (!g_TSL2561_Device.is_initialized) {
        return TSL2561_STATUS_NOT_INITIALIZED;
    }
    if (Copy_pu8Value == NULL) {
        return TSL2561_STATUS_INVALID_PARAM;
    }

    if (I2C_Master_Transmit(g_TSL2561_Device.i2c_instance, g_TSL2561_Device.i2c_address,
                            &cmd, 1, TSL2561_I2C_TIMEOUT) != I2C_STATUS_OK) {
        return TSL2561_STATUS_COMM_ERROR;
    }
    if (I2C_Master_Receive(g_TSL2561_Device.i2c_instance, g_TSL2561_Device.i2c_address,
                           Copy_pu8Value, 1, TSL2561_I2C_TIMEOUT) != I2C_STATUS_OK) {
        return TSL2561_STATUS_COMM_ERROR;
    }
    return TSL2561_STATUS_OK;
}

static TSL2561_Status_t TSL2561_ReadWord(uint8_t Copy_u8RegAddr, uint16_t* Copy_pu16Value)
{
    uint8_t cmd = TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | Copy_u8RegAddr;
    uint8_t data[2];

    if (!g_TSL2561_Device.is_initialized) {
        return TSL2561_STATUS_NOT_INITIALIZED;
    }
    if (Copy_pu16Value == NULL) {
        return TSL2561_STATUS_INVALID_PARAM;
    }

    if (I2C_Master_Transmit(g_TSL2561_Device.i2c_instance, g_TSL2561_Device.i2c_address,
                            &cmd, 1, TSL2561_I2C_TIMEOUT) != I2C_STATUS_OK) {
        return TSL2561_STATUS_COMM_ERROR;
    }
    if (I2C_Master_Receive(g_TSL2561_Device.i2c_instance, g_TSL2561_Device.i2c_address,
                           data, 2, TSL2561_I2C_TIMEOUT) != I2C_STATUS_OK) {
        return TSL2561_STATUS_COMM_ERROR;
    }

    *Copy_pu16Value = ((uint16_t)data[1] << 8) | data[0];
    return TSL2561_STATUS_OK;
}

/*******************************************************************************************************/
/* Internal Helper Functions (Lux Calculation)                                                         */
/*******************************************************************************************************/
static uint32_t TSL2561_CalculateLux(uint16_t Copy_u16Channel0, uint16_t Copy_u16Channel1)
{
    uint32_t chScale;
    uint32_t channel0, channel1;
    uint32_t ratio, ratio1;
    uint32_t b, m;
    int32_t temp;
    uint32_t lux;

    /* Scale based on integration time */
    switch (g_TSL2561_Device.integration_time) {
        case TSL2561_INTEGRATION_13_7MS:
            chScale = TSL2561_CHSCALE_TINT0;
            break;
        case TSL2561_INTEGRATION_101MS:
            chScale = TSL2561_CHSCALE_TINT1;
            break;
        default:  /* TSL2561_INTEGRATION_402MS */
            chScale = (1 << TSL2561_CH_SCALE);
            break;
    }

    /* Scale for gain (16x) */
    if (g_TSL2561_Device.gain == TSL2561_GAIN_1X) {
        chScale = chScale << 4;  /* Scale 1x to 16x */
    }

    /* Scale the channel values */
    channel0 = (Copy_u16Channel0 * chScale) >> TSL2561_CH_SCALE;
    channel1 = (Copy_u16Channel1 * chScale) >> TSL2561_CH_SCALE;

    /* Find the ratio of the channel values (Channel1/Channel0) */
    ratio1 = 0;
    if (channel0 != 0) {
        ratio1 = (channel1 << (TSL2561_RATIO_SCALE + 1)) / channel0;
    }

    /* Round the ratio value */
    ratio = (ratio1 + 1) >> 1;

    /* Determine coefficients based on package type and ratio */
    if (g_TSL2561_Device.package == TSL2561_PACKAGE_CS) {
        /* CS package */
        if (ratio <= TSL2561_K1C) {
            b = TSL2561_B1C; m = TSL2561_M1C;
        } else if (ratio <= TSL2561_K2C) {
            b = TSL2561_B2C; m = TSL2561_M2C;
        } else if (ratio <= TSL2561_K3C) {
            b = TSL2561_B3C; m = TSL2561_M3C;
        } else if (ratio <= TSL2561_K4C) {
            b = TSL2561_B4C; m = TSL2561_M4C;
        } else if (ratio <= TSL2561_K5C) {
            b = TSL2561_B5C; m = TSL2561_M5C;
        } else if (ratio <= TSL2561_K6C) {
            b = TSL2561_B6C; m = TSL2561_M6C;
        } else if (ratio <= TSL2561_K7C) {
            b = TSL2561_B7C; m = TSL2561_M7C;
        } else {
            b = TSL2561_B8C; m = TSL2561_M8C;
        }
    } else {
        /* T, FN, or CL package */
        if (ratio <= TSL2561_K1T) {
            b = TSL2561_B1T; m = TSL2561_M1T;
        } else if (ratio <= TSL2561_K2T) {
            b = TSL2561_B2T; m = TSL2561_M2T;
        } else if (ratio <= TSL2561_K3T) {
            b = TSL2561_B3T; m = TSL2561_M3T;
        } else if (ratio <= TSL2561_K4T) {
            b = TSL2561_B4T; m = TSL2561_M4T;
        } else if (ratio <= TSL2561_K5T) {
            b = TSL2561_B5T; m = TSL2561_M5T;
        } else if (ratio <= TSL2561_K6T) {
            b = TSL2561_B6T; m = TSL2561_M6T;
        } else if (ratio <= TSL2561_K7T) {
            b = TSL2561_B7T; m = TSL2561_M7T;
        } else {
            b = TSL2561_B8T; m = TSL2561_M8T;
        }
    }

    /* Calculate lux */
    temp = ((channel0 * b) - (channel1 * m));

    /* Do not allow negative lux value */
    if (temp < 0) {
        temp = 0;
    }

    /* Round lsb (2^(LUX_SCALE-1)) */
    temp += (1 << (TSL2561_LUX_SCALE - 1));

    /* Strip off fractional portion */
    lux = temp >> TSL2561_LUX_SCALE;

    return lux;
}

/*******************************************************************************************************/
/* Public Function Implementations                                                                     */
/*******************************************************************************************************/
TSL2561_Status_t TSL2561_Initialize(I2C_Instance_t Copy_I2CInstance, uint8_t Copy_u8DeviceAddress)
{
    uint8_t chip_id;

    g_TSL2561_Device.i2c_instance = Copy_I2CInstance;
    g_TSL2561_Device.i2c_address = Copy_u8DeviceAddress;
    g_TSL2561_Device.is_initialized = 1;
    g_TSL2561_Device.is_powered = 0;
    g_TSL2561_Device.integration_time = TSL2561_DEFAULT_INTEGRATION;
    g_TSL2561_Device.gain = TSL2561_DEFAULT_GAIN;
    g_TSL2561_Device.package = TSL2561_PACKAGE_T_FN_CL;

    STK_DelayMs(TSL2561_STARTUP_DELAY_MS);

    /* Power on to read chip ID */
    if (TSL2561_PowerOn() != TSL2561_STATUS_OK) {
        printf("TSL2561: Failed to power on\r\n");
        g_TSL2561_Device.is_initialized = 0;
        return TSL2561_STATUS_ERROR;
    }

    /* Verify chip ID */
    if (TSL2561_GetChipID(&chip_id) != TSL2561_STATUS_OK) {
        printf("TSL2561: Failed to read chip ID\r\n");
        g_TSL2561_Device.is_initialized = 0;
        return TSL2561_STATUS_COMM_ERROR;
    }

    if ((chip_id & TSL2561_CHIP_ID_PARTNO_MASK) != TSL2561_CHIP_ID_TSL2561) {
        printf("TSL2561: Invalid chip ID: 0x%02X (expected 0x5X)\r\n", chip_id);
        g_TSL2561_Device.is_initialized = 0;
        return TSL2561_STATUS_ERROR;
    }

    printf("TSL2561: Chip ID: 0x%02X - OK\r\n", chip_id);

    /* Apply default configuration */
    TSL2561_Config_t default_config = {
        .integration_time = TSL2561_DEFAULT_INTEGRATION,
        .gain = TSL2561_DEFAULT_GAIN,
        .package = TSL2561_PACKAGE_T_FN_CL
    };

    if (TSL2561_Configure(&default_config) != TSL2561_STATUS_OK) {
        printf("TSL2561: Failed to configure\r\n");
        g_TSL2561_Device.is_initialized = 0;
        return TSL2561_STATUS_ERROR;
    }

    printf("TSL2561: Initialized successfully\r\n");
    return TSL2561_STATUS_OK;
}

TSL2561_Status_t TSL2561_Configure(TSL2561_Config_t* Copy_pConfig)
{
    uint8_t timing_reg;

    if (Copy_pConfig == NULL) {
        return TSL2561_STATUS_INVALID_PARAM;
    }

    g_TSL2561_Device.integration_time = Copy_pConfig->integration_time;
    g_TSL2561_Device.gain = Copy_pConfig->gain;
    g_TSL2561_Device.package = Copy_pConfig->package;

    /* Write timing register (integration time + gain) */
    timing_reg = (uint8_t)Copy_pConfig->integration_time | (uint8_t)Copy_pConfig->gain;

    return TSL2561_WriteRegister(TSL2561_REG_TIMING, timing_reg);
}

TSL2561_Status_t TSL2561_SetIntegrationTime(TSL2561_Integration_t Copy_Integration)
{
    uint8_t timing_reg;

    g_TSL2561_Device.integration_time = Copy_Integration;
    timing_reg = (uint8_t)Copy_Integration | (uint8_t)g_TSL2561_Device.gain;

    return TSL2561_WriteRegister(TSL2561_REG_TIMING, timing_reg);
}

TSL2561_Status_t TSL2561_SetGain(TSL2561_Gain_t Copy_Gain)
{
    uint8_t timing_reg;

    g_TSL2561_Device.gain = Copy_Gain;
    timing_reg = (uint8_t)g_TSL2561_Device.integration_time | (uint8_t)Copy_Gain;

    return TSL2561_WriteRegister(TSL2561_REG_TIMING, timing_reg);
}

TSL2561_Status_t TSL2561_PowerOn(void)
{
    TSL2561_Status_t status;

    status = TSL2561_WriteRegister(TSL2561_REG_CONTROL, TSL2561_CONTROL_POWERON);
    if (status == TSL2561_STATUS_OK) {
        g_TSL2561_Device.is_powered = 1;
        STK_DelayMs(TSL2561_POWERUP_DELAY_MS);
    }

    return status;
}

TSL2561_Status_t TSL2561_PowerOff(void)
{
    TSL2561_Status_t status;

    status = TSL2561_WriteRegister(TSL2561_REG_CONTROL, TSL2561_CONTROL_POWEROFF);
    if (status == TSL2561_STATUS_OK) {
        g_TSL2561_Device.is_powered = 0;
    }

    return status;
}

TSL2561_Status_t TSL2561_GetChipID(uint8_t* Copy_pu8ChipID)
{
    if (Copy_pu8ChipID == NULL) {
        return TSL2561_STATUS_INVALID_PARAM;
    }
    return TSL2561_ReadRegister(TSL2561_REG_ID, Copy_pu8ChipID);
}

TSL2561_Status_t TSL2561_ReadRawData(uint16_t* Copy_pu16Channel0, uint16_t* Copy_pu16Channel1)
{
    TSL2561_Status_t status;

    if (Copy_pu16Channel0 == NULL || Copy_pu16Channel1 == NULL) {
        return TSL2561_STATUS_INVALID_PARAM;
    }

    if (!g_TSL2561_Device.is_powered) {
        return TSL2561_STATUS_NOT_INITIALIZED;
    }

    /* Read channel 0 (broadband) */
    status = TSL2561_ReadWord(TSL2561_REG_DATA0LOW, Copy_pu16Channel0);
    if (status != TSL2561_STATUS_OK) {
        return status;
    }

    /* Read channel 1 (infrared) */
    status = TSL2561_ReadWord(TSL2561_REG_DATA1LOW, Copy_pu16Channel1);
    if (status != TSL2561_STATUS_OK) {
        return status;
    }

    return TSL2561_STATUS_OK;
}

TSL2561_Status_t TSL2561_ReadSensorData(TSL2561_Data_t* Copy_pData)
{
    TSL2561_Status_t status;
    uint16_t channel0, channel1;

    if (Copy_pData == NULL) {
        return TSL2561_STATUS_INVALID_PARAM;
    }

    /* Read raw ADC values */
    status = TSL2561_ReadRawData(&channel0, &channel1);
    if (status != TSL2561_STATUS_OK) {
        return status;
    }

    Copy_pData->channel0 = channel0;
    Copy_pData->channel1 = channel1;
    Copy_pData->timestamp = STK_GetMillis();

    /* Check for overflow */
    if ((channel0 == 0xFFFF) || (channel1 == 0xFFFF)) {
        Copy_pData->lux = 0;
        return TSL2561_STATUS_OVERFLOW;
    }

    /* Calculate lux value */
    Copy_pData->lux = TSL2561_CalculateLux(channel0, channel1);

    return TSL2561_STATUS_OK;
}

TSL2561_Status_t TSL2561_ReadLux(uint32_t* Copy_pu32Lux)
{
    TSL2561_Data_t data;
    TSL2561_Status_t status;

    if (Copy_pu32Lux == NULL) {
        return TSL2561_STATUS_INVALID_PARAM;
    }

    status = TSL2561_ReadSensorData(&data);
    if (status == TSL2561_STATUS_OK || status == TSL2561_STATUS_OVERFLOW) {
        *Copy_pu32Lux = data.lux;
    }

    return status;
}

TSL2561_Status_t TSL2561_GetConfig(TSL2561_Config_t* Copy_pConfig)
{
    uint8_t timing_reg;
    TSL2561_Status_t status;

    if (Copy_pConfig == NULL) {
        return TSL2561_STATUS_INVALID_PARAM;
    }

    status = TSL2561_ReadRegister(TSL2561_REG_TIMING, &timing_reg);
    if (status != TSL2561_STATUS_OK) {
        return status;
    }

    Copy_pConfig->integration_time = (TSL2561_Integration_t)(timing_reg & 0x03);
    Copy_pConfig->gain = (TSL2561_Gain_t)(timing_reg & 0x10);
    Copy_pConfig->package = g_TSL2561_Device.package;

    return TSL2561_STATUS_OK;
}

uint8_t TSL2561_IsInitialized(void)
{
    return g_TSL2561_Device.is_initialized;
}
