/*
 * ECE 153B  – Lab 6A  (Part A: software CRC)
 *
 * Name(s):
 * Section:
 */

#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "LED.h"
#include "CRC.h"
#include "UART.h"

#include <stdio.h>
#include <string.h>

#define EXPECTED_CRC 0x5A60861EU          /* value in lab hand-out         */

int main(void)
{
    System_Clock_Init();                  /* 80 MHz core clock             */
    SysTick_Init();
    LED_Init();

    UART2_GPIO_Init();
    UART2_Init();
    USART_Init(USART2);                   /* 9600 baud, 8N1                */

    while (1)
    {
        Green_LED_Toggle();               /* visual heartbeat              */

        uint32_t crc = INITIAL_CRC_VALUE; /* seed defined in CRC.h         */

        startTimer();
        for (uint32_t i = LOWER; i < UPPER; i += STEP)
            crc = CrcSoftwareFunc(crc, DataBuffer[i], POLYNOME);
        uint32_t elapsed_us = endTimer();

        if (crc != EXPECTED_CRC)          /* verification                  */
        {
            Green_LED_Off();
            while (1);                    /* stop on failure               */
        }

        char msg[64];
        sprintf(msg, "Software CRC time: %lu us\r\n", elapsed_us);
        USART_Write(USART2, (uint8_t *)msg, strlen(msg));

        delay(1000);                      /* pause one second              */
    }
}
