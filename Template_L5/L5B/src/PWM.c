#include "PWM.h"
#include "stm32l476xx.h"

/* LD2 on PA5 → TIM2_CH1 (AF1) */
void LED_Pin_Init(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->MODER   &= ~GPIO_MODER_MODE5;
    GPIOA->MODER   |=  GPIO_MODER_MODE5_1;             /* AF mode */

    GPIOA->OSPEEDR |=  GPIO_OSPEEDR_OSPEED5;           /* very high */
    GPIOA->PUPDR   &= ~GPIO_PUPDR_PUPD5;

    GPIOA->AFR[0]  &= ~GPIO_AFRL_AFSEL5;
    GPIOA->AFR[0]  |=  (0x1U << (5 * 4));              /* AF1 */
}

void TIM2_CH1_Init(void) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    TIM2->PSC  = 15U;       /* 16 MHz / (15+1) = 1 MHz */
    TIM2->ARR  = 999U;      /* → 1 kHz PWM  */
    TIM2->CCR1 = 0U;

    /* PWM mode 1, preload enable */
    TIM2->CCMR1 &= ~(7U << 4);
    TIM2->CCMR1 |=  (6U << 4) | TIM_CCMR1_OC1PE;

    TIM2->CCER  &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
    TIM2->CCER  |=  TIM_CCER_CC1E;

    TIM2->CR1   |=  TIM_CR1_ARPE;
    TIM2->EGR   |=  TIM_EGR_UG;
    TIM2->CR1   |=  TIM_CR1_CEN;
}