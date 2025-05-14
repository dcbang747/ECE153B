/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 5A
 */


#include "ADC.h"
#include "PWM.h"
#include "SysClock.h"
#include "stm32l476xx.h"
#include <stdio.h>

	volatile uint32_t measurement = 0;

int main(void) {
    System_Clock_Init();   /* 8 MHz MSI */
    ADC_Init();

    // [TODO] Initialize PWM
	  LED_Pin_Init();
	  TIM2_CH1_Init();
		
    while (1) {
        /* Trigger a single conversion */
        ADC1->CR |= ADC_CR_ADSTART;
        while (!(ADC1->ISR & ADC_ISR_EOC));   /* wait */

        /* Read result (clears EOC) */
        measurement = ADC1->DR;

        /* Map 0-4095 → 0-1000 duty */
        uint32_t duty = (measurement * 1000U) / 4095U;
        TIM2->CCR1   = duty;                  /* update PWM */
    }
}
