/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 12 Dec 2025                                                                     */
/* Description       : tsl2561.h --> TSL2561 Light Sensor Driver Header File                           */
/*******************************************************************************************************/

#ifndef TSL2561_H_
#define TSL2561_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/
#include <stdint.h>
#include "i2c.h"

/*******************************************************************************************************/
/* TSL2561 Configuration Parameters                                                                    */
/*******************************************************************************************************/

/* I2C Configuration */
#define TSL2561_I2C_INSTANCE            I2C_INSTANCE_1
#define TSL2561_I2C_ADDRESS             TSL2561_I2C_ADDR_FLOAT  /* 0x39 */
#define TSL2561_I2C_TIMEOUT             1000  /* Timeout in milliseconds */

/* Default Sensor Configuration */
#define TSL2561_DEFAULT_INTEGRATION     TSL2561_INTEGRATION_402MS
#define TSL2561_DEFAULT_GAIN            TSL2561_GAIN_1X

/* Timing Settings */
#define TSL2561_STARTUP_DELAY_MS        5     /* Delay after power-on */
#define TSL2561_POWERUP_DELAY_MS        50    /* Delay after enabling sensor */

/*******************************************************************************************************/
/* TSL2561 Register Addresses                                                                          */
/*******************************************************************************************************/
#define TSL2561_REG_CONTROL             0x00  /* Control register */
#define TSL2561_REG_TIMING              0x01  /* Timing register */
#define TSL2561_REG_THRESHLOWLOW        0x02  /* Low byte of low interrupt threshold */
#define TSL2561_REG_THRESHLOWHIGH       0x03  /* High byte of low interrupt threshold */
#define TSL2561_REG_THRESHHIGHLOW       0x04  /* Low byte of high interrupt threshold */
#define TSL2561_REG_THRESHHIGHHIGH      0x05  /* High byte of high interrupt threshold */
#define TSL2561_REG_INTERRUPT           0x06  /* Interrupt control register */
#define TSL2561_REG_ID                  0x0A  /* Part number / Rev ID */
#define TSL2561_REG_DATA0LOW            0x0C  /* Low byte of ADC channel 0 */
#define TSL2561_REG_DATA0HIGH           0x0D  /* High byte of ADC channel 0 */
#define TSL2561_REG_DATA1LOW            0x0E  /* Low byte of ADC channel 1 */
#define TSL2561_REG_DATA1HIGH           0x0F  /* High byte of ADC channel 1 */

/*******************************************************************************************************/
/* TSL2561 Command Register Bits                                                                       */
/*******************************************************************************************************/
#define TSL2561_COMMAND_BIT             0x80  /* Must be set for command register access */
#define TSL2561_CLEAR_BIT               0x40  /* Clear any pending interrupt */
#define TSL2561_WORD_BIT                0x20  /* Read/write word (2 bytes) */
#define TSL2561_BLOCK_BIT               0x10  /* Block read/write */

/*******************************************************************************************************/
/* TSL2561 Control Register Values                                                                     */
/*******************************************************************************************************/
#define TSL2561_CONTROL_POWERON         0x03  /* Power on the device */
#define TSL2561_CONTROL_POWEROFF        0x00  /* Power off the device */

/*******************************************************************************************************/
/* TSL2561 Chip ID                                                                                     */
/*******************************************************************************************************/
#define TSL2561_CHIP_ID_PARTNO_MASK     0xF0  /* Part number mask */
#define TSL2561_CHIP_ID_TSL2561         0x50  /* TSL2561 part number (0101xxxx) */

/*******************************************************************************************************/
/* TSL2561 I2C Addresses                                                                               */
/*******************************************************************************************************/
#define TSL2561_I2C_ADDR_GND            0x29  /* ADDR pin connected to GND */
#define TSL2561_I2C_ADDR_FLOAT          0x39  /* ADDR pin floating */
#define TSL2561_I2C_ADDR_VDD            0x49  /* ADDR pin connected to VDD */

