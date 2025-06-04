/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Project
 */

#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "UART.h"
#include "motor.h"
#include <stdio.h>

static void printMenu(void)
{
    printf("\r\n=== Step-Motor Control ===\r\n"
           "  c - clockwise\r\n"
           "  a - anti-clockwise\r\n"
           "  s - stop\r\n"
           "==========================\r\n");
}

int main(void)
{
    System_Clock_Init();   /* 80 MHz */
    Motor_Init();
    SysTick_Init();

    UART2_GPIO_Init();
    UART2_Init();
    USART_Init(USART2);    /* re-enable safe settings used by printf */

    printf("Program Starts.\r\n");
    printMenu();

    while (1) {
        uint8_t ch = USART_Read(USART2);

        switch (ch) {
        case 'c':
        case 'C':
            setDire(+1);
            printf("Clockwise\r\n");
            break;
        case 'a':
        case 'A':
            setDire(-1);
            printf("Counter-clockwise\r\n");
            break;
        case 's':
        case 'S':
            setDire(0);
            printf("Stopped\r\n");
            break;
        default:
            printMenu();
            break;
        }
    }
}