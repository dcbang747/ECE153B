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

volatile uint32_t g_adc_value = 0;      /* for Logic-Analyzer tracing */

int main(void)
{
    /* clocks & peripherals -------------------------------------------------- */
    System_Clock_Init();   
    ADC_Init();
    LED_Pin_Init();
    TIM2_CH1_Init();

    /* main loop ------------------------------------------------------------- */
    while (1) {
        /* trigger single ADC conversion */
        ADC1->CR |= ADC_CR_ADSTART;
        while (!(ADC1->ISR & ADC_ISR_EOC));      /* wait -- EOC flag */

        g_adc_value = ADC1->DR;                  /* read + clear flag */

        /* map 0-4095 → 0-1000 duty */
        uint32_t duty = (g_adc_value * 1000U) / 4095U;
        TIM2->CCR1 = duty;
    }
}