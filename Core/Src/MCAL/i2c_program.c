/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 24 Aug 2025                                                                     */
/* Description       : i2c_program.c --> I2C Implementation File                                       */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include "i2c_private.h"
#include "i2c_config.h"
#include "i2c_interface.h"

/*******************************************************************************************************/
/* Global Variables                                                                                    */
/*******************************************************************************************************/

static void(*I2C1_EventCallbackPtr)(void) = NULL;
static void(*I2C1_ErrorCallbackPtr)(void) = NULL;
static void(*I2C2_EventCallbackPtr)(void) = NULL;
static void(*I2C2_ErrorCallbackPtr)(void) = NULL;
static void(*I2C3_EventCallbackPtr)(void) = NULL;
static void(*I2C3_ErrorCallbackPtr)(void) = NULL;

/*******************************************************************************************************/
/* Helper Functions                                                                                    */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* I2C_GetRegisterPointer                                                                              */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Get I2C register pointer for a given instance                               */
/* Function Input       -> I2C Instance                                                                */
/* Function Return      -> Register pointer or NULL if invalid                                         */
/*******************************************************************************************************/
static volatile I2C_Registers_t* I2C_GetRegisterPointer(I2C_Instance_t Copy_Instance)
{
    switch (Copy_Instance)
    {
        case I2C_INSTANCE_1: return I2C1_REG;
        case I2C_INSTANCE_2: return I2C2_REG;
        case I2C_INSTANCE_3: return I2C3_REG;
        default:             return NULL;
    }
}

/*******************************************************************************************************/
/* Function Implementations                                                                            */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* I2C_Initialize                                                                                      */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Initialize I2C peripheral                                                   */
/* Function Input       -> I2C Instance                                                                */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/
I2C_Status_t I2C_Initialize(I2C_Instance_t Copy_Instance)
{
    volatile I2C_Registers_t* I2C_Reg = I2C_GetRegisterPointer(Copy_Instance);
    if (I2C_Reg == NULL)
    {
        return I2C_STATUS_INVALID_INSTANCE;
    }

    // Disable the peripheral
    I2C_Reg->CR1 &= ~(1U << I2C_CR1_PE_BIT); // (*I2C_Reg).CR1

    // Reset I2C
    I2C_Reg->CR1 |= (1U << I2C_CR1_SWRST_BIT);
    I2C_Reg->CR1 &= ~(1U << I2C_CR1_SWRST_BIT);

    // Configure peripheral clock frequency
    I2C_Reg->CR2 &= ~(0x3FU << I2C_CR2_FREQ_BIT);
    I2C_Reg->CR2 |= (I2C_PERIPHERAL_CLOCK_FREQ_MHZ << I2C_CR2_FREQ_BIT);

    // Configure clock control register (CCR)
    I2C_Reg->CCR = 0;
    uint16_t ccr_value = 0;
    if (I2C1_CLOCK_SPEED <= 100000)
    {
        // Standard mode
        ccr_value = (I2C_PERIPHERAL_CLOCK_FREQ_MHZ * 1000000) / (I2C1_CLOCK_SPEED * 2);
        I2C_Reg->CCR &= ~(1U << 15); // Standard mode
    }
    else
    {
        // Fast mode
        I2C_Reg->CCR |= (1U << 15); // Fast mode
        if (I2C1_DUTY_CYCLE == I2C_DUTY_CYCLE_2)
        {
            ccr_value = (I2C_PERIPHERAL_CLOCK_FREQ_MHZ * 1000000) / (I2C1_CLOCK_SPEED * 3);
            I2C_Reg->CCR &= ~(1U << 14); // Duty cycle 2
        }
        else
        {
            ccr_value = (I2C_PERIPHERAL_CLOCK_FREQ_MHZ * 1000000) / (I2C1_CLOCK_SPEED * 25);
            I2C_Reg->CCR |= (1U << 14); // Duty cycle 16/9
        }
    }
    I2C_Reg->CCR |= ccr_value;

    // Configure TRISE register
    uint16_t trise_value = 0;
    if (I2C1_CLOCK_SPEED <= 100000)
    {
        trise_value = I2C_PERIPHERAL_CLOCK_FREQ_MHZ + 1;
    }
    else
    {
        trise_value = ((I2C_PERIPHERAL_CLOCK_FREQ_MHZ * 300) / 1000) + 1;
    }
    I2C_Reg->TRISE = (trise_value & 0x3F);

    // Configure Own Address Register 1 (OAR1)
    I2C_Reg->OAR1 = (I2C1_OWN_ADDRESS_1 << 1);
    I2C_Reg->OAR1 |= (1U << 14); // Should be kept high by software

    // Enable the peripheral
    I2C_Reg->CR1 |= (1U << I2C_CR1_PE_BIT);

    return I2C_STATUS_OK;
}

