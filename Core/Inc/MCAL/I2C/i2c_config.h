/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 24 Aug 2025                                                                     */
/* Description       : i2c_config.h --> I2C Configuration Header File                                  */
/*******************************************************************************************************/

#ifndef I2C_CONFIG_H_
#define I2C_CONFIG_H_

/*******************************************************************************************************/
/* I2C Configuration Parameters                                                                        */
/*******************************************************************************************************/

/* Peripheral Clock Frequency in MHz (APB1 Clock) */
#define I2C_PERIPHERAL_CLOCK_FREQ_MHZ   16

/* I2C1 Configuration */
#define I2C1_MODE                       I2C_MODE_MASTER
#define I2C1_CLOCK_SPEED                I2C_SPEED_STANDARD_100KHZ
#define I2C1_OWN_ADDRESS_1              0x00  // Not used in master mode
#define I2C1_DUTY_CYCLE                 I2C_DUTY_CYCLE_2 // For fast mode

/*******************************************************************************************************/
/* I2C Duty Cycle Options (for Fast Mode)                                                              */
/*******************************************************************************************************/

#define I2C_DUTY_CYCLE_2                0
#define I2C_DUTY_CYCLE_16_9             1

#endif /* I2C_CONFIG_H_ */
