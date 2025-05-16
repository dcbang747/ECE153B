#include "EXTI.h"
#include "DAC.h"
#include "stm32l476xx.h"

/* USER button on PC13, active-low */
void EXTI_Init(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    /* PC13 input with pull-up */
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR13;
    GPIOC->PUPDR |=  GPIO_PUPDR_PUPDR13_0;

    /* Map EXTI13 to PC */
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR4_EXTI13_PC;

    /* Falling-edge trigger, unmask */
    EXTI->FTSR1 |=  (1U << 13);
    EXTI->RTSR1 &= ~(1U << 13);
    EXTI->IMR1  |=  (1U << 13);

    EXTI->PR1   |=  (1U << 13);     /* clear pending */
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}



#define DAC_MIN       0U
#define DAC_MAX    4095U
#define DAC_INCREMENT 256U

static uint32_t dac_value = 0;
static enum { DOWN, UP } direction = UP;

void EXTI15_10_IRQHandler(void) {
        if (EXTI->PR1 & (1U << 13)) {
            EXTI->PR1 = (1U << 13);        

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