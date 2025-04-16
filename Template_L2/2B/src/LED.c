/*
 * ECE 153B ‑‑ Lab 2B  (Part B – Interrupts)
 *
 * Name(s):
 * Section:
 */

 #include "stm32l476xx.h"
 #include "LED.h"
 
 /* ------------- Private helpers ------------- */
 #define GREEN_LED_PORT      GPIOA          // PA5 on Nucleo‑L476
 #define GREEN_LED_PIN       5u
 #define GREEN_LED_MASK      (1u << GREEN_LED_PIN)
 
 void LED_Init(void)
 {
	 /* 1. Enable GPIOA clock (AHB2 domain) */
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
 
	 /* 2. Configure PA5 as general‑purpose output, push‑pull, low speed, no pull‑up/down */
	 GREEN_LED_PORT->MODER   &= ~(GPIO_MODER_MODE5);          // clear bits
	 GREEN_LED_PORT->MODER   |=  (1u << (GREEN_LED_PIN * 2u));    // 01 = output
	 GREEN_LED_PORT->OTYPER  &= ~GREEN_LED_MASK;                  // 0 = push‑pull
	 GREEN_LED_PORT->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5);      // 00 = low speed
	 GREEN_LED_PORT->PUPDR   &= ~(GPIO_PUPDR_PUPD5);          // 00 = no pull
 
	 Green_LED_Off();                                             // start with LED OFF
 }
 
 void Green_LED_On(void)
 {
	 /* Use BSRR for atomic set */
	 GREEN_LED_PORT->BSRR =  GREEN_LED_MASK;
 }
 
 void Green_LED_Off(void)
 {
	 /* Upper 16 bits of BSRR reset the pin */
	 GREEN_LED_PORT->BSRR = (GREEN_LED_MASK << 16);
 }
 
 void Green_LED_Toggle(void)
 {
	 GREEN_LED_PORT->ODR ^= GREEN_LED_MASK;
 }
 