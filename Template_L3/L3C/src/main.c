/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 3C
 */
 
#include <stdio.h> 
 
#include "stm32l476xx.h"

uint32_t volatile currentValue = 0;
uint32_t volatile lastValue = 0;
uint32_t volatile overflowCount = 0;
uint32_t volatile timeInterval = 0;

void Input_Capture_Setup() {
	// [TODO]
	RCC->AHB2ENR |=RCC_AHB2ENR_GPIOBEN;
	
	GPIOB->MODER &= ~GPIO_MODER_MODE6; 	//PB6
	GPIOB->MODER |= GPIO_MODER_MODE6_1; //AF Mode
	GPIOB->AFR[0] &=	~GPIO_AFRL_AFSEL6; 	// TIM4_CH1, AF2
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1;
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;		//Enable timer
	
	TIM4->PSC =15;
	
	TIM4->CR1 |= TIM_CR1_ARPE;
	TIM4->ARR = 0xFFFF;  // Max value
	
	// Set polarity to both edges
	TIM4->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);  // Clear both
	TIM4->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1NP;     // Both edges

	// Enable capture
	TIM4->CCER |= TIM_CCER_CC1E;

	TIM4->DIER |= TIM_DIER_CC1IE | TIM_DIER_CC1DE | TIM_DIER_UIE;

	TIM4->EGR |= TIM_EGR_UG;
	
	TIM4->CR1 &= ~TIM_CR1_DIR;  // Upcounting
	TIM4->CR1 |= TIM_CR1_CEN;   // Start the counter

	NVIC_SetPriority(TIM4_IRQn, 2);
	NVIC_EnableIRQ(TIM4_IRQn);
	
	currentValue = CR1


}

void TIM4_IRQHandler(void) {
	// [TODO]
	EXTI->RTSR1 |= EXTI_RTSR1_RT9;
	EXTI->FTSR1 |= EXTI_FTSR1_FT6;
	
	
	
}

void Trigger_Setup() {
	// [TODO]
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;  //Enable gpio-a clock
	
	GPIOA->MODER &= ~GPIO_MODER_MODE9;		//Clear bits, PA9
	GPIOA->MODER |= GPIO_MODER_MODER9_1;	//AF Mode
  
	GPIOA->AFR[1]	&= ~GPIO_AFRH_AFSEL9;		//Set AF as TIM1_CH2
	GPIOA->AFR[1] |= GPIO_AFRH_AFSEL9_0;
	
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD9;			//No pull up/no pull down
	
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT9; 			// Push-pull mode
	
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED9;   //Very high output
	
	//Enable TIM1 clock
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	TIM1->PSC = 15;
	TIM1->ARR = 0xFFFF;
	
	TIM1->CCR2 = 10; 
	
	TIM1->CCMR1 |= (6 << 12);        // PWM Mode 1 on CH2
	TIM1->CCMR1 |= TIM_CCMR1_OC2PE; // Preload enable
	
	TIM1->CCER |= TIM_CCER_CC2E;    	 // Enable output on CH2
	
	TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI; // Setup break and dead time reg
	
	TIM1->EGR |= TIM_EGR_UG;        	 // Enable update generation
	
	TIM1->DIER |= TIM_DIER_UIE;    // Enable update interrupt
	
	TIM1->SR &= ~TIM_SR_UIF;       // Clear update interupt flag
	
	TIM1->CR1 |= TIM_CR1_CEN;     // Start counter
	
}

int main(void) {	
	// Enable High Speed Internal Clock (HSI = 16 MHz)
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0); // Wait until HSI is ready
	
	// Select HSI as system clock source 
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) == 0); // Wait until HSI is system clock source
  
	// Input Capture Setup
	Input_Capture_Setup();
	
	// Trigger Setup
	Trigger_Setup();

	
	while(1) {
		// [TODO] Store your measurements on Stack
		
	}
}


