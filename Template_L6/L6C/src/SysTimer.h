/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6C
 */

#ifndef __STM32L476G_NUCLEO_SYSTICK_H
#define __STM32L476G_NUCLEO_SYSTICK_H

#include "stm32l476xx.h"

void SysTick_Init(void);
void SysTick_Handler(void);
void delay(uint32_t ms);
void startTimer(void);
uint32_t endTimer(void);

#endif
