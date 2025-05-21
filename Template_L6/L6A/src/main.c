/*  main.c  — Part A: compute CRC in software and time it           */

#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "LED.h"
#include "CRC.h"
#include "UART.h"
#include <string.h>
#include <stdio.h>

#define EXPECTED_CRC 0x5A60861EU

int main(void)
{
    System_Clock_Init();          /* 80 MHz                    */
    SysTick_Init();
    LED_Init();

    UART2_GPIO_Init();
    UART2_Init();
    USART_Init(USART2);           /* 9600 baud                 */

    while (1)
    {
        LED_Toggle();

        uint32_t crc = INITIAL_CRC_VALUE;

        startTimer();
        for (uint32_t i = LOWER; i < UPPER; i += STEP)
            crc = CrcSoftwareFunc(crc, DataBuffer[i], POLYNOME);
        uint32_t us = endTimer();

        if (crc != EXPECTED_CRC)
        {
            LED_Off();      /* indicate failure          */
            while (1);
        }

        char msg[64];
        printf(msg, "Software CRC time: %lu us\r\n", us);
        USART_Write(USART2, (uint8_t *)msg, strlen(msg));

        delay(1000);              /* one-second pause          */
    }
}