/*******************************************************************************************************/
/* I2C_Enable                                                                                          */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Enable I2C peripheral                                                       */
/* Function Input       -> I2C Instance                                                                */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/
I2C_Status_t I2C_Enable(I2C_Instance_t Copy_Instance)
{
    volatile I2C_Registers_t* I2C_Reg = I2C_GetRegisterPointer(Copy_Instance);
    if (I2C_Reg == NULL)
    {
        return I2C_STATUS_INVALID_INSTANCE;
    }
    I2C_Reg->CR1 |= (1U << I2C_CR1_PE_BIT);
    return I2C_STATUS_OK;
}

/*******************************************************************************************************/
/* I2C_Disable                                                                                         */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Disable I2C peripheral                                                      */
/* Function Input       -> I2C Instance                                                                */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/
I2C_Status_t I2C_Disable(I2C_Instance_t Copy_Instance)
{
    volatile I2C_Registers_t* I2C_Reg = I2C_GetRegisterPointer(Copy_Instance);
    if (I2C_Reg == NULL)
    {
        return I2C_STATUS_INVALID_INSTANCE;
    }
    I2C_Reg->CR1 &= ~(1U << I2C_CR1_PE_BIT);
    return I2C_STATUS_OK;
}

/*******************************************************************************************************/
/* I2C_Master_Transmit                                                                                 */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Transmit data as master                                                     */
/* Function Input       -> Instance, Slave Address, Data, Size, Timeout                                */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/
I2C_Status_t I2C_Master_Transmit(I2C_Instance_t Copy_Instance, uint8_t Copy_u8SlaveAddress, uint8_t* Copy_pu8Data, uint16_t Copy_u16Size, uint32_t Copy_u32Timeout)
{
    volatile I2C_Registers_t* I2C_Reg = I2C_GetRegisterPointer(Copy_Instance);
    if (I2C_Reg == NULL) return I2C_STATUS_INVALID_INSTANCE;

    uint32_t timeout = Copy_u32Timeout;

    // Generate START condition
    I2C_Reg->CR1 |= (1U << I2C_CR1_START_BIT);
    while (!((I2C_Reg->SR1 >> I2C_SR1_SB_BIT) & 1U))
    {
        if (--timeout == 0) return I2C_STATUS_TIMEOUT;
    }

    // Send slave address with write bit
    I2C_Reg->DR = (Copy_u8SlaveAddress << 1) & ~1U;
    timeout = Copy_u32Timeout;
    while (!((I2C_Reg->SR1 >> I2C_SR1_ADDR_BIT) & 1U))
    {
        if (--timeout == 0) return I2C_STATUS_TIMEOUT;
    }
    // Clear ADDR flag by reading SR1 and SR2
    (void)I2C_Reg->SR2;

    // Send data
    for (uint16_t i = 0; i < Copy_u16Size; i++)
    {
        timeout = Copy_u32Timeout;
        while (!((I2C_Reg->SR1 >> I2C_SR1_TXE_BIT) & 1U))
        {
            if (--timeout == 0) return I2C_STATUS_TIMEOUT;
        }
        I2C_Reg->DR = Copy_pu8Data[i];
    }

    // Wait for BTF
    timeout = Copy_u32Timeout;
    while (!((I2C_Reg->SR1 >> I2C_SR1_BTF_BIT) & 1U))
    {
        if (--timeout == 0) return I2C_STATUS_TIMEOUT;
    }

    // Generate STOP condition
    I2C_Reg->CR1 |= (1U << I2C_CR1_STOP_BIT);

    return I2C_STATUS_OK;
}


