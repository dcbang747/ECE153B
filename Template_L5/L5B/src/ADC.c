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
    /* 1 – I/O analog-switch voltage booster */
        SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;

        /* 2 – Common ADC settings                          *
        *    • VREFINT on                                   *
        *    • No prescaler (PRESC = 0000)                  *
        *    • CKMODE = 01 → HCLK/1 synchronous             *
        *    • Independent mode (DUAL = 0000)               */
        ADC123_COMMON->CCR &= ~(ADC_CCR_PRESC | ADC_CCR_CKMODE |
                                ADC_CCR_DUAL  | ADC_CCR_VREFEN);
        ADC123_COMMON->CCR |=  ADC_CCR_VREFEN | ADC_CCR_CKMODE_0;
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
	    /* Ensure ADC disabled before config */
    if (ADC1->CR & ADC_CR_ADEN) {
        ADC1->CR |= ADC_CR_ADDIS;
        while (ADC1->CR & ADC_CR_ADEN);
    }

    /* 12-bit resolution, right alignment */
    ADC1->CFGR &= ~(ADC_CFGR_RES | ADC_CFGR_ALIGN);

    /* Single-conversion sequence length = 1, SQ1 = channel 6 */
    ADC1->SQR1 &= ~(ADC_SQR1_L | (0x1F << 6));
    ADC1->SQR1 |=  (6U << 6);

    /* Channel 6 → single-ended */
    ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_6;

    /* Sample time channel 6 = 24.5 cycles (0b011) */
    ADC1->SMPR1 &= ~ADC_SMPR1_SMP6;
    ADC1->SMPR1 |=  (ADC_SMPR1_SMP6_1 | ADC_SMPR1_SMP6_0);

    /* Single conversion, software trigger only */
    ADC1->CFGR &= ~(ADC_CFGR_CONT | ADC_CFGR_EXTEN);

    /* Enable ADC and wait for readiness */
    ADC1->ISR |=  ADC_ISR_ADRDY;        /* clear */
    ADC1->CR  |=  ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
}
