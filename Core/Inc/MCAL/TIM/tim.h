#ifndef TIM_H
#define TIM_H


#include <stdint.h>


/* Timer Base Addresses */
#define TIM1_BASE   0x40010000
#define TIM2_BASE   0x40000000
#define TIM3_BASE   0x40000400
#define TIM4_BASE   0x40000800
#define TIM5_BASE   0x40000C00
#define TIM9_BASE   0x40014000
#define TIM10_BASE  0x40014400
#define TIM11_BASE  0x40014800


/* Timer Register Structure */
typedef struct {
   volatile uint32_t CR1;      /* Control register 1 */
   volatile uint32_t CR2;      /* Control register 2 */
   volatile uint32_t SMCR;     /* Slave mode control register */
   volatile uint32_t DIER;     /* DMA/Interrupt enable register */
   volatile uint32_t SR;       /* Status register */
   volatile uint32_t EGR;      /* Event generation register */
   volatile uint32_t CCMR1;    /* Capture/compare mode register 1 */
   volatile uint32_t CCMR2;    /* Capture/compare mode register 2 */
   volatile uint32_t CCER;     /* Capture/compare enable register */
   volatile uint32_t CNT;      /* Counter */
   volatile uint32_t PSC;      /* Prescaler */
   volatile uint32_t ARR;      /* Auto-reload register */
   volatile uint32_t RCR;      /* Repetition counter register */
   volatile uint32_t CCR1;     /* Capture/compare register 1 */
   volatile uint32_t CCR2;     /* Capture/compare register 2 */
   volatile uint32_t CCR3;     /* Capture/compare register 3 */
   volatile uint32_t CCR4;     /* Capture/compare register 4 */
   volatile uint32_t BDTR;     /* Break and dead-time register */
   volatile uint32_t DCR;      /* DMA control register */
   volatile uint32_t DMAR;     /* DMA address for full transfer */
   volatile uint32_t OR;       /* Option register */
} TIM_TypeDef;


/* Timer Instances */
#define TIM1    ((TIM_TypeDef *) TIM1_BASE)
#define TIM2    ((TIM_TypeDef *) TIM2_BASE)
#define TIM3    ((TIM_TypeDef *) TIM3_BASE)
#define TIM4    ((TIM_TypeDef *) TIM4_BASE)
#define TIM5    ((TIM_TypeDef *) TIM5_BASE)
#define TIM9    ((TIM_TypeDef *) TIM9_BASE)
#define TIM10   ((TIM_TypeDef *) TIM10_BASE)
#define TIM11   ((TIM_TypeDef *) TIM11_BASE)


/* CR1 Register Bits */
#define TIM_CR1_CEN     (1 << 0)    /* Counter enable */
#define TIM_CR1_UDIS    (1 << 1)    /* Update disable */
#define TIM_CR1_URS     (1 << 2)    /* Update request source */
#define TIM_CR1_OPM     (1 << 3)    /* One pulse mode */
#define TIM_CR1_DIR     (1 << 4)    /* Direction */
#define TIM_CR1_CMS_0   (1 << 5)    /* Center-aligned mode selection */
#define TIM_CR1_CMS_1   (1 << 6)
#define TIM_CR1_ARPE    (1 << 7)    /* Auto-reload preload enable */


/* DIER Register Bits */
#define TIM_DIER_UIE    (1 << 0)    /* Update interrupt enable */
#define TIM_DIER_CC1IE  (1 << 1)    /* Capture/Compare 1 interrupt enable */
#define TIM_DIER_CC2IE  (1 << 2)    /* Capture/Compare 2 interrupt enable */
#define TIM_DIER_CC3IE  (1 << 3)    /* Capture/Compare 3 interrupt enable */
#define TIM_DIER_CC4IE  (1 << 4)    /* Capture/Compare 4 interrupt enable */


