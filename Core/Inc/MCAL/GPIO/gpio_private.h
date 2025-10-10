/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                  */
/* Version           : V1.0.0                                                                          */
/* Date              : 16 Jul 2025                                                                     */
/* Description       : gpio_private.h --> Register definitions and private declarations               */
/* Features          : Register definitions and hardware abstraction                                   */
/*******************************************************************************************************/

/*******************************************************************************************************/
/*                                   Header guard                                                      */
/*******************************************************************************************************/

#ifndef GPIO_PRIVATE_H_
#define GPIO_PRIVATE_H_

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
/*                                    GPIOA Register Map                                               */
/*******************************************************************************************************/

#define GPIOA_MODER            (*((volatile uint32_t *)(GPIOA_BASE_ADDRESS+0x00)))
#define GPIOA_OTYPER           (*((volatile uint32_t *)(GPIOA_BASE_ADDRESS+0x04)))
#define GPIOA_OSPEEDR          (*((volatile uint32_t *)(GPIOA_BASE_ADDRESS+0x08)))
#define GPIOA_PUPDR            (*((volatile uint32_t *)(GPIOA_BASE_ADDRESS+0x0C)))
#define GPIOA_IDR              (*((volatile uint32_t *)(GPIOA_BASE_ADDRESS+0x10)))
#define GPIOA_ODR              (*((volatile uint32_t *)(GPIOA_BASE_ADDRESS+0x14)))
#define GPIOA_BSRR             (*((volatile uint32_t *)(GPIOA_BASE_ADDRESS+0x18)))
#define GPIOA_LCKR             (*((volatile uint32_t *)(GPIOA_BASE_ADDRESS+0x1C)))
#define GPIOA_AFRL             (*((volatile uint32_t *)(GPIOA_BASE_ADDRESS+0x20)))
#define GPIOA_AFRH             (*((volatile uint32_t *)(GPIOA_BASE_ADDRESS+0x24)))

/*******************************************************************************************************/
/*                                    GPIOB Register Map                                               */
/*******************************************************************************************************/

#define GPIOB_MODER            (*((volatile uint32_t *)(GPIOB_BASE_ADDRESS+0x00)))
#define GPIOB_OTYPER           (*((volatile uint32_t *)(GPIOB_BASE_ADDRESS+0x04)))
#define GPIOB_OSPEEDR          (*((volatile uint32_t *)(GPIOB_BASE_ADDRESS+0x08)))
#define GPIOB_PUPDR            (*((volatile uint32_t *)(GPIOB_BASE_ADDRESS+0x0C)))
#define GPIOB_IDR              (*((volatile uint32_t *)(GPIOB_BASE_ADDRESS+0x10)))
#define GPIOB_ODR              (*((volatile uint32_t *)(GPIOB_BASE_ADDRESS+0x14)))
#define GPIOB_BSRR             (*((volatile uint32_t *)(GPIOB_BASE_ADDRESS+0x18)))
#define GPIOB_LCKR             (*((volatile uint32_t *)(GPIOB_BASE_ADDRESS+0x1C)))
#define GPIOB_AFRL             (*((volatile uint32_t *)(GPIOB_BASE_ADDRESS+0x20)))
#define GPIOB_AFRH             (*((volatile uint32_t *)(GPIOB_BASE_ADDRESS+0x24)))

/*******************************************************************************************************/
/*                                    GPIOC Register Map                                               */
/*******************************************************************************************************/

#define GPIOC_MODER            (*((volatile uint32_t *)(GPIOC_BASE_ADDRESS+0x00)))
#define GPIOC_OTYPER           (*((volatile uint32_t *)(GPIOC_BASE_ADDRESS+0x04)))
#define GPIOC_OSPEEDR          (*((volatile uint32_t *)(GPIOC_BASE_ADDRESS+0x08)))
#define GPIOC_PUPDR            (*((volatile uint32_t *)(GPIOC_BASE_ADDRESS+0x0C)))
#define GPIOC_IDR              (*((volatile uint32_t *)(GPIOC_BASE_ADDRESS+0x10)))
#define GPIOC_ODR              (*((volatile uint32_t *)(GPIOC_BASE_ADDRESS+0x14)))
#define GPIOC_BSRR             (*((volatile uint32_t *)(GPIOC_BASE_ADDRESS+0x18)))
#define GPIOC_LCKR             (*((volatile uint32_t *)(GPIOC_BASE_ADDRESS+0x1C)))
#define GPIOC_AFRL             (*((volatile uint32_t *)(GPIOC_BASE_ADDRESS+0x20)))
#define GPIOC_AFRH             (*((volatile uint32_t *)(GPIOC_BASE_ADDRESS+0x24)))

