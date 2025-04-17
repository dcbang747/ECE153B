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
		RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOCEN;     // PC13 clock
		RCC->APB2ENR  |= RCC_APB2ENR_SYSCFGEN;    // SYSCFG clock
		SYSCFG->EXTICR[3] &= ~(0xF << 4);         // clear EXTI13[7:4]
		SYSCFG->EXTICR[3] |=  (2   << 4);         // 0b0010 = port C

	
	// Configure EXTI Trigger
	// [TODO]
	    EXTI->RTSR1 &= ~EXTI_RTSR1_RT13;          // disable rising edge
    	EXTI->FTSR1 |=  EXTI_FTSR1_FT13;          // enable falling edge
	
	// Enable EXTI
	// [TODO]
    	EXTI->IMR1  |=  EXTI_IMR1_IM13;           // un‑mask EXTI13
	
	// Configure and Enable in NVIC
	// [TODO]
		NVIC_SetPriority(EXTI15_10_IRQn, 0);
		NVIC_EnableIRQ  (EXTI15_10_IRQn);
}

// [TODO] Write Interrupt Handlers (look in startup_stm32l476xx.s to find the 
// interrupt handler names that you should use)
void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR1 & EXTI_PR1_PIF13)           
    {
        EXTI->PR1 = EXTI_PR1_PIF13;           
        Green_LED_Toggle();                   
    }
}

