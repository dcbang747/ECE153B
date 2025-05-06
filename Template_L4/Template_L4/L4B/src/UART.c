/*


 * ECE 153B  – Lab 4A
 *
 * Low‑level USART (UART) support for STM32L4‑Nucleo.
 *
 * All USARTs run at 9 600 baud, 8 N 1, oversampling‑by‑16 and use the
 * 80 MHz system clock (SYSCLK) as their clock source.
 */

#include "UART.h"
#include "stm32l476xx.h"

#define SYSCLK_HZ  80000000UL
#define BAUDRATE   9600UL

static uint32_t BRR_Value(uint32_t pclk, uint32_t baud)
{
    return (pclk + baud / 2U) / baud;
}

/* ————— Clock enable / source ————— */
void UART1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->CCIPR &= ~(3U << 2);
    RCC->CCIPR |=  (3U << 2);          /* SYSCLK */
}

void UART2_Init(void)
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    RCC->CCIPR &= ~(3U << 4);
    RCC->CCIPR |=  (3U << 4);          /* SYSCLK */
}

/* ————— GPIO ————— */
static void gpio_usart_pins(GPIO_TypeDef *port, uint8_t txPin, uint8_t rxPin)
{
    uint32_t txShift = txPin * 2, rxShift = rxPin * 2;

    /* MODER alt‑func */
    port->MODER &= ~((3U << txShift) | (3U << rxShift));
    port->MODER |=  (2U << txShift) | (2U << rxShift);

    /* OTYPER push‑pull cleared */
    port->OTYPER &= ~(1U << txPin | 1U << rxPin);

    /* Very high speed */
    port->OSPEEDR |= (3U << txShift) | (3U << rxShift);

    /* Pull‑ups */
    port->PUPDR &= ~((3U << txShift) | (3U << rxShift));
    port->PUPDR |=  (1U << txShift) | (1U << rxShift);
}

void UART1_GPIO_Init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;   /* PB6 TX, PB7 RX */
    gpio_usart_pins(GPIOB, 6, 7);
    GPIOB->AFR[0] |= (7U << 24) | (7U << 28);
}

void UART2_GPIO_Init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;   /* PA2 TX, PA3 RX */
    gpio_usart_pins(GPIOA, 2, 3);
    GPIOA->AFR[0] |= (7U << 8) | (7U << 12);
}

/* ————— USART generic config ————— */
void USART_Init(USART_TypeDef *USARTx)
{
    USARTx->CR1 &= ~USART_CR1_UE;                 /* disable               */
    USARTx->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1 |
                     USART_CR1_PCE | USART_CR1_OVER8);
    USARTx->CR2 &= ~USART_CR2_STOP;               /* 1 stop                */
    USARTx->BRR  = BRR_Value(SYSCLK_HZ, BAUDRATE);
    USARTx->CR1 |= USART_CR1_TE | USART_CR1_RE;   /* TX & RX               */
    USARTx->CR1 |= USART_CR1_UE;                  /* enable                */
    while (!(USARTx->ISR & USART_ISR_TEACK));
    while (!(USARTx->ISR & USART_ISR_REACK));
}

/* ————— Low‑level helpers (same as starter, delay extended) ————— */
uint8_t USART_Read(USART_TypeDef *USARTx)
{
    while (!(USARTx->ISR & USART_ISR_RXNE));
    return (uint8_t)USARTx->RDR;
}

void USART_Write(USART_TypeDef *USARTx, uint8_t *buf, uint32_t n)
{
    for (uint32_t i = 0; i < n; ++i) {
        while (!(USARTx->ISR & USART_ISR_TXE));
        USARTx->TDR = buf[i];
        /* long delay so HC‑05 stays happy */
        for (volatile uint32_t d = 0; d < 40000; ++d) __NOP();
    }
    while (!(USARTx->ISR & USART_ISR_TC));
    USARTx->ISR &= ~USART_ISR_TC;
}

void USART_Delay(uint32_t us)
{
    uint32_t t = 100U * us / 7U;
    while (t--) __NOP();
}