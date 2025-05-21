/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6B
 */

#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "LED.h"
#include "CRC.h"
#include "UART.h"
#include <stdio.h>

/* Expected CRC Value */
static uint32_t uwExpectedCRCValue = 0x5A60861E;	

int main(void) {
  static uint32_t ComputedCRC;
	uint32_t time;
	
	// Switch System Clock = 80 MHz
	System_Clock_Init(); 
	SysTick_Init();
	LED_Init();	
	CRC_Init();
	UART2_GPIO_Init();
	UART2_Init();
	USART_Init(USART2);
	
    while (1)
    {
        LED_Toggle();

        startTimer();

        CRC->CR |= CRC_CR_RESET;  /* restart CRC engine                       */
        ComputedCRC = CRC_CalcBlockCRC(DataBuffer, BUFFER_SIZE);

        uint32_t elapsed_us = endTimer();

        if (ComputedCRC != EXPECTED_CRC)
        {
            LED_Off();            /* indicate failure and halt                */
            while (1);
        }

        char msg[64];
        sprintf(msg, "Hardware CRC time: %lu us\r\n", elapsed_us);
        USART_Write(USART2, (uint8_t *)msg, strlen(msg));

        delay(1000);              /* wait one second before next cycle        */
    }
}


