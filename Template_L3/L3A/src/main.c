/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 3A
 */

 #include "stm32l476xx.h"
 #include "LED.h"
 #include "SysTimer.h"
 
 void System_Clock_Init(void) {
	 /* 1. Make sure we switch back to MSI (SW = 00) */
	 RCC->CFGR &= ~RCC_CFGR_SW;                 /* MSI selected as SYSCLK */
 
	 /* 2. Use MSIRANGE bits in RCC_CR (set MSIRGSEL) */
	 RCC->CR |= RCC_CR_MSIRGSEL;
 
	 /* 3. Select 8MHz range (MSIRANGE = 0b0111) */
	 RCC->CR &= ~RCC_CR_MSIRANGE;               /* clear range */
	 RCC->CR |=  (7U << RCC_CR_MSIRANGE_Pos);   /* range 7 -> 8MHz */
 
	 /* 4. Enable MSI oscillator */
	 RCC->CR |= RCC_CR_MSION;
 
	 /* 5. Wait until MSI is ready */
	 while ((RCC->CR & RCC_CR_MSIRDY) == 0);
 }
 
 int main(void) {
	 /*------------------------------------------------------------------*/
	 /*  Initialise periphery                                            */
	 /*------------------------------------------------------------------*/
	 System_Clock_Init();   /* 8MHz MSI */
	 SysTick_Init();        /* 1ms ticks */
	 LED_Init();            /* PA5 green LED */
 
	 /*------------------------------------------------------------------*/
	 /*  Main loop : toggle LED every second                             */
	 /*------------------------------------------------------------------*/
	 while (1) {
		 Green_LED_Toggle();
		 delay(1000);       /* wait 1000ms = 1s */
	 }
 }
 