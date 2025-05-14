#include "SysClock.h"

 void System_Clock_Init(void) {
	 /* 1. Make sure we switch back to MSI (SW = 00) */
	 RCC->CFGR &= ~RCC_CFGR_SW;                 /* MSI selected as SYSCLK */
 
	 /* 2. Use MSIRANGE bits in RCC_CR (set MSIRGSEL) */
	 RCC->CR |= RCC_CR_MSIRGSEL;
 
	 /* 3. Select 8MHz range (MSIRANGE = 0b0111) */ 
	 RCC->CR &= ~RCC_CR_MSIRANGE;               /* clear range */
	 RCC->CR |=  RCC_CR_MSIRANGE_7;   /* range 7 -> 8MHz */
 
	 /* 4. Enable MSI oscillator */
	 RCC->CR |= RCC_CR_MSION;
 
	 /* 5. Wait until MSI is ready */
	 while ((RCC->CR & RCC_CR_MSIRDY) == 0);
 }