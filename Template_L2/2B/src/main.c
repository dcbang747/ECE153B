/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 2B
 */

#include "stm32l476xx.h"
#include "EXTI.h"
#include "LED.h"
#include "SysClock.h"

int main() {
	// Initialization
	System_Clock_Init();
	LED_Init();
	EXTI_Init();
	
	while(1);
	
	return 0;
}
