/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.1.0                                                                          */
/* Date              : 03 Oct 2025                                                                     */
/* Description       : bmp280_program.c --> BMP280 Implementation File                                 */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/
#include "bmp280_private.h"
#include "bmp280_config.h"
#include "bmp280_interface.h"
#include "i2c_interface.h"
#include "stk_interface.h"
#include <stdio.h>

/*******************************************************************************************************/
/* Global Variables                                                                                    */
/*******************************************************************************************************/
static BMP280_Device_t g_BMP280_Device = {0};

/*******************************************************************************************************/
/* Internal Helper Functions (Register Access)                                                         */
/*******************************************************************************************************/
static BMP280_Status_t BMP280_WriteRegister(uint8_t Copy_u8RegAddr, uint8_t Copy_u8Value)
{
    uint8_t data[2] = {Copy_u8RegAddr, Copy_u8Value};
    if (!g_BMP280_Device.is_initialized) return BMP280_STATUS_NOT_INITIALIZED;
    if (I2C_Master_Transmit(g_BMP280_Device.i2c_instance, g_BMP280_Device.i2c_address, data, 2, BMP280_I2C_TIMEOUT) != I2C_STATUS_OK) {
        return BMP280_STATUS_COMM_ERROR;
    }
    return BMP280_STATUS_OK;
}

static BMP280_Status_t BMP280_ReadRegisters(uint8_t Copy_u8RegAddr, uint8_t* Copy_pu8Data, uint8_t Copy_u8Length)
{
    if (!g_BMP280_Device.is_initialized) return BMP280_STATUS_NOT_INITIALIZED;
    if (Copy_pu8Data == NULL || Copy_u8Length == 0) return BMP280_STATUS_INVALID_PARAM;
    if (I2C_Master_Transmit(g_BMP280_Device.i2c_instance, g_BMP280_Device.i2c_address, &Copy_u8RegAddr, 1, BMP280_I2C_TIMEOUT) != I2C_STATUS_OK) {
        return BMP280_STATUS_COMM_ERROR;
    }
    if (I2C_Master_Receive(g_BMP280_Device.i2c_instance, g_BMP280_Device.i2c_address, Copy_pu8Data, Copy_u8Length, BMP280_I2C_TIMEOUT) != I2C_STATUS_OK) {
        return BMP280_STATUS_COMM_ERROR;
    }
    return BMP280_STATUS_OK;
}

static BMP280_Status_t BMP280_ReadRegister(uint8_t Copy_u8RegAddr, uint8_t* Copy_pu8Value)
{
    return BMP280_ReadRegisters(Copy_u8RegAddr, Copy_pu8Value, 1);
}

/*******************************************************************************************************/
/* Internal Helper Functions (Calibration & Compensation)                                              */
/*******************************************************************************************************/
static BMP280_Status_t BMP280_ReadCalibrationData(void)
{
    uint8_t calib_data[24];
    if (BMP280_ReadRegisters(BMP280_REG_DIG_T1, calib_data, 24) != BMP280_STATUS_OK) {
        return BMP280_STATUS_CALIBRATION_ERROR;
    }
    g_BMP280_Device.calib_data.dig_T1 = (uint16_t)(calib_data[1] << 8) | calib_data[0];
    g_BMP280_Device.calib_data.dig_T2 = (int16_t)((calib_data[3] << 8) | calib_data[2]);
    g_BMP280_Device.calib_data.dig_T3 = (int16_t)((calib_data[5] << 8) | calib_data[4]);
    g_BMP280_Device.calib_data.dig_P1 = (uint16_t)(calib_data[7] << 8) | calib_data[6];
    g_BMP280_Device.calib_data.dig_P2 = (int16_t)((calib_data[9] << 8) | calib_data[8]);
    g_BMP280_Device.calib_data.dig_P3 = (int16_t)((calib_data[11] << 8) | calib_data[10]);
    g_BMP280_Device.calib_data.dig_P4 = (int16_t)((calib_data[13] << 8) | calib_data[12]);
    g_BMP280_Device.calib_data.dig_P5 = (int16_t)((calib_data[15] << 8) | calib_data[14]);
    g_BMP280_Device.calib_data.dig_P6 = (int16_t)((calib_data[17] << 8) | calib_data[16]);
    g_BMP280_Device.calib_data.dig_P7 = (int16_t)((calib_data[19] << 8) | calib_data[18]);
    g_BMP280_Device.calib_data.dig_P8 = (int16_t)((calib_data[21] << 8) | calib_data[20]);
    g_BMP280_Device.calib_data.dig_P9 = (int16_t)((calib_data[23] << 8) | calib_data[22]);
    return BMP280_STATUS_OK;
}

