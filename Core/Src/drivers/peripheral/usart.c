/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.2                                                                          */
/* Date              : 01 Aug 2025                                                                     */
/* Description       : usart.c --> USART Implementation File (Fixed BRR Calculation)                  */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* Include Files                                                                                       */
/*******************************************************************************************************/

#include "usart.h"
#include <stdio.h>
#include <stdint.h>

/*******************************************************************************************************/
/* Global Variables                                                                                    */
/*******************************************************************************************************/

static void(*USART1_CallbackPtr)(void) = NULL;
static void(*USART2_CallbackPtr)(void) = NULL;

/*******************************************************************************************************/
/* Helper Functions                                                                                    */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* USART_GetRegisterPointer                                                                            */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Get USART register pointer for given instance                              */
/* Function Input       -> USART Instance                                                              */
/* Function Return      -> Register pointer or NULL if invalid                                         */
/*******************************************************************************************************/

static volatile USART_Registers_t* USART_GetRegisterPointer(USART_Instance_t Copy_Instance)
{
    switch (Copy_Instance)
    {
        case USART_INSTANCE_1:
            return USART1_REG;
        case USART_INSTANCE_2:
            return USART2_REG;
        default:
            return NULL;
    }
}


/*******************************************************************************************************/
/* Function Implementations                                                                            */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* USART_Initialize                                                                                    */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Initialize USART peripheral                                                 */
/* Function Input       -> USART Instance                                                              */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* USART_Initialize - Fixed Version for Custom RCC Implementation                                      */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Initialize USART peripheral with proper BRR calculation                    */
/* Function Input       -> USART Instance                                                              */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

USART_Status_t USART_Initialize(USART_Instance_t Copy_Instance)
{
    volatile USART_Registers_t* USART_Reg;
    uint32_t baudRate, wordLength, stopBits, parity, oversampling;

    uint32_t periph_clk = 16000000;

    /* Get register pointer */
    USART_Reg = USART_GetRegisterPointer(Copy_Instance);
    if (USART_Reg == NULL)
    {
        return USART_STATUS_INVALID_INSTANCE;
    }


    /* Get instance-specific configuration */
    switch (Copy_Instance)
    {
        case USART_INSTANCE_1:
            baudRate = USART1_BAUD_RATE;
            wordLength = USART1_WORD_LENGTH;
            stopBits = USART1_STOP_BITS;
            parity = USART1_PARITY;
            oversampling = USART1_OVERSAMPLING;
            break;

        case USART_INSTANCE_2:
            baudRate = USART2_BAUD_RATE;
            wordLength = USART2_WORD_LENGTH;
            stopBits = USART2_STOP_BITS;
            parity = USART2_PARITY;
            oversampling = USART2_OVERSAMPLING;
            break;

        default:
            return USART_STATUS_INVALID_INSTANCE;
    }

    /* Disable USART during configuration */
    USART_Reg->CR1.UE = 0;

    /* Set the calculated baud rate */
//    USART_Reg->BRR = 0x008B;
    USART_Reg->BRR = ((periph_clk + (baudRate/2U))/baudRate);

    /* Clear and configure CR1 register with instance-specific settings */
    /* Reset relevant bits first */
    USART_Reg->CR1.M = 0;          // Clear word length
    USART_Reg->CR1.PCE = 0;        // Clear parity control enable
    USART_Reg->CR1.PS = 0;         // Clear parity selection
    USART_Reg->CR1.OVER8 = 0;      // Clear oversampling
    USART_Reg->CR1.TE = 0;         // Clear transmitter enable
    USART_Reg->CR1.RE = 0;         // Clear receiver enable
    USART_Reg->CR1.RXNEIE = 0;     // Clear RX interrupt enable

    /* Configure oversampling mode */
    USART_Reg->CR1.OVER8 = oversampling;

    /* Configure word length */
    USART_Reg->CR1.M = wordLength;

    /* Configure parity */
    if (parity != USART_PARITY_NONE)
    {
        USART_Reg->CR1.PCE = 1;              // Enable parity control
        if (parity == USART_PARITY_ODD)
        {
            USART_Reg->CR1.PS = 1;           // Set odd parity
        }
        else // USART_PARITY_EVEN
        {
            USART_Reg->CR1.PS = 0;           // Set even parity
        }
    }

    /* Configure stop bits in CR2 register */
    USART_Reg->CR2 &= ~(3U << USART_CR2_STOP_BIT12); // Clear stop bits field (bits 13:12)
    if (stopBits == USART_STOP_BITS_2)
    {
        USART_Reg->CR2 |= (2U << USART_CR2_STOP_BIT12); // Set 2 stop bits (10)
    }
    /* For 1 stop bit, keep the field as 00 (already cleared above) */

    /* Enable transmitter and receiver */
    USART_Reg->CR1.TE = 1;         // Enable transmitter
    USART_Reg->CR1.RE = 1;         // Enable receiver

    /* Enable receive interrupt */
    USART_Reg->CR1.RXNEIE = 1;     // Enable RX not empty interrupt

    /* Clear any pending flags by reading SR and DR */
    volatile uint32_t temp;
    temp = USART_Reg->SR;           // Read SR to clear flags
    temp = USART_Reg->DR;           // Read DR to clear RXNE flag
    (void)temp;                     // Prevent unused variable warning

    return USART_STATUS_OK;
}

