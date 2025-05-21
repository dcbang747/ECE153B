/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6C
 */

#ifndef __STM32L476G_DISCOVERY_DMA_H
#define __STM32L476G_DISCOVERY_DMA_H

#include "stm32l476xx.h"

void DMA_Init(void);
void DMA1_Channel6_IRQHandler(void);
void completeCRC(uint32_t crc);

#endif /* __STM32L476G_DISCOVERY_DMA_H */
