#include "ADC.h"
#include "stm32l476xx.h"
#include <stdint.h>

void ADC_Wakeup(void) {
    /* ~20 µs delay (16 MHz ⇒ 320 cycles) */
    volatile uint32_t wait_time = 320;

    /* Leave deep-power-down if required */
    if (ADC1->CR & ADC_CR_DEEPPWD) {
        ADC1->CR &= ~ADC_CR_DEEPPWD;
    }

    /* Enable internal regulator */
    ADC1->CR |= ADC_CR_ADVREGEN;

    /* Wait start-up time */
    while (wait_time--) { __NOP(); }
}

void ADC_Common_Configuration(void) {
    /* 1 – analog-switch booster */
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;

    /* 2 – VREFINT ON, no prescaler, HCLK/1 synchronous, independent */
    ADC123_COMMON->CCR &= ~(ADC_CCR_PRESC | ADC_CCR_CKMODE |
                            ADC_CCR_DUAL  | ADC_CCR_VREFEN);
    ADC123_COMMON->CCR |=  ADC_CCR_VREFEN | ADC_CCR_CKMODE_0;
}

void ADC_Pin_Init(void) {
    /* PA1 analog input */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->MODER &= ~GPIO_MODER_MODE1;
    GPIOA->MODER |=  GPIO_MODER_MODE1;      /* analog (11) */
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD1;
    GPIOA->ASCR  |=  GPIO_ASCR_ASC1;        /* connect switch */
}

void ADC_Init(void) {
    /* Enable & reset ADC clock */
    RCC->AHB2ENR  |=  RCC_AHB2ENR_ADCEN;
    RCC->AHB2RSTR |=  RCC_AHB2RSTR_ADCRST;
    RCC->AHB2RSTR &= ~RCC_AHB2RSTR_ADCRST;

    ADC_Pin_Init();
    ADC_Common_Configuration();
    ADC_Wakeup();

    /* Make sure ADC disabled before config */
    if (ADC1->CR & ADC_CR_ADEN) {
        ADC1->CR |= ADC_CR_ADDIS;
        while (ADC1->CR & ADC_CR_ADEN);
    }

    /* 12-bit, right align */
    ADC1->CFGR &= ~(ADC_CFGR_RES | ADC_CFGR_ALIGN);

    /* Sequence length 1, SQ1 = channel 6 (PA1) */
    ADC1->SQR1 &= ~(ADC_SQR1_L | (0x1F << 6));
    ADC1->SQR1 |=  (6U << 6);

    /* Single-ended mode */
    ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_6;

    /* Sample time ch 6 = 24.5 cycles (0b011) */
    ADC1->SMPR1 &= ~ADC_SMPR1_SMP6;
    ADC1->SMPR1 |=  (ADC_SMPR1_SMP6_1 | ADC_SMPR1_SMP6_0);

    /* Single conversion, SW trigger only */
    ADC1->CFGR &= ~(ADC_CFGR_CONT | ADC_CFGR_EXTEN);

    /* Enable ADC */
    ADC1->ISR |=  ADC_ISR_ADRDY;      /* clear */
    ADC1->CR  |=  ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
}