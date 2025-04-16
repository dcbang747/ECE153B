/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 2B
 */

#include "LED.h"

void LED_Init(void) {
	// Enable GPIO Clocks
	// [TODO]
	//Enables Clock for PORT A and C
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; 
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	
	// Initialize Green LED
	// [TODO]
	GPIOA->MODER &= ~GPIO_MODER_MODE5; //Clear LED GPIO pin
	GPIOA->MODER |= GPIO_MODER_MODE5_0; // Set LED to output 3.3v
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT5; // Push-pull mode
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5; // No Pull up/pull down
}

void Green_LED_Off(void) {
	// [TODO]
	GPIOA->ODR |= ~GPIO_ODR_OD5;
	
}

void Green_LED_On(void) {
	// [TODO]
	GPIOA->ODR |= GPIO_ODR_OD5;
}

void Green_LED_Toggle(void) {
	// [TODO]
	 if (GPIOC->IDR & GPIO_IDR_ID13) {
            // Toggle PA5 by XOR'ing its bit in ODR
            GPIOA->ODR ^= GPIO_ODR_OD5;  
            
            // Wait until the button is released, so we only toggle once
            while (GPIOC->IDR & GPIO_IDR_ID13) {
                // do nothing
            }
        }
}
