/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 24 Aug 2025                                                                     */
/* Description       : i2c_interface.h --> I2C Interface Header File                                   */
/*******************************************************************************************************/

#ifndef I2C_INTERFACE_H_
#define I2C_INTERFACE_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

/*******************************************************************************************************/
/* I2C Instance Definitions                                                                            */
/*******************************************************************************************************/

typedef enum
{
    I2C_INSTANCE_1 = 0,
    I2C_INSTANCE_2,
    I2C_INSTANCE_3
} I2C_Instance_t;

/*******************************************************************************************************/
/* I2C Operating Mode                                                                                  */
/*******************************************************************************************************/

typedef enum
{
    I2C_MODE_MASTER = 0,
    I2C_MODE_SLAVE
} I2C_Mode_t;

/*******************************************************************************************************/
/* I2C Clock Speed                                                                                     */
/*******************************************************************************************************/

typedef enum
{
    I2C_SPEED_STANDARD_100KHZ = 100000,
    I2C_SPEED_FAST_400KHZ     = 400000
} I2C_ClockSpeed_t;

/*******************************************************************************************************/
/* Function Return Status                                                                              */
/*******************************************************************************************************/

typedef enum
{
    I2C_STATUS_OK = 0,
    I2C_STATUS_ERROR,
    I2C_STATUS_INVALID_INSTANCE,
    I2C_STATUS_TIMEOUT,
    I2C_STATUS_BUSY,
    I2C_STATUS_NACK_RECEIVED
} I2C_Status_t;

/*******************************************************************************************************/
/* Function Prototypes                                                                                 */
/*******************************************************************************************************/

I2C_Status_t I2C_Initialize(I2C_Instance_t Copy_Instance);
I2C_Status_t I2C_Enable(I2C_Instance_t Copy_Instance);
I2C_Status_t I2C_Disable(I2C_Instance_t Copy_Instance);

I2C_Status_t I2C_Master_Transmit(I2C_Instance_t Copy_Instance, uint8_t Copy_u8SlaveAddress, uint8_t* Copy_pu8Data, uint16_t Copy_u16Size, uint32_t Copy_u32Timeout);
I2C_Status_t I2C_Master_Receive(I2C_Instance_t Copy_Instance, uint8_t Copy_u8SlaveAddress, uint8_t* Copy_pu8Data, uint16_t Copy_u16Size, uint32_t Copy_u32Timeout);

I2C_Status_t I2C_Slave_Transmit(I2C_Instance_t Copy_Instance, uint8_t* Copy_pu8Data, uint16_t Copy_u16Size, uint32_t Copy_u32Timeout);
I2C_Status_t I2C_Slave_Receive(I2C_Instance_t Copy_Instance, uint8_t* Copy_pu8Data, uint16_t Copy_u16Size, uint32_t Copy_u32Timeout);

I2C_Status_t I2C_Set_EventCallback(I2C_Instance_t Copy_Instance, void(*Copy_Fptr)(void));
I2C_Status_t I2C_Set_ErrorCallback(I2C_Instance_t Copy_Instance, void(*Copy_Fptr)(void));

#endif /* I2C_INTERFACE_H_ */