/*******************************************************************************************************/
/* USART_SendData                                                                                      */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Send data through USART                                                     */
/* Function Input       -> USART Instance, Data pointer, Data length                                  */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

USART_Status_t USART_SendData(USART_Instance_t Copy_Instance, uint8_t* Copy_u8Data, uint8_t Copy_u8Length)
{
    volatile USART_Registers_t* USART_Reg;
    uint32_t timeout;

    /* Validate inputs */
    if (Copy_u8Data == NULL || Copy_u8Length == 0)
    {
        return USART_STATUS_ERROR;
    }

    /* Get register pointer */
    USART_Reg = USART_GetRegisterPointer(Copy_Instance);
    if (USART_Reg == NULL)
    {
        return USART_STATUS_INVALID_INSTANCE;
    }

    /* Send data byte by byte */
    for (uint8_t i = 0; i < Copy_u8Length; i++)
    {
        /* Load data into data register */
        USART_Reg->DR = Copy_u8Data[i];

        /* Wait until transmission complete with timeout */
        timeout = USART_TIMEOUT_VALUE;
        while (((USART_Reg->SR >> USART_SR_TC_BIT) & 1) == 0)
        {
            if (--timeout == 0)
            {
                return USART_STATUS_TIMEOUT;
            }
        }
    }

    return USART_STATUS_OK;
}

/*******************************************************************************************************/
/* USART_ReadData                                                                                      */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Read data from USART (should only be called from interrupt)                */
/* Function Input       -> USART Instance                                                              */
/* Function Return      -> Received data byte                                                          */
/*******************************************************************************************************/

uint8_t USART_ReadData(USART_Instance_t Copy_Instance)
{
    volatile USART_Registers_t* USART_Reg;

    /* Get register pointer */
    USART_Reg = USART_GetRegisterPointer(Copy_Instance);
    if (USART_Reg == NULL)
    {
        return USART_STATUS_INVALID_INSTANCE;
    }

    /* Return received data (reading DR clears RXNE flag) */
    return (uint8_t)USART_Reg->DR;
}

/*******************************************************************************************************/
/* USART_Enable                                                                                        */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Enable USART peripheral                                                     */
/* Function Input       -> USART Instance                                                              */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

USART_Status_t USART_Enable(USART_Instance_t Copy_Instance)
{
    volatile USART_Registers_t* USART_Reg;

    /* Get register pointer */
    USART_Reg = USART_GetRegisterPointer(Copy_Instance);
    if (USART_Reg == NULL)
    {
        return USART_STATUS_INVALID_INSTANCE;
    }

    /* Enable USART */
    USART_Reg->CR1.UE = 1;

    return USART_STATUS_OK;
}

