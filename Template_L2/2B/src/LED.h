/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 2B
 */

#ifndef __STM32L476G_NUCLEO_LED_H
#define __STM32L476G_NUCLEO_LED_H

#include "stm32l476xx.h"

void LED_Init(void);

void Green_LED_Off(void);
void Green_LED_On(void);
void Green_LED_Toggle(void);

#endif
