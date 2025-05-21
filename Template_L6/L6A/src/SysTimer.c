/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6A
 */

#include "SysTimer.h"

static uint32_t volatile step;

void SysTick_Init(void) {
	// SysTick Control & Status Register
	SysTick->CTRL = 0; // Disable SysTick IRQ and SysTick Counter
	
	// Enables SysTick exception request
	// 1 = counting down to zero asserts the SysTick exception request
	// 0 = counting down to zero does not assert the SysTick exception request
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;      /* processor clk =80 MHz*/
	
	// Select clock source
	// If CLKSOURCE = 0, the external clock is used. The frequency of SysTick clock is the frequency of the AHB clock divided by 8.
	// If CLKSOURCE = 1, the processor clock is used.
	// TODO
	
	// Configure and Enable SysTick interrupt in NVIC
	NVIC_EnableIRQ(SysTick_IRQn);
	NVIC_SetPriority(SysTick_IRQn, 1); // Set Priority to 1
}

void SysTick_Handler(void)
{
    ++step;                                           /* one under-flow       */
}

void delay(uint32_t ms)
{
    step = 0;
    SysTick->LOAD = (SYSTICK_CLK_HZ / 1000UL) - 1UL;  /* 1 ms period          */
    SysTick->VAL  = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;         /* start counting       */
    while (step < ms) { __NOP(); }
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;        /* stop                 */
}

void startTimer(void)
{
    step = 0;
    SysTick->LOAD = 0x00FFFFFFUL;                     /* 24-bit max           */
    SysTick->VAL  = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;         /* run free             */
}

uint32_t endTimer(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;        /* freeze               */
    uint32_t ticks  = step * (SysTick->LOAD + 1UL) +
                      (SysTick->LOAD - SysTick->VAL); /* elapsed core cycles  */
    return ticks / 80UL;                              /* → µs (@80 MHz)       */
}