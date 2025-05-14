#include "stm32l476xx.h"

static void DAC_Pin_Init(void) {
    /* PA4 analog output */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->MODER &= ~GPIO_MODER_MODE4;
    GPIOA->MODER |=  GPIO_MODER_MODE4;       /* analog (11) */
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD4;
    GPIOA->ASCR  |=  GPIO_ASCR_ASC4;         /* connect switch */
}

void DAC_Write_Value(uint32_t value) {
    if (value > 4095U) value = 4095U;
    DAC1->DHR12R1 = value;                   /* 12-bit right-aligned */
}

void DAC_Init(void) {
    DAC_Pin_Init();

    /* 1 – enable DAC clock */
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;

    /* 2 – be sure channel 1 disabled */
    DAC1->CR &= ~DAC_CR_EN1;

    /* 3 + 4 – software trigger selected but trigger disabled (TEN1 = 0) */
    DAC1->CR &= ~(DAC_CR_TEN1 | DAC_CR_TSEL1);

    /* 5 – normal mode, buffer enabled (MODE1 bits = 00) */
    DAC1->MCR &= ~DAC_MCR_MODE1;

    /* 6 – enable channel 1 */
    DAC1->CR |= DAC_CR_EN1;

    DAC_Write_Value(0);
}
