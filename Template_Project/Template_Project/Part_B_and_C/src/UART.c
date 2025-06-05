/*
 * ECE 153B – 2-buffer DMA UART (non-blocking printf-style)
 */

#include <string.h>
#include <stdio.h>
#include "UART.h"
#include "DMA.h"
#include "motor.h"

#define UART_BAUD  9600U

/* ---------------- private globals ---------------- */
static DMA_Channel_TypeDef *tx_ch      = DMA1_Channel7;   /* USART2_TX        */
static volatile uint8_t     buf0[IO_SIZE];
static volatile uint8_t     buf1[IO_SIZE];
static volatile uint8_t    *active = buf0;
static volatile uint8_t    *pend   = buf1;
static volatile uint16_t    pend_len = 0;
static volatile uint8_t     tx_busy  = 0;

static char                 line[IO_SIZE];
static uint16_t             in_len   = 0;

/* ---------------- helpers ---------------- */
static inline void dma_start(uint8_t *mem, uint16_t len)
{
    tx_ch->CCR  &= ~DMA_CCR_EN;             /* disable first        */
    tx_ch->CMAR  = (uint32_t)mem;
    tx_ch->CNDTR = len;
    tx_ch->CCR  |= DMA_CCR_EN;              /* go!                  */
    USART2->CR3 |= USART_CR3_DMAT;         /* link DMA             */
    tx_busy      = 1;
}

/* ---------------- public init ---------------- */
void UART2_GPIO_Init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    /* PA2 TX, PA3 RX – AF7 */
    GPIOA->MODER   &= ~(3U<<(2*2) | 3U<<(3*2));
    GPIOA->MODER   |=  (2U<<(2*2) | 2U<<(3*2));
    GPIOA->AFR[0]  &= ~(0xF<<(2*4) | 0xF<<(3*4));
    GPIOA->AFR[0]  |=  (7U<<(2*4) | 7U<<(3*4));
}

void UART2_Init(void)
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    /* 80 MHz / 9600 = 8333 → 0x208D  */
    USART2->BRR = 0x208D;
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
    USART2->CR1 |= USART_CR1_UE;

    NVIC_SetPriority(USART2_IRQn, 0);
    NVIC_EnableIRQ (USART2_IRQn);

    /* DMA helper */
    DMA_Init_UARTx(tx_ch, USART2);
}

void USART_Init(USART_TypeDef *u) { (void)u; }   /* compatibility no-op */

/* ---------------- non-blocking print ---------------- */
void UART_print(char *s)
{
    uint16_t len = strlen(s);
    if (len == 0 || len >= IO_SIZE) return;

    __disable_irq();
    if (!tx_busy) {                          /* channel idle – send now      */
        memcpy((void*)active, s, len);
        dma_start((uint8_t*)active, len);
    } else {                                 /* DMA busy – queue in pend buf */
        memcpy((void*)pend,  s, len);
        pend_len = len;
    }
    __enable_irq();
}

/* ---------------- RX assembler ---------------- */
static void transfer_data(char ch)
{
    if (in_len < IO_SIZE-1) line[in_len++] = ch;

    if (ch == '\n') {                        /* full line ready              */
        line[in_len-1] = '\0';               /* kill newline                 */
        extern void UART_onInput(char*,uint32_t);
        UART_onInput((char*)line, in_len-1);
        in_len = 0;
    }
}

/* ---------------- TX done hook ---------------- */
void on_complete_transfer(void)
{
    tx_busy = 0;
    if (pend_len) {                          /* swap & fire next             */
        uint8_t *tmp = active; active = pend; pend = tmp;
        uint16_t l   = pend_len; pend_len = 0;
        dma_start((uint8_t*)active, l);
    }
}

/* ---------------- IRQ handler ---------------- */
void USART2_IRQHandler(void)
{
    if (USART2->ISR & USART_ISR_RXNE) {              /* byte arrived */
        char c = (char)USART2->RDR;
        transfer_data(c);
    }
}