/* SR Register Bits */
#define TIM_SR_UIF      (1 << 0)    /* Update interrupt flag */
#define TIM_SR_CC1IF    (1 << 1)    /* Capture/Compare 1 interrupt flag */
#define TIM_SR_CC2IF    (1 << 2)    /* Capture/Compare 2 interrupt flag */
#define TIM_SR_CC3IF    (1 << 3)    /* Capture/Compare 3 interrupt flag */
#define TIM_SR_CC4IF    (1 << 4)    /* Capture/Compare 4 interrupt flag */
#define TIM_SR_CC1OF    (1 << 9)    /* Capture/Compare 1 overcapture flag */
#define TIM_SR_CC2OF    (1 << 10)   /* Capture/Compare 2 overcapture flag */
#define TIM_SR_CC3OF    (1 << 11)   /* Capture/Compare 3 overcapture flag */
#define TIM_SR_CC4OF    (1 << 12)   /* Capture/Compare 4 overcapture flag */


/* EGR Register Bits */
#define TIM_EGR_UG      (1 << 0)    /* Update generation */


/* CCMR Register Bits (Output Compare Mode) */
#define TIM_CCMR_OC1M_PWM1  (0x6 << 4)  /* PWM mode 1 */
#define TIM_CCMR_OC1M_PWM2  (0x7 << 4)  /* PWM mode 2 */
#define TIM_CCMR_OC1PE      (1 << 3)    /* Output compare 1 preload enable */
#define TIM_CCMR_OC2M_PWM1  (0x6 << 12) /* PWM mode 1 */
#define TIM_CCMR_OC2M_PWM2  (0x7 << 12) /* PWM mode 2 */
#define TIM_CCMR_OC2PE      (1 << 11)   /* Output compare 2 preload enable */


/* CCMR Register Bits (Input Capture Mode) */
#define TIM_CCMR_CC1S_INPUT_TI1  (0x1 << 0)  /* CC1 channel is input, IC1 mapped on TI1 */
#define TIM_CCMR_CC1S_INPUT_TI2  (0x2 << 0)  /* CC1 channel is input, IC1 mapped on TI2 */
#define TIM_CCMR_CC2S_INPUT_TI2  (0x1 << 8)  /* CC2 channel is input, IC2 mapped on TI2 */
#define TIM_CCMR_CC2S_INPUT_TI1  (0x2 << 8)  /* CC2 channel is input, IC2 mapped on TI1 */
#define TIM_CCMR_IC1PSC_OFF      (0x0 << 2)  /* No prescaler */
#define TIM_CCMR_IC2PSC_OFF      (0x0 << 10) /* No prescaler */
#define TIM_CCMR_IC1F_MASK       (0xF << 4)  /* Input capture 1 filter */
#define TIM_CCMR_IC2F_MASK       (0xF << 12) /* Input capture 2 filter */


/* CCER Register Bits */
#define TIM_CCER_CC1E   (1 << 0)    /* Capture/Compare 1 output enable */
#define TIM_CCER_CC1P   (1 << 1)    /* Capture/Compare 1 output polarity */
#define TIM_CCER_CC2E   (1 << 4)    /* Capture/Compare 2 output enable */
#define TIM_CCER_CC2P   (1 << 5)    /* Capture/Compare 2 output polarity */
#define TIM_CCER_CC3E   (1 << 8)    /* Capture/Compare 3 output enable */
#define TIM_CCER_CC3P   (1 << 9)    /* Capture/Compare 3 output polarity */
#define TIM_CCER_CC4E   (1 << 12)   /* Capture/Compare 4 output enable */
#define TIM_CCER_CC4P   (1 << 13)   /* Capture/Compare 4 output polarity */


/* Timer Channel Enumeration */
typedef enum {
   TIM_CHANNEL_1 = 0,
   TIM_CHANNEL_2,
   TIM_CHANNEL_3,
   TIM_CHANNEL_4
} TIM_Channel_t;


/* Input Capture Polarity */
typedef enum {
   TIM_IC_POLARITY_RISING = 0,
   TIM_IC_POLARITY_FALLING
} TIM_IC_Polarity_t;


/* Input Capture Prescaler */
typedef enum {
   TIM_IC_PSC_DIV1 = 0,
   TIM_IC_PSC_DIV2 = 1,
   TIM_IC_PSC_DIV4 = 2,
   TIM_IC_PSC_DIV8 = 3
} TIM_IC_Prescaler_t;


