/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 4B
 */

#include "stm32l476xx.h"
#include "I2C.h"
#include "SysClock.h"
#include "UART.h"
#include <string.h>
#include <stdio.h>

// Initializes USARTx
// USART2: UART Communication with Termite
// USART1: Bluetooth Communication with Phone
static inline void delay_cycles(uint32_t n)
{
    while (n--) __NOP();
}

void Init_USARTx(int x)
{
    if (x == 1) {
        UART1_Init();
        UART1_GPIO_Init();
        USART_Init(USART1);
    } else {
        UART2_Init();
        UART2_GPIO_Init();
        USART_Init(USART2);
    }
}

int main(void)
{
    System_Clock_Init();          /* 80 MHz core                                    */
    I2C_GPIO_Init();              /* PB6/PB7                                        */
    I2C_Initialization();         /* I²C1 master                                    */
    Init_USARTx(2);               /* USART2 → Termite                               */

    const uint8_t TC74_ADDR = 0x48 << 1;    /* TC74A0 default (1001000b)           */
    const uint8_t READ_TEMP_CMD = 0x00;     /* Register pointer for temperature    */

    while (1)
    {
        /* Send register pointer (write)                                          */
        if (I2C_SendData(I2C1, TC74_ADDR, (uint8_t *)&READ_TEMP_CMD, 1) < 0) {
            printf("I2C‑WRITE‑ERR\r\n");
            continue;
        }

        /* Read one byte temperature                                              */
        uint8_t raw;
        if (I2C_ReceiveData(I2C1, TC74_ADDR, &raw, 1) < 0) {
            printf("I2C‑READ‑ERR\r\n");
            continue;
        }

        /* TC74 returns signed 8‑bit Celsius                                      */
        printf("Temp: %d C\r\n", (int8_t)raw);

        /* ≈1 s delay (coarse)                                                    */
        delay_cycles(8000000);
    }
}
