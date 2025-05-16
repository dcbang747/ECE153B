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
    // Force reset of Trace
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;   /* disable */
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk;   /* re-enable */
    TPI->SPPR         = 2;                             /* NRZ protocol */
    TPI->FFCR        |= TPI_FFCR_EnFCont_Msk;          /* flush-on-overflow */

    ADC_Init();

    // [TODO] Initialize PWM
	  LED_Pin_Init();
	  TIM2_CH1_Init();
      uint32_t decimator = 0; 
		
    while (1) {
        /* Trigger a single conversion */
        ADC1->CR |= ADC_CR_ADSTART;
        while (!(ADC1->ISR & ADC_ISR_EOC));   /* wait */

        /* Read result (clears EOC) */
        uint16_t raw = ADC1->DR;
        measurement = raw;

        if ((decimator++ & 0xFF) == 0) {
            adc_trace = raw;       Delayer
        }

        /* Map 0-4095 → 0-1000 duty */
        uint32_t duty = (measurement * 1000U) / 4095U;
        TIM2->CCR1   = duty;                  /* update PWM */
    }
}
