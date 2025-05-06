#include "SysTimer.h"
#include "stm32l476xx.h"

static volatile uint32_t msTicks = 0;

/* ——— 1 ms SysTick init ——— */
void SysTick_Init(void)
{
    SysTick->CTRL = 0;
    SysTick->LOAD = 80000U - 1U;         /* 80 MHz / 80 000 = 1 kHz         */
    SysTick->VAL  = 0;
    NVIC_SetPriority(SysTick_IRQn, 1);
    NVIC_EnableIRQ(SysTick_IRQn);
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler(void)
{
    ++msTicks;
}

/* ——— millisecond blocking delay ——— */
void delay(uint32_t ms)
{
    uint32_t target = msTicks + ms;
    while ((int32_t)(target - msTicks) > 0);
}
