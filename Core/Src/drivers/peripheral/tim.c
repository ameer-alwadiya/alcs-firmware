#include "tim.h"


/**
* @brief Initialize timer with prescaler and period
* @param TIMx: Timer instance
* @param prescaler: Prescaler value (0-65535)
* @param period: Auto-reload value
*/
void TIM_Init(TIM_TypeDef *TIMx, uint16_t prescaler, uint32_t period) {

   /* Set prescaler */
   TIMx->PSC = prescaler - 1;

   /* Set auto-reload value */
   TIMx->ARR = period - 1;

   /* Generate update event to load the prescaler value */
   TIMx->EGR |= TIM_EGR_UG;

   /* Clear update flag */
   TIMx->SR &= ~TIM_SR_UIF;
}




/**
* @brief Start the timer
* @param TIMx: Timer instance
*/
void TIM_Start(TIM_TypeDef *TIMx) {
   TIMx->CR1 |= TIM_CR1_CEN;
}




/**
* @brief Stop the timer
* @param TIMx: Timer instance
*/
void TIM_Stop(TIM_TypeDef *TIMx) {
   TIMx->CR1 &= ~TIM_CR1_CEN;
}




/**
* @brief Enable update interrupt
* @param TIMx: Timer instance
*/
void TIM_EnableInterrupt(TIM_TypeDef *TIMx) {
   TIMx->DIER |= TIM_DIER_UIE;
}




/**
* @brief Disable update interrupt
* @param TIMx: Timer instance
*/
void TIM_DisableInterrupt(TIM_TypeDef *TIMx) {
   TIMx->DIER &= ~TIM_DIER_UIE;
}




/**
* @brief Clear update interrupt flag
* @param TIMx: Timer instance
*/
void TIM_ClearInterruptFlag(TIM_TypeDef *TIMx) {
   TIMx->SR &= ~TIM_SR_UIF;
}




/**
* @brief Get current counter value
* @param TIMx: Timer instance
* @return Counter value
*/
uint32_t TIM_GetCounter(TIM_TypeDef *TIMx) {
   return TIMx->CNT;
}




/**
* @brief Set counter value
* @param TIMx: Timer instance
* @param value: Counter value to set
*/
void TIM_SetCounter(TIM_TypeDef *TIMx, uint32_t value) {
   TIMx->CNT = value;
}




/**
* @brief Initialize PWM mode for a specific channel
* @param TIMx: Timer instance
* @param channel: Timer channel
* @param prescaler: Prescaler value (0-65535)
* @param period: Auto-reload value (PWM period)
*/
void TIM_PWM_Init(TIM_TypeDef *TIMx, TIM_Channel_t channel, uint16_t prescaler, uint32_t period) {

   /* Set prescaler */
   TIMx->PSC = prescaler - 1;

   /* Set auto-reload value (PWM period) */
   TIMx->ARR = period - 1;

   /* Configure PWM mode based on channel */
   switch (channel) {
       case TIM_CHANNEL_1:
           TIMx->CCMR1 |= TIM_CCMR_OC1M_PWM1;  /* PWM mode 1 */
           TIMx->CCMR1 |= TIM_CCMR_OC1PE;      /* Preload enable */
           TIMx->CCER |= TIM_CCER_CC1E;        /* Enable output */
           TIMx->CCR1 = 0;                     /* Initial duty cycle = 0 */
           break;

       case TIM_CHANNEL_2:
           TIMx->CCMR1 |= TIM_CCMR_OC2M_PWM1;
           TIMx->CCMR1 |= TIM_CCMR_OC2PE;
           TIMx->CCER |= TIM_CCER_CC2E;
           TIMx->CCR2 = 0;
           break;

       case TIM_CHANNEL_3:
           TIMx->CCMR2 |= (0x6 << 4);          /* PWM mode 1 for channel 3 */
           TIMx->CCMR2 |= (1 << 3);            /* Preload enable */
           TIMx->CCER |= TIM_CCER_CC3E;
           TIMx->CCR3 = 0;
           break;

       case TIM_CHANNEL_4:
           TIMx->CCMR2 |= (0x6 << 12);         /* PWM mode 1 for channel 4 */
           TIMx->CCMR2 |= (1 << 11);           /* Preload enable */
           TIMx->CCER |= TIM_CCER_CC4E;
           TIMx->CCR4 = 0;
           break;
   }

   /* Enable auto-reload preload */
   TIMx->CR1 |= TIM_CR1_ARPE;

   /* Generate update event */
   TIMx->EGR |= TIM_EGR_UG;

   /* Clear update flag */
   TIMx->SR &= ~TIM_SR_UIF;

   /* Start timer */
   TIMx->CR1 |= TIM_CR1_CEN;
}




/**
* @brief Set PWM duty cycle
* @param TIMx: Timer instance
* @param channel: Timer channel
* @param duty: Duty cycle value (0 to ARR)
*/
void TIM_PWM_SetDutyCycle(TIM_TypeDef *TIMx, TIM_Channel_t channel, uint32_t duty) {
   switch (channel) {
       case TIM_CHANNEL_1:
           TIMx->CCR1 = duty;
           break;
       case TIM_CHANNEL_2:
           TIMx->CCR2 = duty;
           break;
       case TIM_CHANNEL_3:
           TIMx->CCR3 = duty;
           break;
       case TIM_CHANNEL_4:
           TIMx->CCR4 = duty;
           break;
   }
}




/**
* @brief Blocking delay in microseconds using timer
* @param TIMx: Timer instance (preferably TIM2 or TIM5 for 32-bit)
* @param microseconds: Delay duration in microseconds
* @note Assumes APB1 timer clock is 84 MHz for STM32F401
*/
void TIM_Delay_us(TIM_TypeDef *TIMx, uint32_t microseconds) {
   /* Configure timer for 1 MHz (1 us resolution) */
   /* Assuming 84 MHz timer clock: PSC = 84-1 = 83 */
   TIM_Init(TIMx, 84, microseconds);

   /* Enable one-pulse mode */
   TIMx->CR1 |= TIM_CR1_OPM;

   /* Start timer */
   TIM_Start(TIMx);

   /* Wait until update flag is set */
   while (!(TIMx->SR & TIM_SR_UIF));

   /* Clear update flag */
   TIMx->SR &= ~TIM_SR_UIF;
}




/**
* @brief Blocking delay in milliseconds using timer
* @param TIMx: Timer instance (preferably TIM2 or TIM5 for 32-bit)
* @param milliseconds: Delay duration in milliseconds
* @note Assumes APB1 timer clock is 84 MHz for STM32F401
*/
void TIM_Delay_ms(TIM_TypeDef *TIMx, uint32_t milliseconds) {
   for (uint32_t i = 0; i < milliseconds; i++) {
       TIM_Delay_us(TIMx, 1000);
   }
}
