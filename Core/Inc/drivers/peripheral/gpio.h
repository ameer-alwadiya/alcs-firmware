/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 16 Jul 2025                                                                     */
/* Description       : gpio.h --> GPIO driver for STM32F401xE                                          */
/* Module Features   :                                                                                 */
/*      01- GPIO_SetPinMode()                                                                          */
/*      02- GPIO_SetOutputType()                                                                       */
/*      03- GPIO_SetOutputSpeed()                                                                      */
/*      04- GPIO_SetPullState()                                                                        */
/*      05- GPIO_GetPinValue()                                                                         */
/*      06- GPIO_SetPinValue()                                                                         */
/*      07- GPIO_DirectSetReset()                                                                      */
/*      08- GPIO_PinLock()                                                                             */
/*      09- GPIO_SetPinAltFunction()                                                                   */
/*******************************************************************************************************/

/*******************************************************************************************************/
/*                                   Header guard                                                      */
/*******************************************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

/*******************************************************************************************************/
/*                                      Standard Types                                                 */
/*******************************************************************************************************/

#include <stdint.h>

/*******************************************************************************************************/
/*                                      Configuration Section                                          */
/*******************************************************************************************************/

/* Configuration parameters can be added here as needed */

/*******************************************************************************************************/
/*                                    GPIO Base Addresses                                              */
/*******************************************************************************************************/

#define  GPIOA_BASE_ADDRESS         0x40020000UL
#define  GPIOB_BASE_ADDRESS         0x40020400UL
#define  GPIOC_BASE_ADDRESS         0x40020800UL
#define  GPIOD_BASE_ADDRESS 		0x40020C00UL
#define  GPIOE_BASE_ADDRESS 		0x40021000UL
#define  GPIOH_BASE_ADDRESS 		0x40021C00UL

/*******************************************************************************************************/
/*                                    GPIO Register Structure                                          */
/*******************************************************************************************************/

typedef struct {
	volatile uint32_t MODER;   /*!< GPIO port mode register,               Address offset: 0x00      */
	volatile uint32_t OTYPER;  /*!< GPIO port output type register,        Address offset: 0x04      */
	volatile uint32_t OSPEEDR; /*!< GPIO port output speed register,       Address offset: 0x08      */
	volatile uint32_t PUPDR;   /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
	volatile uint32_t IDR;     /*!< GPIO port input data register,         Address offset: 0x10      */
	volatile uint32_t ODR;     /*!< GPIO port output data register,        Address offset: 0x14      */
	volatile uint32_t BSRR;    /*!< GPIO port bit set/reset register,      Address offset: 0x18      */
	volatile uint32_t LCKR;    /*!< GPIO port configuration lock register, Address offset: 0x1C      */
	volatile uint32_t AFRL;    /*!< GPIO alternate function low register,  Address offset: 0x20      */
	volatile uint32_t AFRH;    /*!< GPIO alternate function high register, Address offset: 0x24      */
} GPIO_RegDef_t;

/*******************************************************************************************************/
/*                                   GPIO Peripheral Definitions                                       */
/*******************************************************************************************************/

#define GPIOA        ((GPIO_RegDef_t *) GPIOA_BASE_ADDRESS)
#define GPIOB        ((GPIO_RegDef_t *) GPIOB_BASE_ADDRESS)
#define GPIOC        ((GPIO_RegDef_t *) GPIOC_BASE_ADDRESS)
#define GPIOD        ((GPIO_RegDef_t *) GPIOD_BASE_ADDRESS)
#define GPIOE        ((GPIO_RegDef_t *) GPIOE_BASE_ADDRESS)
#define GPIOH        ((GPIO_RegDef_t *) GPIOH_BASE_ADDRESS)

/*******************************************************************************************************/
/*                                      Port Definitions                                               */
/*******************************************************************************************************/

#define   GPIO_PORT_A      0U
#define   GPIO_PORT_B      1U
#define   GPIO_PORT_C      2U
#define   GPIO_PORT_D      3U
#define   GPIO_PORT_E      4U
#define   GPIO_PORT_H      5U

/*******************************************************************************************************/
/*                                      Pin Definitions                                                */
/*******************************************************************************************************/

#define  GPIO_PIN_0        0U
#define  GPIO_PIN_1        1U
#define  GPIO_PIN_2        2U
#define  GPIO_PIN_3        3U
#define  GPIO_PIN_4        4U
#define  GPIO_PIN_5        5U
#define  GPIO_PIN_6        6U
#define  GPIO_PIN_7        7U
#define  GPIO_PIN_8        8U
#define  GPIO_PIN_9        9U
#define  GPIO_PIN_10      10U
#define  GPIO_PIN_11      11U
#define  GPIO_PIN_12      12U
#define  GPIO_PIN_13      13U
#define  GPIO_PIN_14      14U
#define  GPIO_PIN_15      15U

/*******************************************************************************************************/
/*                                      Mode Definitions                                               */
/*******************************************************************************************************/

#define  GPIO_MODE_INPUT       (0b00)
#define  GPIO_MODE_OUTPUT      (0b01)
#define  GPIO_MODE_ALTF        (0b10)
#define  GPIO_MODE_ANALOG      (0b11)

/*******************************************************************************************************/
/*                                      Output Type Definitions                                        */
/*******************************************************************************************************/

#define   GPIO_OUTPUT_TYPE_PUSH_PULL          0U
#define   GPIO_OUTPUT_TYPE_OPEN_DRAIN         1U

/*******************************************************************************************************/
/*                                      Speed Definitions                                              */
/*******************************************************************************************************/

