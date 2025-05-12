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

int main(void) {
    // Initialization
    System_Clock_Init(); // Switch System Clock = 16 MHz

    ADC_Init();

    // [TODO] Initialize PWM
	  LED_Pin_Init();
	  TIM2_CH1_Init();
		
		unsigned int measurement;
		unsigned int Vin;
	  unsigned int duty = 0;
    while (1) {
        // [TODO] Trigger ADC and get result
				ADC1->CR |= ADC_CR_ADSTART;
				while((ADC1->CSR) != ADC_CSR_EOC_MST);	//wait until adc conversion complete
			
				measurement = ADC1->DR;								//store measurement from data register
				Vin = (measurement/4096)*3.3;					//input voltage
			 
        // [TODO] LED behavior based on ADC result
			  duty = Vin *1000/3.3;
				TIM2->CCR1 = duty;
			
				if(duty > 1000) {duty = 1000;}
				if(duty < 0) {duty = 0;}
			
    }
}
