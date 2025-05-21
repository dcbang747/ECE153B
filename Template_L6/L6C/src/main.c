/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6C
 */

#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "LED.h"
#include "CRC.h"
#include "DMA.h"
#include "UART.h"
#include <stdio.h>
#include <string.h>

/* Expected CRC Value */
static uint32_t uwExpectedCRCValue = 0x5A60861E;	
static volatile uint32_t ComputedCRC;	
static volatile uint32_t ComputationDone = 0;	

void completeCRC(uint32_t crc){
    ComputedCRC     = crc;
    ComputationDone = 1;
}

int main(void) {
  	uint32_t time;
	System_Clock_Init(); // 80 hz
	SysTick_Init();
	LED_Init();	
	CRC_Init();
	DMA_Init();

	UART2_GPIO_Init();
	UART2_Init();
	USART_Init(USART2);
	// Switch System Clock = 80 MHz
	// systick init
	// uart init
	// led, crc, dma init
    while (1)
    {
        LED_Toggle();

        startTimer();
				ComputationDone = 0;

        CRC->CR |= CRC_CR_RESET;  /* restart CRC engine                       */
        DMA1_Channel6->CNDTR = BUFFER_SIZE;                       /* word count */
        DMA1_Channel6->CCR |= DMA_CCR_EN;                         /* go         */

		while(!ComputationDone) __WFI();						  /* sleep */

        DMA1_Channel6->CCR &= ~DMA_CCR_EN;                        /* tidy up    */		
		
        uint32_t elapsed_us = endTimer();

        if (ComputedCRC != uwExpectedCRCValue)
        {
            LED_Off();            /* indicate failure and halt                */
            while (1);
        }

        char msg[64];
        sprintf(msg, "Hardware CRC time: %lu us\r\n", elapsed_us);
        USART_Write(USART2, (uint8_t *)msg, strlen(msg));

        //delay(1000);              /* wait one second before next cycle        */
    }
}


