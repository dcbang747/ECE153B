#include "EXTI.h"
#include "DAC.h"
#include "stm32l476xx.h"

#define DAC_MIN       0U
#define DAC_MAX    4095U
#define DAC_INCREMENT 256U

volatile int dac_value = 000;
static enum { DOWN, UP } direction = UP;

/*------------------------------------------------------------------*/
void EXTI_Init(void)
{
    /* 1. Clocks ----------------------------------------------------*/
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;      /* GPIOC */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;     /* SYSCFG */

    /* 2. PC13 pin --------------------------------------------------*/
    GPIOC->MODER &= ~GPIO_MODER_MODE13;       /* input  */
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;
    GPIOC->PUPDR |=  GPIO_PUPDR_PUPD13_1;     /* pull-up */

    /* 3. Route EXTI13 to port C -----------------------------------*/
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR4_EXTI13_PC;          /* 0b0010 = C */

    /* 4. Edge + mask ----------------------------------------------*/
    EXTI->FTSR1 |=  EXTI_FTSR1_FT13;          /* falling on  */
    EXTI->RTSR1 &= ~EXTI_RTSR1_RT13;          /* rising off  */
    EXTI->IMR1  |=  EXTI_IMR1_IM13;           /* unmask      */

    /* 5. Clear any stale pending bit ------------------------------*/
    EXTI->PR1 = EXTI_PR1_PIF13;

    /* 6. NVIC ------------------------------------------------------*/
    NVIC_SetPriority(EXTI15_10_IRQn, 0);
    NVIC_EnableIRQ  (EXTI15_10_IRQn);
}

// [TODO] Write Interrupt Handlers (look in startup_stm32l476xx.s to find the 
// interrupt handler names that you should use)
void EXTI15_10_IRQHandler(void)
{
        dac_value += 256;
        DAC_Write_Value(dac_value);
				
				EXTI->PR1|= EXTI_PR1_PIF13;
}
