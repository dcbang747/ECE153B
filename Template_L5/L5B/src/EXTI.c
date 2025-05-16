#include "EXTI.h"
#include "DAC.h"
#include "stm32l476xx.h"

#define DAC_MIN       0U
#define DAC_MAX    4095U
#define DAC_INCREMENT 256U

volatile uint32_t dac_value = 0;
static enum { DOWN, UP } direction = UP;

/*------------------------------------------------------------------*/
void EXTI_Init(void)
{
    /* 1. Enable clocks ---------------------------------------------------- */
    RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOCEN;     /* GPIOC clock       */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;     /* *** SYSCFG clock ***/

    /* 2. Map EXTI line 13 to PC13 ---------------------------------------- */
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR4_EXTI13_PC;

    /* 3. Select edge(s) --------------------------------------------------- */
    EXTI->FTSR1 |= EXTI_FTSR1_FT13;    /* comment this line if you want both */

    /* 4. Unmask & NVIC ---------------------------------------------------- */
		EXTI->PR1|= EXTI_PR1_PIF13;
    EXTI->IMR1  |=  EXTI_IMR1_IM13;     /* line enable    */
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    NVIC_SetPriority(EXTI15_10_IRQn, 0);
}

/*------------------------------------------------------------------*/
void EXTI15_10_IRQHandler(void)
{        /* clear flag */
        dac_value += DAC_INCREMENT;
        DAC_Write_Value(dac_value);
				
				EXTI->PR1|= EXTI_PR1_PIF13;
}
