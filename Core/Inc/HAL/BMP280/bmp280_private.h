/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.1.0                                                                          */
/* Date              : 03 Oct 2025                                                                     */
/* Description       : bmp280_private.h --> BMP280 Private Header File                                 */
/*******************************************************************************************************/

#ifndef BMP280_PRIVATE_H_
#define BMP280_PRIVATE_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/
#include <stdint.h>
#include "i2c_interface.h"
#include "bmp280_interface.h"

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
/* Internal Function Prototypes                                                                        */
/*******************************************************************************************************/
static int32_t BMP280_CompensateTemperature(int32_t Copy_s32AdcTemp);
static uint32_t BMP280_CompensatePressure(int32_t Copy_s32AdcPress);

#endif /* BMP280_PRIVATE_H_ */
