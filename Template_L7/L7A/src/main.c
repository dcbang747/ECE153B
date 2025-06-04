/*  main.c  – ECE 153B “non-blocking stepper” demo
 *
 *  UART   : 115 200 Bd on USART2 (ST-LINK VCP, PA2/PA3)
 *  Stepper: 28BYJ-48 on PC5 PC6 PC8 PC9 (ULN2003 board)
 *  Build  : gcc-arm-none-eabi  –mcpu=cortex-m4 –mthumb –O2 …
 */

#include "stm32l476xx.h"
#include "SysClock.h"
#include "StepMotor.h"
#include "UART.h"
#include <stdio.h>

#define SYSTICK_HZ 1000U   /* 1 ms tick */

static void SysTick_Init(void)
{
    SysTick->LOAD  = (SystemCoreClock / SYSTICK_HZ) - 1;
    SysTick->VAL   = 0;
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk;
}

/* ---------- USART2 RX interrupt ------------------------------------------------ */
void USART2_IRQHandler(void)
{
    if (USART2->ISR & USART_ISR_RXNE)        /* byte arrived? */
    {
        char c = USART2->RDR;                /* clears RXNE  */
        switch (c)
        {
            case 'c': StepMotor_SetDir(STEPMOTOR_CW);   printf("CW\r\n");   break;
            case 'a': StepMotor_SetDir(STEPMOTOR_CCW);  printf("CCW\r\n");  break;
            case 's': StepMotor_SetDir(STEPMOTOR_STOP); printf("STOP\r\n"); break;
            default : break;                 /* ignore junk  */
        }
    }
}

int main(void)
{
    System_Clock_Init();                     /* 80 MHz core      */
    UART2_GPIO_Init();
    UART2_Init();                            /* 115 200 Bd       */

    /* enable RX interrupt */
    USART2->CR1 |= USART_CR1_RXNEIE;
    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ  (USART2_IRQn);

    StepMotor_Init();                        /* PC pins + table  */
    SysTick_Init();                          /* 1 ms heartbeat   */

    printf("\r\nStepper demo ready – type 'c', 'a', or 's'\r\n");

    while (1)                                /* nothing to do – sleep */
        __WFI();                             /* CPU wakes on IRQs     */
}