static int32_t BMP280_CompensateTemperature(int32_t Copy_s32AdcTemp)
{
    int32_t var1, var2;
    var1 = ((((Copy_s32AdcTemp >> 3) - ((int32_t)g_BMP280_Device.calib_data.dig_T1 << 1))) * ((int32_t)g_BMP280_Device.calib_data.dig_T2)) >> 11;
    var2 = (((((Copy_s32AdcTemp >> 4) - ((int32_t)g_BMP280_Device.calib_data.dig_T1)) * ((Copy_s32AdcTemp >> 4) - ((int32_t)g_BMP280_Device.calib_data.dig_T1))) >> 12) * ((int32_t)g_BMP280_Device.calib_data.dig_T3)) >> 14;
    g_BMP280_Device.t_fine = var1 + var2;
    return (g_BMP280_Device.t_fine * 5 + 128) >> 8;
}

static uint32_t BMP280_CompensatePressure(int32_t Copy_s32AdcPress)
{
    int64_t var1, var2, p;
    var1 = ((int64_t)g_BMP280_Device.t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)g_BMP280_Device.calib_data.dig_P6;
    var2 = var2 + ((var1 * (int64_t)g_BMP280_Device.calib_data.dig_P5) << 17);
    var2 = var2 + (((int64_t)g_BMP280_Device.calib_data.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)g_BMP280_Device.calib_data.dig_P3) >> 8) + ((var1 * (int64_t)g_BMP280_Device.calib_data.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)g_BMP280_Device.calib_data.dig_P1) >> 33;
    if (var1 == 0) return 0;
    p = 1048576 - Copy_s32AdcPress;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)g_BMP280_Device.calib_data.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)g_BMP280_Device.calib_data.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)g_BMP280_Device.calib_data.dig_P7) << 4);
    return (uint32_t)p / 256;
}

/*******************************************************************************************************/
/* Public Function Implementations                                                                     */
/*******************************************************************************************************/
BMP280_Status_t BMP280_Initialize(I2C_Instance_t Copy_I2CInstance, uint8_t Copy_u8DeviceAddress)
{
    uint8_t chip_id;
    g_BMP280_Device.i2c_instance = Copy_I2CInstance;
    g_BMP280_Device.i2c_address = Copy_u8DeviceAddress;
    g_BMP280_Device.is_initialized = 1;
    STK_DelayMs(BMP280_STARTUP_DELAY_MS);

    if (BMP280_GetChipID(&chip_id) != BMP280_STATUS_OK || chip_id != BMP280_CHIP_ID) {
        printf("BMP280: Invalid chip ID: 0x%02X (expected 0x%02X)\r\n", chip_id, BMP280_CHIP_ID);
        g_BMP280_Device.is_initialized = 0;
        return BMP280_STATUS_ERROR;
    }
    if (BMP280_Reset() != BMP280_STATUS_OK) return BMP280_STATUS_ERROR;
    if (BMP280_ReadCalibrationData() != BMP280_STATUS_OK) return BMP280_STATUS_CALIBRATION_ERROR;

    BMP280_Config_t default_config = {
        .mode = BMP280_DEFAULT_MODE,
        .osrs_temp = BMP280_DEFAULT_TEMP_OSRS,
        .osrs_press = BMP280_DEFAULT_PRESS_OSRS,
        .filter = BMP280_DEFAULT_FILTER,
        .standby_time = BMP280_DEFAULT_STANDBY_TIME,
        .spi3w_en = 0
    };
    return BMP280_Configure(&default_config);
}

BMP280_Status_t BMP280_Reset(void)
{
    BMP280_Status_t status = BMP280_WriteRegister(BMP280_REG_RESET, BMP280_SOFT_RESET_CMD);
    if (status == BMP280_STATUS_OK) STK_DelayMs(BMP280_RESET_DELAY_MS);
    return status;
}

BMP280_Status_t BMP280_Configure(BMP280_Config_t* Copy_pConfig)
{
    if (Copy_pConfig == NULL) return BMP280_STATUS_INVALID_PARAM;
    uint8_t ctrl_meas = (Copy_pConfig->osrs_temp << 5) | (Copy_pConfig->osrs_press << 2) | Copy_pConfig->mode;
    if (BMP280_WriteRegister(BMP280_REG_CTRL_MEAS, ctrl_meas) != BMP280_STATUS_OK) return BMP280_STATUS_ERROR;
    uint8_t config = (Copy_pConfig->standby_time << 5) | (Copy_pConfig->filter << 2) | Copy_pConfig->spi3w_en;
    return BMP280_WriteRegister(BMP280_REG_CONFIG, config);
}

