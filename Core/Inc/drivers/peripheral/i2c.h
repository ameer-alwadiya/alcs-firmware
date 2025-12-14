/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 24 Aug 2025                                                                     */
/* Description       : i2c.h --> I2C Driver (Merged Header)                                            */
/*******************************************************************************************************/

#ifndef I2C_H_
#define I2C_H_

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include <stdint.h>
#include <stdio.h>

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

/*******************************************************************************************************/
/* I2C Base Addresses                                                                                  */
/*******************************************************************************************************/

#define I2C1_BASE_ADDRESS       0x40005400UL
#define I2C2_BASE_ADDRESS       0x40005800UL
#define I2C3_BASE_ADDRESS       0x40005C00UL

/*******************************************************************************************************/
/* I2C Timeout Value                                                                                   */
/*******************************************************************************************************/

#define I2C_TIMEOUT_VALUE       100000UL

/*******************************************************************************************************/
/* I2C Register Structure                                                                              */
/*******************************************************************************************************/

typedef struct
{
    uint32_t CR1;       /* I2C Control register 1 */
    uint32_t CR2;       /* I2C Control register 2 */
    uint32_t OAR1;      /* I2C Own address register 1 */
    uint32_t OAR2;      /* I2C Own address register 2 */
    uint32_t DR;        /* I2C Data register */
    uint32_t SR1;       /* I2C Status register 1 */
    uint32_t SR2;       /* I2C Status register 2 */
    uint32_t CCR;       /* I2C Clock control register */
    uint32_t TRISE;     /* I2C TRISE register */
    uint32_t FLTR;      /* I2C FLTR register */
} I2C_Registers_t;

/*******************************************************************************************************/
/* I2C Register Pointers                                                                               */
/*******************************************************************************************************/

#define I2C1_REG        ((volatile I2C_Registers_t*)(I2C1_BASE_ADDRESS))
#define I2C2_REG        ((volatile I2C_Registers_t*)(I2C2_BASE_ADDRESS))
#define I2C3_REG        ((volatile I2C_Registers_t*)(I2C3_BASE_ADDRESS))

/*******************************************************************************************************/
/* I2C Control Register 1 (CR1) Bits                                                                   */
/*******************************************************************************************************/

#define I2C_CR1_PE_BIT          0   /* Peripheral enable */
#define I2C_CR1_SMBUS_BIT       1   /* SMBus mode */
#define I2C_CR1_SMBTYPE_BIT     3   /* SMBus type */
#define I2C_CR1_ENARP_BIT       4   /* ARP enable */
#define I2C_CR1_ENPEC_BIT       5   /* PEC enable */
#define I2C_CR1_ENGC_BIT        6   /* General call enable */
#define I2C_CR1_NOSTRETCH_BIT   7   /* Clock stretching disable (Slave mode) */
#define I2C_CR1_START_BIT       8   /* Start generation */
#define I2C_CR1_STOP_BIT        9   /* Stop generation */
#define I2C_CR1_ACK_BIT         10  /* Acknowledge enable */
#define I2C_CR1_POS_BIT         11  /* Acknowledge/PEC Position (for data reception) */
#define I2C_CR1_PEC_BIT         12  /* Packet error checking */
#define I2C_CR1_ALERT_BIT       13  /* SMBus alert */
#define I2C_CR1_SWRST_BIT       15  /* Software reset */

/*******************************************************************************************************/
/* I2C Control Register 2 (CR2) Bits                                                                   */
/*******************************************************************************************************/

#define I2C_CR2_FREQ_BIT        0   /* Peripheral clock frequency [5:0] */
#define I2C_CR2_ITERREN_BIT     8   /* Error interrupt enable */
#define I2C_CR2_ITEVTEN_BIT     9   /* Event interrupt enable */
#define I2C_CR2_ITBUFEN_BIT     10  /* Buffer interrupt enable */
#define I2C_CR2_DMAEN_BIT       11  /* DMA requests enable */
#define I2C_CR2_LAST_BIT        12  /* DMA last transfer */

/*******************************************************************************************************/
/* I2C Status Register 1 (SR1) Bits                                                                    */
/*******************************************************************************************************/

#define I2C_SR1_SB_BIT          0   /* Start bit (Master mode) */
#define I2C_SR1_ADDR_BIT        1   /* Address sent (master mode)/matched (slave mode) */
#define I2C_SR1_BTF_BIT         2   /* Byte transfer finished */
#define I2C_SR1_ADD10_BIT       3   /* 10-bit header sent (Master mode) */
#define I2C_SR1_STOPF_BIT       4   /* Stop detection (slave mode) */
#define I2C_SR1_RXNE_BIT        6   /* Data register not empty (receivers) */
#define I2C_SR1_TXE_BIT         7   /* Data register empty (transmitters) */
#define I2C_SR1_BERR_BIT        8   /* Bus error */
#define I2C_SR1_ARLO_BIT        9   /* Arbitration lost (master mode) */
#define I2C_SR1_AF_BIT          10  /* Acknowledge failure */
#define I2C_SR1_OVR_BIT         11  /* Overrun/Underrun */
#define I2C_SR1_PECERR_BIT      12  /* PEC Error in reception */
#define I2C_SR1_TIMEOUT_BIT     14  /* Timeout or Tlow error */
#define I2C_SR1_SMBALERT_BIT    15  /* SMBus alert */

/*******************************************************************************************************/
/* I2C Status Register 2 (SR2) Bits                                                                    */
/*******************************************************************************************************/

#define I2C_SR2_MSL_BIT         0   /* Master/slave */
#define I2C_SR2_BUSY_BIT        1   /* Bus busy */
#define I2C_SR2_TRA_BIT         2   /* Transmitter/receiver */
#define I2C_SR2_GENCALL_BIT     4   /* General call address (Slave mode) */
#define I2C_SR2_SMBDEFAULT_BIT  5   /* SMBus device default address (Slave mode) */
#define I2C_SR2_SMBHOST_BIT     6   /* SMBus host header (Slave mode) */
#define I2C_SR2_DUALF_BIT       7   /* Dual flag (Slave mode) */
#define I2C_SR2_PEC_BIT         8   /* Packet error checking register [7:0] */

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

#endif /* I2C_H_ */
