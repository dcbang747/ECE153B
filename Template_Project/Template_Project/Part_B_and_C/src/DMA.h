/*
 * ECE 153B – DMA helper for UART-TX
 */

#ifndef __STM32L476_UART_DMA_H
#define __STM32L476_UART_DMA_H

#include "stm32l476xx.h"

/* configure DMA1-channel for the given UART’s TX */
void DMA_UART_TX_Init(DMA_Channel_TypeDef *chan, USART_TypeDef *uart);

/* IRQ handlers – just prototypes so user code links */
void DMA1_Channel4_IRQHandler(void);   /* USART1_TX */
void DMA1_Channel7_IRQHandler(void);   /* USART2_TX */

#endif
