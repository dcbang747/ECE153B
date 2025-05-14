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

		unsigned int measurement;

int main(void) {
    // Initialization
    System_Clock_Init(); // Switch System Clock = 16 MHz

    ADC_Init();

    // [TODO] Initialize PWM
	  LED_Pin_Init();
	  TIM2_CH1_Init();
		
		unsigned int Vref;
	  unsigned int duty = 0;

    while (1) {
        // [TODO] Trigger ADC and get result
				ADC1->CR |= ADC_CR_ADSTART;
				while(!(ADC1->ISR & ADC_ISR_EOC));	//wait until adc conversion complete
				
				measurement = ADC1->DR;								//store measurement from data register
				
				duty = (measurement-1980)/4;					//input voltage
			
				Vref = 3000*4096/measurement;

				if(duty > 1000) {duty = 1000;}
				if(duty < 0) {duty = 0;}
			
			
        // [TODO] LED behavior based on ADC result
				TIM2->CCR1 = duty;
			
    }
}
