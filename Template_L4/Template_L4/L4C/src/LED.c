/*
 * ECE 153B - Winter 2021
 *
 * Name(s):
 * Section:
 * Lab: 4C
 */

 #include "LED.h"
 #include "stm32l476xx.h"
 
 void LED_Init(void)
 {
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	 GPIOA->MODER &= ~GPIO_MODER_MODE5;
	 GPIOA->MODER |=  GPIO_MODER_MODE5_0;  /* output */
 }
