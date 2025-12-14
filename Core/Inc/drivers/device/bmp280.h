/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.1.0                                                                          */
/* Date              : 03 Oct 2025                                                                     */
/* Description       : bmp280.h --> BMP280 Driver Header File                                          */
/*******************************************************************************************************/

#ifndef BMP280_H_
#define BMP280_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/
#include <stdint.h>
#include "i2c.h"

/*******************************************************************************************************/
/* BMP280 Configuration Parameters                                                                     */
/*******************************************************************************************************/

/* I2C Configuration */
#define BMP280_I2C_INSTANCE             I2C_INSTANCE_1
#define BMP280_I2C_ADDRESS              BMP280_I2C_ADDR_PRIMARY  /* 0x76 */
#define BMP280_I2C_TIMEOUT              1000  /* Timeout in milliseconds */

/* Default Sensor Configuration */
#define BMP280_DEFAULT_MODE             BMP280_MODE_NORMAL
#define BMP280_DEFAULT_TEMP_OSRS        BMP280_OVERSAMPLING_16X
#define BMP280_DEFAULT_PRESS_OSRS       BMP280_OVERSAMPLING_16X
#define BMP280_DEFAULT_FILTER           BMP280_FILTER_4
#define BMP280_DEFAULT_STANDBY_TIME     BMP280_STANDBY_500_MS

/* Measurement Settings */
#define BMP280_MEASUREMENT_DELAY_MS     10   /* Delay after triggering forced measurement */
#define BMP280_MAX_MEASUREMENT_TIME_MS  40   /* Max measurement time for temp/press at 16x oversampling */

/* Calibration Settings */
#define BMP280_STARTUP_DELAY_MS         5    /* Delay after power-on */
#define BMP280_RESET_DELAY_MS           10   /* Delay after soft reset */

/*******************************************************************************************************/
/* BMP280 Register Addresses                                                                           */
/*******************************************************************************************************/
#define BMP280_REG_DIG_T1          0x88
#define BMP280_REG_DIG_T2          0x8A
#define BMP280_REG_DIG_T3          0x8C
#define BMP280_REG_DIG_P1          0x8E
#define BMP280_REG_DIG_P2          0x90
#define BMP280_REG_DIG_P3          0x92
#define BMP280_REG_DIG_P4          0x94
#define BMP280_REG_DIG_P5          0x96
#define BMP280_REG_DIG_P6          0x98
#define BMP280_REG_DIG_P7          0x9A
#define BMP280_REG_DIG_P8          0x9C
#define BMP280_REG_DIG_P9          0x9E
#define BMP280_REG_CHIP_ID         0xD0
#define BMP280_REG_RESET           0xE0
#define BMP280_REG_STATUS          0xF3
#define BMP280_REG_CTRL_MEAS       0xF4
#define BMP280_REG_CONFIG          0xF5
#define BMP280_REG_PRESS_MSB       0xF7

/*******************************************************************************************************/
/* BMP280 Command Values                                                                               */
/*******************************************************************************************************/
#define BMP280_CHIP_ID              0x58
#define BMP280_SOFT_RESET_CMD       0xB6

/*******************************************************************************************************/
/* BMP280 I2C Addresses                                                                                */
/*******************************************************************************************************/
#define BMP280_I2C_ADDR_PRIMARY     0x76  /* SDO pin connected to GND */
#define BMP280_I2C_ADDR_SECONDARY   0x77  /* SDO pin connected to VDD */

/*******************************************************************************************************/
/* BMP280 Operating Modes                                                                              */
/*******************************************************************************************************/
typedef enum
{
    BMP280_MODE_SLEEP  = 0x00,
    BMP280_MODE_FORCED = 0x01,
    BMP280_MODE_NORMAL = 0x03
} BMP280_Mode_t;

/*******************************************************************************************************/
/* BMP280 Oversampling Settings                                                                        */
/*******************************************************************************************************/
typedef enum
{
    BMP280_OVERSAMPLING_SKIP = 0x00,
    BMP280_OVERSAMPLING_1X   = 0x01,
    BMP280_OVERSAMPLING_2X   = 0x02,
    BMP280_OVERSAMPLING_4X   = 0x03,
    BMP280_OVERSAMPLING_8X   = 0x04,
    BMP280_OVERSAMPLING_16X  = 0x05
} BMP280_Oversampling_t;