BMP280_Status_t BMP280_SetMode(BMP280_Mode_t Copy_Mode)
{
    uint8_t ctrl_meas;
    if (BMP280_ReadRegister(BMP280_REG_CTRL_MEAS, &ctrl_meas) != BMP280_STATUS_OK) return BMP280_STATUS_ERROR;
    ctrl_meas = (ctrl_meas & ~0x03) | (Copy_Mode & 0x03);
    return BMP280_WriteRegister(BMP280_REG_CTRL_MEAS, ctrl_meas);
}

BMP280_Status_t BMP280_GetChipID(uint8_t* Copy_pu8ChipID)
{
    if (Copy_pu8ChipID == NULL) return BMP280_STATUS_INVALID_PARAM;
    return BMP280_ReadRegister(BMP280_REG_CHIP_ID, Copy_pu8ChipID);
}

BMP280_Status_t BMP280_ReadSensorData(BMP280_Data_t* Copy_pData)
{
    uint8_t raw_data[6];
    if (Copy_pData == NULL) return BMP280_STATUS_INVALID_PARAM;
    if (BMP280_ReadRegisters(BMP280_REG_PRESS_MSB, raw_data, 6) != BMP280_STATUS_OK) return BMP280_STATUS_ERROR;

    int32_t adc_p = ((uint32_t)raw_data[0] << 12) | ((uint32_t)raw_data[1] << 4) | ((uint32_t)raw_data[2] >> 4);
    int32_t adc_t = ((uint32_t)raw_data[3] << 12) | ((uint32_t)raw_data[4] << 4) | ((uint32_t)raw_data[5] >> 4);

    Copy_pData->temperature = BMP280_CompensateTemperature(adc_t);
    Copy_pData->pressure = BMP280_CompensatePressure(adc_p);
    Copy_pData->timestamp = STK_GetMillis();
    return BMP280_STATUS_OK;
}

BMP280_Status_t BMP280_ReadTemperature(int32_t* Copy_ps32Temperature)
{
    BMP280_Data_t data;
    BMP280_Status_t status = BMP280_ReadSensorData(&data);
    if (status == BMP280_STATUS_OK) *Copy_ps32Temperature = data.temperature;
    return status;
}

BMP280_Status_t BMP280_ReadPressure(uint32_t* Copy_pu32Pressure)
{
    BMP280_Data_t data;
    BMP280_Status_t status = BMP280_ReadSensorData(&data);
    if (status == BMP280_STATUS_OK) *Copy_pu32Pressure = data.pressure;
    return status;
}

BMP280_Status_t BMP280_IsMeasuring(uint8_t* Copy_pu8IsMeasuring)
{
    uint8_t status_reg;
    if (Copy_pu8IsMeasuring == NULL) return BMP280_STATUS_INVALID_PARAM;
    BMP280_Status_t status = BMP280_ReadRegister(BMP280_REG_STATUS, &status_reg);
    if (status == BMP280_STATUS_OK) *Copy_pu8IsMeasuring = (status_reg >> 3) & 0x01;
    return status;
}

BMP280_Status_t BMP280_GetConfig(BMP280_Config_t* Copy_pConfig)
{
    uint8_t ctrl_meas, config_reg;
    if (Copy_pConfig == NULL) return BMP280_STATUS_INVALID_PARAM;
    if (BMP280_ReadRegister(BMP280_REG_CTRL_MEAS, &ctrl_meas) != BMP280_STATUS_OK) return BMP280_STATUS_ERROR;
    if (BMP280_ReadRegister(BMP280_REG_CONFIG, &config_reg) != BMP280_STATUS_OK) return BMP280_STATUS_ERROR;

    Copy_pConfig->osrs_temp = (ctrl_meas >> 5) & 0x07;
    Copy_pConfig->osrs_press = (ctrl_meas >> 2) & 0x07;
    Copy_pConfig->mode = (ctrl_meas & 0x03);
    Copy_pConfig->standby_time = (config_reg >> 5) & 0x07;
    Copy_pConfig->filter = (config_reg >> 2) & 0x07;
    Copy_pConfig->spi3w_en = (config_reg & 0x01);
    return BMP280_STATUS_OK;
}