/*******************************************************************************************************/
/* USART_Disable                                                                                       */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Disable USART peripheral                                                    */
/* Function Input       -> USART Instance                                                              */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

USART_Status_t USART_Disable(USART_Instance_t Copy_Instance)
{
    volatile USART_Registers_t* USART_Reg;

    /* Get register pointer */
    USART_Reg = USART_GetRegisterPointer(Copy_Instance);
    if (USART_Reg == NULL)
    {
        return USART_STATUS_INVALID_INSTANCE;
    }

    /* Disable USART */
    USART_Reg->CR1.UE = 0;

    return USART_STATUS_OK;
}

/*******************************************************************************************************/
/* USART_SetCallback                                                                                   */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Set callback function for USART interrupt                                   */
/* Function Input       -> USART Instance, Function pointer                                            */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

USART_Status_t USART_SetCallback(USART_Instance_t Copy_Instance, void(*Copy_Fptr)(void))
{
    if (Copy_Fptr == NULL)
    {
        return USART_STATUS_ERROR;
    }

    switch (Copy_Instance)
    {
        case USART_INSTANCE_1:
            USART1_CallbackPtr = Copy_Fptr;
            break;
        case USART_INSTANCE_2:
            USART2_CallbackPtr = Copy_Fptr;
            break;
        default:
            return USART_STATUS_INVALID_INSTANCE;
    }

    return USART_STATUS_OK;
}

/*******************************************************************************************************/
/* Interrupt Service Routines                                                                          */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* USART1_IRQHandler                                                                                   */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> USART1 interrupt handler                                                    */
/* Function Input       -> None                                                                        */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void USART1_IRQHandler(void)
{
    /* Check if RXNE interrupt occurred */
    if (((USART1_REG->SR >> USART_SR_RXNE_BIT) & 1) == 1)
    {
        if (USART1_CallbackPtr != NULL)
        {
            USART1_CallbackPtr();
        }
    }
}

/*******************************************************************************************************/
/* USART2_IRQHandler                                                                                   */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> USART2 interrupt handler                                                    */
/* Function Input       -> None                                                                        */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void USART2_IRQHandler(void)
{
    /* Check if RXNE interrupt occurred */
    if (((USART2_REG->SR >> USART_SR_RXNE_BIT) & 1) == 1)
    {
        if (USART2_CallbackPtr != NULL)
        {
            USART2_CallbackPtr();
        }
    }
}

/*******************************************************************************************************/
/* Printf Support Functions                                                                            */
/*******************************************************************************************************/

/*******************************************************************************************************/
/* USART_WriteByte                                                                                     */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Write single byte to USART (for printf support)                            */
/* Function Input       -> USART Instance, Data byte                                                   */
/* Function Return      -> Status                                                                      */
/*******************************************************************************************************/

USART_Status_t USART_WriteByte(USART_Instance_t Copy_Instance, uint8_t Copy_u8Data)
{
    volatile USART_Registers_t* USART_Reg;
    uint32_t timeout;

    /* Get register pointer */
    USART_Reg = USART_GetRegisterPointer(Copy_Instance);
    if (USART_Reg == NULL)
    {
        return USART_STATUS_INVALID_INSTANCE;
    }

    /* Wait until transmit data register is empty with timeout */
    timeout = USART_TIMEOUT_VALUE;
    while (((USART_Reg->SR >> USART_SR_TXE_BIT) & 1) == 0)
    {
        if (--timeout == 0)
        {
            return USART_STATUS_TIMEOUT;
        }
    }

    /* Write data to transmit register */
    USART_Reg->DR = (Copy_u8Data & 0xFF);

    return USART_STATUS_OK;
}

/*******************************************************************************************************/
/* __io_putchar                                                                                        */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Redirect printf output to USART1                                           */
/* Function Input       -> Character to send                                                           */
/* Function Return      -> Character sent                                                              */
/*******************************************************************************************************/

int __io_putchar(int ch)
{
    USART_WriteByte(USART_INSTANCE_1, (uint8_t)ch);
    return ch;
}
