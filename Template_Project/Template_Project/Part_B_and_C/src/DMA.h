/*
 * ECE 153B – DMA helper for UART-TX
 */

#ifndef __STM32L476G_DISCOVERY_DMA_H
#define __STM32L476G_DISCOVERY_DMA_H

#include "stm32l476xx.h"

/* configure one DMA channel as a USART-TX helper                            */
void DMA_Init_UARTx(DMA_Channel_TypeDef *ch, USART_TypeDef *uart);

/* IRQ handlers – only TX is used here                                       */
void DMA1_Channel7_IRQHandler(void);     /* USART2_TX  */
void DMA1_Channel4_IRQHandler(void);     /* USART1_TX  */

#endif
