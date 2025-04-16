/*
 * ECE 153B ‑‑ Lab 2B  (Part B – Interrupts)
 *
 * Name(s):
 * Section:
 */

 #include "stm32l476xx.h"
 #include "EXTI.h"
 #include "LED.h"
 #include "SysClock.h"
 
 int main(void)
 {
	 /* Initialise system clock, LED, and EXTI (user button) */
	 System_Clock_Init();
	 LED_Init();
	 EXTI_Init();
 
	 /* main loop does nothing – everything happens in the ISR */
	 while (1)
	 {
		 __WFI();        // sleep until next interrupt (optional, saves power)
	 }
 }
 