#include "ADC.h"

#include "stm32l476xx.h"

#include <stdint.h>

void ADC_Wakeup(void) {
    int wait_time;

    // To start ADC operations, the following sequence should be applied
    // DEEPPWD = 0: ADC not in deep-power down
    // DEEPPWD = 1: ADC in deep-power-down (default reset state)
    if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD)
        ADC1->CR &= ~ADC_CR_DEEPPWD; // Exit deep power down mode if still in that state

    // Enable the ADC internal voltage regulator
    // Before performing any operation such as launching a calibration or enabling the ADC, the ADC
    // voltage regulator must first be enabled and the software must wait for the regulator start-up
    // time.
    ADC1->CR |= ADC_CR_ADVREGEN;

    // Wait for ADC voltage regulator start-up time
    // The software must wait for the startup time of the ADC voltage regulator (T_ADCVREG_STUP)
    // before launching a calibration or enabling the ADC.
    // T_ADCVREG_STUP = 20 us
    wait_time = 20 * (80000000 / 1000000);
    while (wait_time != 0) {
        wait_time--;
    }
}

void ADC_Common_Configuration() {
    // [TODO]
		SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;		//Enable I/O analog switch voltage booster
		
		ADC123_COMMON->CCR |= ADC_CCR_VREFEN;			//Enable V_refint
		ADC123_COMMON->CCR |= ADC_CCR_PRESC;			//clock not divided
		ADC123_COMMON->CCR |= ADC_CCR_CKMODE_0;		//HCLK/1 clock mode
		ADC123_COMMON->CCR &= ~ADC_CCR_DUAL;			//all ADC in indep mode
}

void ADC_Pin_Init(void) {
    // [TODO]
	// turn on gpioa 
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
		GPIOA->MODER &= ~GPIO_MODER_MODE1;		//Clear pin PA1
		GPIOA->MODER |= GPIO_MODER_MODE1;			//Analog mode
		GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD1;		//No pull up, no pull down
	
		GPIOA->ASCR  |= GPIO_ASCR_ASC1;
}

void ADC_Init(void) {
    // [TODO] Enable & Reset ADC Clock
		RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
		RCC->AHB2RSTR |= RCC_AHB2RSTR_ADCRST;		//2. reset adc
		RCC->AHB2RSTR &= ~RCC_AHB2RSTR_ADCRST;	//clear reset bit
	
    // Other ADC Initialization
    ADC_Pin_Init();
    ADC_Common_Configuration();
    ADC_Wakeup();
		ADC1->CR &= ~ADC_CR_ADEN;							
    // [TODO] Other Configuration
		//ADC1->CR |= ADC_CR_ADDIS;								//3. disable adc
		ADC1->CFGR &= ~ADC_CFGR_RES;						//4. 12 bit resolution, bits 4:3 = 00
		ADC1->CFGR &= ~ADC_CFGR_ALIGN;					//right align, bit 5 = 0
		
		ADC1->SQR1 &= ~ADC_SQR1_L;							//5. sequence length of 1, all bits 0
		ADC1->SQR1 |= (ADC_SQR1_SQ1_2 | ADC_SQR1_SQ1_1); //first conversion is channel 6, bits 10:6 = 0110
	
		ADC1->DIFSEL |= ADC_DIFSEL_DIFSEL_6;		//6. channel 6 to single ended mode
		ADC1->SMPR1 |= (ADC_SMPR1_SMP6_1 | ADC_SMPR1_SMP6_0);		//7. sampling time 24.5
	
		ADC1->CFGR &= ~ADC_CFGR_CONT;					//8. Single conversion mode
		ADC1->CFGR &= ~ADC_CFGR_EXTEN;				//hardware trigger dectetion disabled
	
		ADC1->CR |= ADC_CR_ADEN;							//9. Enable adc
	
		while(!(ADC1->ISR & ADC_ISR_ADRDY));							//wait until adc ready
}
