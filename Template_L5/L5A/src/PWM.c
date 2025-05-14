#include "PWM.h"

// [TODO]
void LED_Pin_Init(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;			//enable gpio a clk
	
	GPIOA->MODER &= ~GPIO_MODER_MODE5;				//clear bits
	GPIOA->MODER |= GPIO_MODER_MODE5_1;				//set to AF mode
	
	 GPIOA->OSPEEDR |= (0x3UL << (5 * 2));
	
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;				//no pull up, no pull down
	
	GPIOA->AFR[0] &= ~(0xFUL << (5 * 4));
	 GPIOA->AFR[0] |=  (0x1UL << (5 * 4));       /* AF1 */
	
}

void TIM2_CH1_Init(void){
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;			//enable tim2 clk
	
	TIM2->PSC=7;															
	TIM2->ARR=999;
	
	TIM2->CCMR1 &= ~(7U << 4);								//pwm mode 1, output compare, preload
	TIM2->CCMR1 |=  (6U << 4);
	TIM2->CCMR1 |=  TIM_CCMR1_OC1PE;
	
	TIM2->CCER  &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP); //output active high & enable
  TIM2->CCER  |=  TIM_CCER_CC1E;
	
	TIM2->CR1  |= TIM_CR1_ARPE;								//enable arr buffer and force update
	TIM2->EGR  |= TIM_EGR_UG;
	
	TIM2->CR1  |= TIM_CR1_CEN;								//start timer
}

