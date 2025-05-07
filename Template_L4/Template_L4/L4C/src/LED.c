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
	 RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOAEN;
	 GPIOA->MODER  &= ~(3U << (5 * 2));
	 GPIOA->MODER  |=  (1U << (5 * 2));     /* general‑purpose output */
	 GPIOA->OTYPER &= ~(1U << 5);           /* push‑pull             */
	 GPIOA->OSPEEDR|=  (3U << (5 * 2));     /* very‑high speed       */
	 GPIOA->PUPDR  &= ~(3U << (5 * 2));     /* no pull‑up/down       */
 }