/*******************************************************************************************************/
/*                                    GPIOD Register Map                                               */
/*******************************************************************************************************/

#define GPIOD_MODER            (*((volatile uint32_t *)(GPIOD_BASE_ADDRESS+0x00)))
#define GPIOD_OTYPER           (*((volatile uint32_t *)(GPIOD_BASE_ADDRESS+0x04)))
#define GPIOD_OSPEEDR          (*((volatile uint32_t *)(GPIOD_BASE_ADDRESS+0x08)))
#define GPIOD_PUPDR            (*((volatile uint32_t *)(GPIOD_BASE_ADDRESS+0x0C)))
#define GPIOD_IDR              (*((volatile uint32_t *)(GPIOD_BASE_ADDRESS+0x10)))
#define GPIOD_ODR              (*((volatile uint32_t *)(GPIOD_BASE_ADDRESS+0x14)))
#define GPIOD_BSRR             (*((volatile uint32_t *)(GPIOD_BASE_ADDRESS+0x18)))
#define GPIOD_LCKR             (*((volatile uint32_t *)(GPIOD_BASE_ADDRESS+0x1C)))
#define GPIOD_AFRL             (*((volatile uint32_t *)(GPIOD_BASE_ADDRESS+0x20)))
#define GPIOD_AFRH             (*((volatile uint32_t *)(GPIOD_BASE_ADDRESS+0x24)))

/*******************************************************************************************************/
/*                                    GPIOE Register Map                                               */
/*******************************************************************************************************/

#define GPIOE_MODER            (*((volatile uint32_t *)(GPIOE_BASE_ADDRESS+0x00)))
#define GPIOE_OTYPER           (*((volatile uint32_t *)(GPIOE_BASE_ADDRESS+0x04)))
#define GPIOE_OSPEEDR          (*((volatile uint32_t *)(GPIOE_BASE_ADDRESS+0x08)))
#define GPIOE_PUPDR            (*((volatile uint32_t *)(GPIOE_BASE_ADDRESS+0x0C)))
#define GPIOE_IDR              (*((volatile uint32_t *)(GPIOE_BASE_ADDRESS+0x10)))
#define GPIOE_ODR              (*((volatile uint32_t *)(GPIOE_BASE_ADDRESS+0x14)))
#define GPIOE_BSRR             (*((volatile uint32_t *)(GPIOE_BASE_ADDRESS+0x18)))
#define GPIOE_LCKR             (*((volatile uint32_t *)(GPIOE_BASE_ADDRESS+0x1C)))
#define GPIOE_AFRL             (*((volatile uint32_t *)(GPIOE_BASE_ADDRESS+0x20)))
#define GPIOE_AFRH             (*((volatile uint32_t *)(GPIOE_BASE_ADDRESS+0x24)))

/*******************************************************************************************************/
/*                                    GPIOH Register Map                                               */
/*******************************************************************************************************/

#define GPIOH_MODER            (*((volatile uint32_t *)(GPIOH_BASE_ADDRESS+0x00)))
#define GPIOH_OTYPER           (*((volatile uint32_t *)(GPIOH_BASE_ADDRESS+0x04)))
#define GPIOH_OSPEEDR          (*((volatile uint32_t *)(GPIOH_BASE_ADDRESS+0x08)))
#define GPIOH_PUPDR            (*((volatile uint32_t *)(GPIOH_BASE_ADDRESS+0x0C)))
#define GPIOH_IDR              (*((volatile uint32_t *)(GPIOH_BASE_ADDRESS+0x10)))
#define GPIOH_ODR              (*((volatile uint32_t *)(GPIOH_BASE_ADDRESS+0x14)))
#define GPIOH_BSRR             (*((volatile uint32_t *)(GPIOH_BASE_ADDRESS+0x18)))
#define GPIOH_LCKR             (*((volatile uint32_t *)(GPIOH_BASE_ADDRESS+0x1C)))
#define GPIOH_AFRL             (*((volatile uint32_t *)(GPIOH_BASE_ADDRESS+0x20)))
#define GPIOH_AFRH             (*((volatile uint32_t *)(GPIOH_BASE_ADDRESS+0x24)))

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
/*                                    Private Macros                                                   */
/*******************************************************************************************************/

#define LOCK_PIN_BIT    16U

#endif /* GPIO_PRIVATE_H_ */
