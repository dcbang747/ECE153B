/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Project
 */

#ifndef __STM32L476_UART_H
#define __STM32L476_UART_H

#include "stm32l476xx.h"
#include <stdint.h>

#define IO_SIZE      256        /* size of each TX buffer            */
#define QUEUE_DEPTH   32        /* #queued strings (≥2)              */

/* public API */
void UART1_Init(void);          /* Bluetooth */
void UART2_Init(void);          /* USB VCP   */

void UART_print(char *s);       /* non-blocking printf-style */
void UART_onInput(char *buf, uint32_t len);   /* defined in main */

#endif

