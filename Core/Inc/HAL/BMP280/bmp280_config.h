/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.1.0                                                                          */
/* Date              : 03 Oct 2025                                                                     */
/* Description       : bmp280_config.h --> BMP280 Configuration Header File                            */
/*******************************************************************************************************/

#ifndef BMP280_CONFIG_H_
#define BMP280_CONFIG_H_

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

#endif /* BMP280_CONFIG_H_ */
