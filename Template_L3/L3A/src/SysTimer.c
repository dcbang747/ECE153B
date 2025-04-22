/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 3A
 */

 #include "SysTimer.h"
 #include "stm32l476xx.h"
 
 uint32_t volatile msTicks;
 
 void SysTick_Init(void) {
	 /* Disable SysTick during configuration */
	 SysTick->CTRL = 0;
 
	 /* ------------------------------------------------------------
	  * System clock (SYSCLK)   = 8 MHz (MSI range 7)
	  * CLKSOURCE bit     = 0   -> SysTick clock = HCLK/8 = 1 MHz
	  * Desired period          = 1ms -> 1,000,000,Hz / 1000 = 1000 counts
	  * RELOAD value            = counts - 1 = 999
	  * ---------------------------------------------------------- */
	 SysTick->LOAD = 999;                                /* 1ms period */
 
	 SysTick->VAL  = 0;                                  /* clear current count */
 
	 NVIC_EnableIRQ   (SysTick_IRQn);                    /* enable interrupt  */
	 NVIC_SetPriority (SysTick_IRQn, 1);
 
	 SysTick->CTRL |=  (1UL << 1);          /* enable IRQ       */
	 SysTick->CTRL &= ~(1UL << 2);      /* use HCLK/8 clock */
	 SysTick->CTRL |=  (1UL << 0);         /* start SysTick    */
 }
 
 void SysTick_Handler(void) {
	 ++msTicks;                                          /* 1ms tick */
 }
 
 void delay(uint32_t T) {
	 /* Busy-wait until T milliseconds have elapsed */
	 uint32_t start = msTicks;
	 while ((msTicks - start) < T) {
		 /* just wait */
	 }
 }
 