/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.1.0                                                                          */
/* Date              : 03 Oct 2025                                                                     */
/* Description       : bmp280_interface.h --> BMP280 Interface Header File                             */
/*******************************************************************************************************/

#ifndef BMP280_INTERFACE_H_
#define BMP280_INTERFACE_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/
#include <stdint.h>
#include "i2c_interface.h"

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

#endif /* BMP280_INTERFACE_H_ */
