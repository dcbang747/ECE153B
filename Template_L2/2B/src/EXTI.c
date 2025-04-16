/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 2B
 */

#include "EXTI.h"
#include "LED.h"

void EXTI_Init(void) {
	// Initialize User Button
	// [TODO]
		GPIOC -> MODER &= ~GPIO_MODER_MODE13;
		GPIOC -> PUPDR &= ~GPIO_PUPDR_PUPD13;
		GPIOA -> ODR |= GPIO_ODR_OD5;
	
	// Configure SYSCFG EXTI
	// [TODO]
		SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
		SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	
	// Configure EXTI Trigger
	// [TODO]
		EXTI->RTSR1 |= EXTI_RTSR1_RT0;
		EXTI->FTSR1 |= EXTI_FTSR1_FT1;
	
	// Enable EXTI
	// [TODO]
		EXTI->IMR1 |= EXTI_IMR1_IM1;
	
	// Configure and Enable in NVIC
	// [TODO]
		NVIC_EnableIRQ(EXTI4_IRQn);
		NVIC_SetPriority(EXTI4_IRQn, 0);
}

// [TODO] Write Interrupt Handlers (look in startup_stm32l476xx.s to find the 
// interrupt handler names that you should use)
void EXTI4_IRQHandler(void){
	// Clear interrupt pending bit
	EXTI->PR1 |= EXTI_PR1_PIF13;
	
	// Define behavior that occurs when interrupt occurs
	if((EXTI->PR1 & EXTI_PR1_PIF3) != 0){
		Green_LED_Toggle();
	}
}

