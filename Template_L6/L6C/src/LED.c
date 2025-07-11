/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6B
 */

#include "LED.h"


#define LED_PIN 5

void LED_Init(void){
	//TODO
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

void LED_On(void){
	//TODO
	GPIOA->ODR |= GPIO_ODR_OD5;
}

void LED_Off(void){
	//TODO
	GPIOA->ODR &= ~GPIO_ODR_OD5;
}

void LED_Toggle(void){
	//TODO
	GPIOA->ODR ^= GPIO_ODR_OD5;
}