/* Output Compare Mode */
typedef enum {
   TIM_OC_MODE_TIMING = 0,
   TIM_OC_MODE_ACTIVE,
   TIM_OC_MODE_INACTIVE,
   TIM_OC_MODE_TOGGLE,
   TIM_OC_MODE_FORCE_INACTIVE,
   TIM_OC_MODE_FORCE_ACTIVE,
   TIM_OC_MODE_PWM1,
   TIM_OC_MODE_PWM2
} TIM_OC_Mode_t;


/* Callback function pointer types */
typedef void (*TIM_Callback_t)(void);


/* Function Prototypes - Basic Timer Functions */
void TIM_Init(TIM_TypeDef *TIMx, uint16_t prescaler, uint32_t period);
void TIM_Start(TIM_TypeDef *TIMx);
void TIM_Stop(TIM_TypeDef *TIMx);
void TIM_EnableInterrupt(TIM_TypeDef *TIMx);
void TIM_DisableInterrupt(TIM_TypeDef *TIMx);
void TIM_ClearInterruptFlag(TIM_TypeDef *TIMx);
uint32_t TIM_GetCounter(TIM_TypeDef *TIMx);
void TIM_SetCounter(TIM_TypeDef *TIMx, uint32_t value);


/* PWM Functions */
void TIM_PWM_Init(TIM_TypeDef *TIMx, TIM_Channel_t channel, uint16_t prescaler, uint32_t period);
void TIM_PWM_SetDutyCycle(TIM_TypeDef *TIMx, TIM_Channel_t channel, uint32_t duty);


/* Delay Functions */
void TIM_Delay_us(TIM_TypeDef *TIMx, uint32_t microseconds);
void TIM_Delay_ms(TIM_TypeDef *TIMx, uint32_t milliseconds);


/* Input Capture Functions */
void TIM_IC_Init(TIM_TypeDef *TIMx, TIM_Channel_t channel, TIM_IC_Polarity_t polarity,
                 TIM_IC_Prescaler_t prescaler, uint8_t filter);
uint32_t TIM_IC_GetCapture(TIM_TypeDef *TIMx, TIM_Channel_t channel);
void TIM_IC_EnableInterrupt(TIM_TypeDef *TIMx, TIM_Channel_t channel);
void TIM_IC_DisableInterrupt(TIM_TypeDef *TIMx, TIM_Channel_t channel);
void TIM_IC_ClearFlag(TIM_TypeDef *TIMx, TIM_Channel_t channel);
uint8_t TIM_IC_GetFlag(TIM_TypeDef *TIMx, TIM_Channel_t channel);


/* Output Compare Functions */
void TIM_OC_Init(TIM_TypeDef *TIMx, TIM_Channel_t channel, TIM_OC_Mode_t mode,
                 uint32_t pulse, uint8_t preload);
void TIM_OC_SetCompare(TIM_TypeDef *TIMx, TIM_Channel_t channel, uint32_t compare);
uint32_t TIM_OC_GetCompare(TIM_TypeDef *TIMx, TIM_Channel_t channel);
void TIM_OC_EnableInterrupt(TIM_TypeDef *TIMx, TIM_Channel_t channel);
void TIM_OC_DisableInterrupt(TIM_TypeDef *TIMx, TIM_Channel_t channel);
void TIM_OC_ClearFlag(TIM_TypeDef *TIMx, TIM_Channel_t channel);
uint8_t TIM_OC_GetFlag(TIM_TypeDef *TIMx, TIM_Channel_t channel);


/* Callback Registration Functions */
void TIM_RegisterCallback_Update(TIM_TypeDef *TIMx, TIM_Callback_t callback);
void TIM_RegisterCallback_CC1(TIM_TypeDef *TIMx, TIM_Callback_t callback);
void TIM_RegisterCallback_CC2(TIM_TypeDef *TIMx, TIM_Callback_t callback);
void TIM_RegisterCallback_CC3(TIM_TypeDef *TIMx, TIM_Callback_t callback);
void TIM_RegisterCallback_CC4(TIM_TypeDef *TIMx, TIM_Callback_t callback);


/* IRQ Handler Functions (to be called from ISR) */
void TIM_IRQHandler(TIM_TypeDef *TIMx);


#endif /* TIM_H */
