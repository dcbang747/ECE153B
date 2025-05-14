#include "EXTI.h"
#include "DAC.h"
#include "stm32l476xx.h"

/* USER button on PC13, active-low */
void EXTI_Init(void) {
    /* Enable GPIOC clock */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    /* PC13 input with pull-up */
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;
    GPIOC->PUPDR |=  GPIO_PUPDR_PUPD13_0;   /* pull-up (01) */

    /* Map EXTI13 to PC13 */
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR4_EXTI13_PC;

    /* Falling-edge trigger, unmask line 13 */
    EXTI->FTSR1 |=  EXTI_FTSR1_FT13;
    EXTI->RTSR1 &= ~EXTI_RTSR1_RT13;
    EXTI->IMR1  |=  EXTI_IMR1_IM13;

    /* Clear pending and enable NVIC */
    EXTI->PR1 = EXTI_PR1_PIF13;
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

#define DAC_MIN       0U
#define DAC_MAX    4095U
#define DAC_INCREMENT 256U

static uint32_t dac_value = 0;
static enum { DOWN, UP } direction = UP;

void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR1 & EXTI_PR1_PIF13) {
        EXTI->PR1 = EXTI_PR1_PIF13;          /* clear flag */

        if (direction == UP) {
            if (dac_value + DAC_INCREMENT >= DAC_MAX) {
                dac_value = DAC_MAX;
                direction = DOWN;
            } else {
                dac_value += DAC_INCREMENT;
            }
        } else { /* direction == DOWN */
            if (dac_value <= DAC_INCREMENT) {
                dac_value = DAC_MIN;
                direction = UP;
            } else {
                dac_value -= DAC_INCREMENT;
            }
        }
        DAC_Write_Value(dac_value);
    }
}   