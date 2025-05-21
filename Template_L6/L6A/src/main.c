/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6A
 */

#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "LED.h"
#include "CRC.h"
#include "UART.h"
#include <stdio.h>
#include <string.h>

#define EXPECTED_CRC  0x5A60861EU

static volatile uint32_t Software_ComputedCRC;

int main(void)
{
    System_Clock_Init();          /* 80 MHz                               */
    SysTick_Init();
    LED_Init();

    UART2_GPIO_Init();
    UART2_Init();
    USART_Init(USART2);           /* 9600 baud @ SYSCLK                   */

    while (1)
    {
        Green_LED_Toggle();

        uint32_t crc = 0xFFFFFFFFUL;

        startTimer();
        for (uint32_t i = 0; i < BUFFER_SIZE; ++i)
            crc = CrcSoftwareFunc(crc, DataBuffer[i], CRC_POLYNOMIAL);
        uint32_t elapsed_us = endTimer();

        Software_ComputedCRC = crc;

        if (crc != EXPECTED_CRC)
        {
            Green_LED_Off();      /* signal error, then halt              */
            while (1);
        }

        char msg[64];
        sprintf(msg, "Software CRC time: %lu us\r\n", elapsed_us);
        USART_Write(USART2, (uint8_t *)msg, strlen(msg));

        delay(1000);              /* one-second gap for Termite           */
    }
}