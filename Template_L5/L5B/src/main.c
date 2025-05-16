/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 5B
 */


#include "ADC.h"
#include "PWM.h"
#include "SysClock.h"
#include "stm32l476xx.h"
#include <stdio.h>

volatile uint32_t measurement = 000;   /* for Logic-Analyzer viewing */

int main(void) {
    /* Initialisation */
    System_Clock_Init();            /* 16 MHz HSI */
    ADC_Init();
    LED_Pin_Init();
    TIM2_CH1_Init();

    while (1) {
        /* Trigger one ADC conversion */
        ADC1->CR |= ADC_CR_ADSTART;
				for (volatile uint32_t i=0;i<300;i++) __NOP();
        while (!(ADC1->ISR & ADC_ISR_EOC));
        measurement = ADC1->DR;                     /* read & clear EOC */

        /* Map 0-4095 → 0-1000 PWM duty */
        TIM2->CCR1 = (measurement * 1000U) / 4095U;
    }
}