/*******************************************************************************************************/
/* I2C_Master_Receive                                                                                  */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Receive data as master                                                      */
/* Function Input       -> Instance, Slave Address, Data buffer, Size, Timeout                         */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/
I2C_Status_t I2C_Master_Receive(I2C_Instance_t Copy_Instance, uint8_t Copy_u8SlaveAddress, uint8_t* Copy_pu8Data, uint16_t Copy_u16Size, uint32_t Copy_u32Timeout)
{
    volatile I2C_Registers_t* I2C_Reg = I2C_GetRegisterPointer(Copy_Instance);
    if (I2C_Reg == NULL) return I2C_STATUS_INVALID_INSTANCE;

    uint32_t timeout = Copy_u32Timeout;

    // Enable ACK
    I2C_Reg->CR1 |= (1U << I2C_CR1_ACK_BIT);

    // Generate START condition
    I2C_Reg->CR1 |= (1U << I2C_CR1_START_BIT);
    while (!((I2C_Reg->SR1 >> I2C_SR1_SB_BIT) & 1U))
    {
        if (--timeout == 0) return I2C_STATUS_TIMEOUT;
    }

    // Send slave address with read bit
    I2C_Reg->DR = (Copy_u8SlaveAddress << 1) | 1U;
    timeout = Copy_u32Timeout;
    while (!((I2C_Reg->SR1 >> I2C_SR1_ADDR_BIT) & 1U))
    {
        if (--timeout == 0) return I2C_STATUS_TIMEOUT;
    }
    // Clear ADDR flag
    (void)I2C_Reg->SR2;

    // Receive data
    for (uint16_t i = 0; i < Copy_u16Size; i++)
    {
        if (i == Copy_u16Size - 1)
        {
            // Disable ACK for the last byte
            I2C_Reg->CR1 &= ~(1U << I2C_CR1_ACK_BIT);
        }
        timeout = Copy_u32Timeout;
        while (!((I2C_Reg->SR1 >> I2C_SR1_RXNE_BIT) & 1U))
        {
            if (--timeout == 0) return I2C_STATUS_TIMEOUT;
        }
        Copy_pu8Data[i] = I2C_Reg->DR;
    }

    // Generate STOP condition
    I2C_Reg->CR1 |= (1U << I2C_CR1_STOP_BIT);

    return I2C_STATUS_OK;
}

// Dummy implementations for Slave mode and Callbacks
I2C_Status_t I2C_Slave_Transmit(I2C_Instance_t Copy_Instance, uint8_t* Copy_pu8Data, uint16_t Copy_u16Size, uint32_t Copy_u32Timeout){return I2C_STATUS_OK;}
I2C_Status_t I2C_Slave_Receive(I2C_Instance_t Copy_Instance, uint8_t* Copy_pu8Data, uint16_t Copy_u16Size, uint32_t Copy_u32Timeout){return I2C_STATUS_OK;}


/*******************************************************************************************************/
/* I2C_Set_EventCallback                                                                               */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Set event callback function for I2C interrupt                              */
/* Function Input       -> I2C Instance, Event callback function pointer                              */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

I2C_Status_t I2C_Set_EventCallback(I2C_Instance_t Copy_Instance, void(*Copy_Fptr)(void))
{
    if (Copy_Fptr == NULL)
    {
        return I2C_STATUS_ERROR;
    }

    switch (Copy_Instance)
    {
        case I2C_INSTANCE_1:
            I2C1_EventCallbackPtr = Copy_Fptr;
            break;
        case I2C_INSTANCE_2:
            I2C2_EventCallbackPtr = Copy_Fptr;
            break;
        case I2C_INSTANCE_3:
            I2C3_EventCallbackPtr = Copy_Fptr;
            break;
        default:
            return I2C_STATUS_INVALID_INSTANCE;
    }

    return I2C_STATUS_OK;
}

/*******************************************************************************************************/
/* I2C_Set_ErrorCallback                                                                               */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Set error callback function for I2C interrupt                              */
/* Function Input       -> I2C Instance, Error callback function pointer                              */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

I2C_Status_t I2C_Set_ErrorCallback(I2C_Instance_t Copy_Instance, void(*Copy_Fptr)(void))
{
    if (Copy_Fptr == NULL)
    {
        return I2C_STATUS_ERROR;
    }

    switch (Copy_Instance)
    {
        case I2C_INSTANCE_1:
            I2C1_ErrorCallbackPtr = Copy_Fptr;
            break;
        case I2C_INSTANCE_2:
            I2C2_ErrorCallbackPtr = Copy_Fptr;
            break;
        case I2C_INSTANCE_3:
            I2C3_ErrorCallbackPtr = Copy_Fptr;
            break;
        default:
            return I2C_STATUS_INVALID_INSTANCE;
    }

    return I2C_STATUS_OK;
}

/*******************************************************************************************************/
/* Interrupt Service Routines                                                                          */
/*******************************************************************************************************/
void I2C1_EV_IRQHandler(void)
{
    if (I2C1_EventCallbackPtr != NULL)
    {
        I2C1_EventCallbackPtr();
    }
}

void I2C1_ER_IRQHandler(void)
{
    if (I2C1_ErrorCallbackPtr != NULL)
    {
        I2C1_ErrorCallbackPtr();
    }
}