/*******************************************************************************************************/
/* BMP280 Filter Settings                                                                              */
/*******************************************************************************************************/
typedef enum
{
    BMP280_FILTER_OFF = 0x00,
    BMP280_FILTER_2   = 0x01,
    BMP280_FILTER_4   = 0x02,
    BMP280_FILTER_8   = 0x03,
    BMP280_FILTER_16  = 0x04
} BMP280_Filter_t;

/*******************************************************************************************************/
/* BMP280 Standby Time (for Normal Mode)                                                               */
/*******************************************************************************************************/
typedef enum
{
    BMP280_STANDBY_0_5_MS  = 0x00,
    BMP280_STANDBY_62_5_MS = 0x01,
    BMP280_STANDBY_125_MS  = 0x02,
    BMP280_STANDBY_250_MS  = 0x03,
    BMP280_STANDBY_500_MS  = 0x04,
    BMP280_STANDBY_1000_MS = 0x05,
    BMP280_STANDBY_10_MS   = 0x06,
    BMP280_STANDBY_20_MS   = 0x07
} BMP280_StandbyTime_t;

/*******************************************************************************************************/
/* BMP280 Configuration Structure                                                                      */
/*******************************************************************************************************/
typedef struct
{
    BMP280_Mode_t mode;
    BMP280_Oversampling_t osrs_temp;
    BMP280_Oversampling_t osrs_press;
    BMP280_Filter_t filter;
    BMP280_StandbyTime_t standby_time;
    uint8_t spi3w_en;  /* 3-wire SPI enable (0 for I2C) */
} BMP280_Config_t;

/*******************************************************************************************************/
/* BMP280 Sensor Data Structure                                                                        */
/*******************************************************************************************************/
typedef struct
{
    int32_t temperature;    /* Temperature in 0.01 degrees Celsius (e.g., 2351 = 23.51°C) */
    uint32_t pressure;      /* Pressure in Pa */
    uint32_t timestamp;     /* Timestamp in milliseconds */
} BMP280_Data_t;

/*******************************************************************************************************/
/* BMP280 Status                                                                                       */
/*******************************************************************************************************/
typedef enum
{
    BMP280_STATUS_OK = 0,
    BMP280_STATUS_ERROR,
    BMP280_STATUS_INVALID_PARAM,
    BMP280_STATUS_COMM_ERROR,
    BMP280_STATUS_NOT_INITIALIZED,
    BMP280_STATUS_BUSY,
    BMP280_STATUS_TIMEOUT,
    BMP280_STATUS_CALIBRATION_ERROR
} BMP280_Status_t;

/*******************************************************************************************************/
/* BMP280 Calibration Data Structure                                                                   */
/*******************************************************************************************************/
typedef struct
{
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} BMP280_CalibData_t;

/*******************************************************************************************************/
/* BMP280 Device Structure                                                                             */
/*******************************************************************************************************/
typedef struct
{
    uint8_t i2c_address;
    I2C_Instance_t i2c_instance;
    BMP280_CalibData_t calib_data;
    int32_t t_fine;
    uint8_t is_initialized;
} BMP280_Device_t;

/*******************************************************************************************************/
/* Function Prototypes                                                                                 */
/*******************************************************************************************************/
BMP280_Status_t BMP280_Initialize(I2C_Instance_t Copy_I2CInstance, uint8_t Copy_u8DeviceAddress);
BMP280_Status_t BMP280_Reset(void);
BMP280_Status_t BMP280_Configure(BMP280_Config_t* Copy_pConfig);
BMP280_Status_t BMP280_SetMode(BMP280_Mode_t Copy_Mode);
BMP280_Status_t BMP280_GetChipID(uint8_t* Copy_pu8ChipID);
BMP280_Status_t BMP280_ReadSensorData(BMP280_Data_t* Copy_pData);
BMP280_Status_t BMP280_ReadTemperature(int32_t* Copy_ps32Temperature);
BMP280_Status_t BMP280_ReadPressure(uint32_t* Copy_pu32Pressure);
BMP280_Status_t BMP280_IsMeasuring(uint8_t* Copy_pu8IsMeasuring);
BMP280_Status_t BMP280_GetConfig(BMP280_Config_t* Copy_pConfig);

#endif /* BMP280_H_ */