#define GPIO_OUTPUT_SPEED_LOW               (0b00)
#define GPIO_OUTPUT_SPEED_MEDIUM            (0b01)
#define GPIO_OUTPUT_SPEED_HIGH              (0b10)
#define GPIO_OUTPUT_SPEED_VERY_HIGH         (0b11)

/*******************************************************************************************************/
/*                                      Pull State Definitions                                         */
/*******************************************************************************************************/

#define GPIO_PULL_FLOATING                   (0b00)
#define GPIO_PULL_UP                         (0b01)
#define GPIO_PULL_DOWN                       (0b10)

/*******************************************************************************************************/
/*                                      Pin Value Definitions                                          */
/*******************************************************************************************************/

#define GPIO_PIN_HIGH                            1U
#define GPIO_PIN_LOW                             0U

/*******************************************************************************************************/
/*                                      Alternate Function Definitions                                 */
/*******************************************************************************************************/

 #define GPIO_ALTFN_0                          (0b0000)
 #define GPIO_ALTFN_1                          (0b0001)
 #define GPIO_ALTFN_2                          (0b0010)
 #define GPIO_ALTFN_3                          (0b0011)
 #define GPIO_ALTFN_4                          (0b0100)
 #define GPIO_ALTFN_5                          (0b0101)
 #define GPIO_ALTFN_6                          (0b0110)
 #define GPIO_ALTFN_7                          (0b0111)
 #define GPIO_ALTFN_8                          (0b1000)
 #define GPIO_ALTFN_9                          (0b1001)
 #define GPIO_ALTFN_10                         (0b1010)
 #define GPIO_ALTFN_11                         (0b1011)
 #define GPIO_ALTFN_12                         (0b1100)
 #define GPIO_ALTFN_13                         (0b1101)
 #define GPIO_ALTFN_14                         (0b1110)
 #define GPIO_ALTFN_15                         (0b1111)

/*******************************************************************************************************/
/*                                    Private Macros                                                   */
/*******************************************************************************************************/

#define LOCK_PIN_BIT    16U

/*******************************************************************************************************/
/*                                      Function Prototypes                                            */
/*******************************************************************************************************/

/*******************************************************************************************************/
/*                                      01- GPIO_SetPinMode                                            */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Sets the mode of a GPIO pin (Input/Output/Alternate/Analog)                */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         mode: Pin mode (GPIO_MODE_INPUT/OUTPUT/ALTF/ANALOG)                        */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_SetPinMode(uint8_t port_name, uint8_t pin_num, uint8_t mode);

/*******************************************************************************************************/
/*                                      02- GPIO_SetOutputType                                         */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Sets the output type of a GPIO pin (Push-Pull/Open-Drain)                  */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         output_type: Output type (GPIO_OUTPUT_TYPE_PUSH_PULL/OPEN_DRAIN)           */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_SetOutputType(uint8_t port_name, uint8_t pin_num, uint8_t output_type);

/*******************************************************************************************************/
/*                                      03- GPIO_SetOutputSpeed                                        */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Sets the output speed of a GPIO pin                                         */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         speed_mode: Speed setting (GPIO_OUTPUT_SPEED_LOW/MEDIUM/HIGH/VERY_HIGH)    */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_SetOutputSpeed(uint8_t port_name, uint8_t pin_num, uint8_t speed_mode);

/*******************************************************************************************************/
/*                                      04- GPIO_SetPullState                                          */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Sets the pull-up/pull-down resistor state of a GPIO pin                    */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         pull_type: Pull resistor type (GPIO_PULL_FLOATING/UP/DOWN)                 */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_SetPullState(uint8_t port_name, uint8_t pin_num, uint8_t pull_type);

/*******************************************************************************************************/
/*                                      05- GPIO_GetPinValue                                           */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Reads the value of a GPIO pin from Input Data Register                     */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/* Function Return      -> Pin state value (GPIO_PIN_HIGH/GPIO_PIN_LOW)                               */
/*******************************************************************************************************/

uint8_t GPIO_GetPinValue(uint8_t port_name, uint8_t pin_num);

/*******************************************************************************************************/
/*                                      06- GPIO_SetPinValue                                           */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Sets the value of a GPIO pin in Output Data Register                       */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         pin_value: Pin value (GPIO_PIN_HIGH/GPIO_PIN_LOW)                          */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_SetPinValue(uint8_t port_name, uint8_t pin_num, uint8_t pin_value);

/*******************************************************************************************************/
/*                                      07- GPIO_DirectSetReset                                        */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Directly sets or resets a GPIO pin using Bit Set/Reset Register            */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         pin_value: Pin value (GPIO_PIN_HIGH/GPIO_PIN_LOW)                          */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_DirectSetReset(uint8_t port_name, uint8_t pin_num, uint8_t pin_value);

/*******************************************************************************************************/
/*                                      08- GPIO_PinLock                                               */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Locks the configuration of a GPIO pin until next system reset              */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_PinLock(uint8_t port_name, uint8_t pin_num);

/*******************************************************************************************************/
/*                                      09- GPIO_SetPinAltFunction                                     */
/*-----------------------------------------------------------------------------------------------------*/
/* Function Description -> Sets the alternate function of a GPIO pin                                   */
/* Function Input       -> port_name: GPIO port (GPIO_PORT_A to GPIO_PORT_H)                          */
/*                         pin_num: Pin number (GPIO_PIN_0 to GPIO_PIN_15)                            */
/*                         alt_function: Alternate function (GPIO_ALTFN_0 to GPIO_ALTFN_15)           */
/* Function Return      -> None                                                                        */
/*******************************************************************************************************/

void GPIO_SetPinAltFunction(uint8_t port_name, uint8_t pin_num, uint8_t alt_function);

#endif /* GPIO_H_ */