/*******************************************************************************************************/
/* TSL2561 Integration Time                                                                            */
/*******************************************************************************************************/
typedef enum
{
    TSL2561_INTEGRATION_13_7MS  = 0x00,  /* 13.7ms - fast but low resolution */
    TSL2561_INTEGRATION_101MS   = 0x01,  /* 101ms - medium */
    TSL2561_INTEGRATION_402MS   = 0x02   /* 402ms - slow but high resolution */
} TSL2561_Integration_t;

/*******************************************************************************************************/
/* TSL2561 Gain Settings                                                                               */
/*******************************************************************************************************/
typedef enum
{
    TSL2561_GAIN_1X  = 0x00,  /* No gain - for bright light */
    TSL2561_GAIN_16X = 0x10   /* 16x gain - for low light */
} TSL2561_Gain_t;

/*******************************************************************************************************/
/* TSL2561 Package Type (affects lux calculation)                                                      */
/*******************************************************************************************************/
typedef enum
{
    TSL2561_PACKAGE_CS = 0,   /* Chipscale package */
    TSL2561_PACKAGE_T_FN_CL   /* T, FN, or CL package */
} TSL2561_Package_t;

/*******************************************************************************************************/
/* TSL2561 Configuration Structure                                                                     */
/*******************************************************************************************************/
typedef struct
{
    TSL2561_Integration_t integration_time;
    TSL2561_Gain_t gain;
    TSL2561_Package_t package;
} TSL2561_Config_t;

/*******************************************************************************************************/
/* TSL2561 Sensor Data Structure                                                                       */
/*******************************************************************************************************/
typedef struct
{
    uint16_t channel0;      /* Broadband (visible + IR) raw ADC value */
    uint16_t channel1;      /* Infrared raw ADC value */
    uint32_t lux;           /* Calculated lux value */
    uint32_t timestamp;     /* Timestamp in milliseconds */
} TSL2561_Data_t;

/*******************************************************************************************************/
/* TSL2561 Status                                                                                      */
/*******************************************************************************************************/
typedef enum
{
    TSL2561_STATUS_OK = 0,
    TSL2561_STATUS_ERROR,
    TSL2561_STATUS_INVALID_PARAM,
    TSL2561_STATUS_COMM_ERROR,
    TSL2561_STATUS_NOT_INITIALIZED,
    TSL2561_STATUS_TIMEOUT,
    TSL2561_STATUS_OVERFLOW
} TSL2561_Status_t;

/*******************************************************************************************************/
/* TSL2561 Device Structure                                                                            */
/*******************************************************************************************************/
typedef struct
{
    uint8_t i2c_address;
    I2C_Instance_t i2c_instance;
    TSL2561_Integration_t integration_time;
    TSL2561_Gain_t gain;
    TSL2561_Package_t package;
    uint8_t is_initialized;
    uint8_t is_powered;
} TSL2561_Device_t;

/*******************************************************************************************************/
/* Function Prototypes                                                                                 */
/*******************************************************************************************************/
TSL2561_Status_t TSL2561_Initialize(I2C_Instance_t Copy_I2CInstance, uint8_t Copy_u8DeviceAddress);
TSL2561_Status_t TSL2561_Configure(TSL2561_Config_t* Copy_pConfig);
TSL2561_Status_t TSL2561_SetIntegrationTime(TSL2561_Integration_t Copy_Integration);
TSL2561_Status_t TSL2561_SetGain(TSL2561_Gain_t Copy_Gain);
TSL2561_Status_t TSL2561_PowerOn(void);
TSL2561_Status_t TSL2561_PowerOff(void);
TSL2561_Status_t TSL2561_GetChipID(uint8_t* Copy_pu8ChipID);
TSL2561_Status_t TSL2561_ReadRawData(uint16_t* Copy_pu16Channel0, uint16_t* Copy_pu16Channel1);
TSL2561_Status_t TSL2561_ReadSensorData(TSL2561_Data_t* Copy_pData);
TSL2561_Status_t TSL2561_ReadLux(uint32_t* Copy_pu32Lux);
TSL2561_Status_t TSL2561_GetConfig(TSL2561_Config_t* Copy_pConfig);
uint8_t TSL2561_IsInitialized(void);

#endif /* TSL2561_H_ */
