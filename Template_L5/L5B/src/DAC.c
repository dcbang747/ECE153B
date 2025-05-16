#include "stm32l476xx.h"
#include "DAC.h"

static void DAC_Pin_Init(void);

/*------------------------------------------------------------------*/
static void DAC_Pin_Init(void)
{
    /* PA4 → analog (DAC1_OUT1) */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->MODER |=  GPIO_MODER_MODE4;      /* 11b = analog */
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD4;
    GPIOA->ASCR  |=  GPIO_ASCR_ASC4;
}

/*------------------------------------------------------------------*/
void DAC_Write_Value(uint32_t value)
{
    /* right-aligned 12-bit data register (DHR12R1) */
    value &= 0x0FFFUL;                      /* clamp to 12 bits */
    DAC1->DHR12R1 = value;
}

/*------------------------------------------------------------------*/
void DAC_Init(void)
{
    DAC_Pin_Init();

    /* enable clock */
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;

    /* make sure ch1 disabled first */
    DAC1->CR &= ~DAC_CR_EN1;

    /* TSEL1 = 000 → software trigger;  TEN1 = 0 → trigger disabled */
    DAC1->CR &= ~(DAC_CR_TEN1 | DAC_CR_TSEL1);

    /* MODE1 = 000 → normal mode, buffer enabled, output on PA4 */
    DAC1->MCR &= ~(DAC_MCR_MODE1);

    /* enable channel 1 */
    DAC1->CR |= DAC_CR_EN1;

    DAC_Write_Value(0);
}
c
Copy
Edit
