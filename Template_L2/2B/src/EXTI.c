/*
 * ECE 153B ‑‑ Lab 2B  (Part B – Interrupts)
 *
 * Name(s):
 * Section:
 */

 #include "stm32l476xx.h"
 #include "EXTI.h"
 #include "LED.h"
 
 /* ------------- Private helpers ------------- */
 #define USER_BTN_PORT       GPIOC      // PC13 on Nucleo‑L476
 #define USER_BTN_PIN        13u
 #define USER_BTN_MASK       (1u << USER_BTN_PIN)
 
 void EXTI_Init(void)
 {
	 /* 1. Enable GPIOC + SYSCFG clocks */
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;      // GPIOC clock
	 RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;     // SYSCFG clock
 
	 /* 2. Configure PC13 as input with pull‑up (button is active‑low) */
	 USER_BTN_PORT->MODER  &= ~(GPIO_MODER_MODE13_Msk);          // 00 = input
	 USER_BTN_PORT->PUPDR  &= ~(GPIO_PUPDR_PUPD13_Msk);
	 USER_BTN_PORT->PUPDR  |=  (1u << (USER_BTN_PIN * 2u));      // 01 = pull‑up
 
	 /* 3. Map PC13 → EXTI13 in SYSCFG_EXTICR4 */
	 SYSCFG->EXTICR[3] &= ~(SYSCFG_EXTICR4_EXTI13_Msk);          // clear
	 SYSCFG->EXTICR[3] |=  (2u << SYSCFG_EXTICR4_EXTI13_Pos);    // 0b0010 = Port C
 
	 /* 4. Configure EXTI trigger: falling edge only (button press) */
	 EXTI->RTSR1 &= ~USER_BTN_MASK;          // disable rising
	 EXTI->FTSR1 |=  USER_BTN_MASK;          // enable falling
 
	 /* 5. Un‑mask EXTI13 */
	 EXTI->IMR1  |=  USER_BTN_MASK;
 
	 /* 6. Clear any pending flag */
	 EXTI->PR1   =   USER_BTN_MASK;
 
	 /* 7. Enable NVIC line and set top priority (0 is highest) */
	 NVIC_SetPriority(EXTI15_10_IRQn, 0);
	 NVIC_EnableIRQ  (EXTI15_10_IRQn);
 }
 
 /* --------------------------------------------------------- */
 /*                    Interrupt handler                      */
 /* --------------------------------------------------------- */
 void EXTI15_10_IRQHandler(void)
 {
	 if (EXTI->PR1 & USER_BTN_MASK)          // was it EXTI13?
	 {
		 EXTI->PR1 = USER_BTN_MASK;          // 1 → clear pending
 
		 /* Every press toggles;  odd press  → LED ON
								   even press → LED OFF    */
		 Green_LED_Toggle();
	 }
 }
 