/*
 * Two-buffer, queue-in-RAM, DMA-driven UART
 *  – USART2 (PA2/PA3) for ST-LINK VCP
 *  – USART1 (PB6/PB7) for Bluetooth
 *
 * Only TX uses DMA; RX is IRQ-driven.
 */

#include <string.h>
#include "UART.h"
#include "DMA.h"

/* ---------- build-time parameters ---------- */
static const uint32_t BAUD_BRR = 0x208D;   /* 80 MHz / 9600 */

/* ---------- ring of pending strings ---------- */
static volatile uint8_t  pool[QUEUE_DEPTH][IO_SIZE];
static volatile uint8_t *active  = pool[0];
static volatile uint16_t act_len = 0;              /* bytes left */
static uint8_t  head = 0, tail = 0;                /* queue indices */

/* share one DMA channel variable so code is generic */
static DMA_Channel_TypeDef *tx_chan;
static USART_TypeDef       *uart_tx;

/* ---------- GPIO helpers ---------- */
static void USART1_GPIO(void)            /* PB6 / PB7 */
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    GPIOB->MODER   &= ~(3U<<(6*2) | 3U<<(7*2));
    GPIOB->MODER   |=  (2U<<(6*2) | 2U<<(7*2));
    GPIOB->AFR[0]  |=  (7U<<(6*4)) | (7U<<(7*4));
    GPIOB->OSPEEDR |=  (3U<<(6*2)) | (3U<<(7*2));
    GPIOB->PUPDR   |=  (1U<<(6*2)) | (1U<<(7*2));  /* pull-ups */
}
static void USART2_GPIO(void)            /* PA2 / PA3 */
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    GPIOA->MODER   &= ~(3U<<(2*2) | 3U<<(3*2));
    GPIOA->MODER   |=  (2U<<(2*2) | 2U<<(3*2));
    GPIOA->AFR[0]  |=  (7U<<(2*4)) | (7U<<(3*4));
    GPIOA->OSPEEDR |=  (3U<<(2*2)) | (3U<<(3*2));
    GPIOA->PUPDR   |=  (1U<<(2*2)) | (1U<<(3*2));
}

/* ---------- common USART init ---------- */
static void uart_basic_init(USART_TypeDef *u)
{
    u->CR1 = 0;                              /* disable */
    u->BRR = BAUD_BRR;
    u->CR3 |= USART_CR3_DMAT;                /* enable DMA TX */
    u->CR1 |= USART_CR1_TE | USART_CR1_RE |  /* TX + RX       */
              USART_CR1_RXNEIE | USART_CR1_TCIE |
              USART_CR1_UE;
}

/* ---------- public init wrappers ---------- */
void UART1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    USART1_GPIO();
    uart_basic_init(USART1);

    tx_chan = DMA1_Channel4; uart_tx = USART1;
    DMA_UART_TX_Init(tx_chan, USART1);

    NVIC_EnableIRQ(USART1_IRQn);
}
void UART2_Init(void)
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    USART2_GPIO();
    uart_basic_init(USART2);

    tx_chan = DMA1_Channel7; uart_tx = USART2;
    DMA_UART_TX_Init(tx_chan, USART2);

    NVIC_EnableIRQ(USART2_IRQn);
}

/* ---------- queue helper ---------- */
static int queue_empty(void){ return head == tail; }
static uint8_t* queue_front(void){ return (uint8_t*)pool[head]; }
static uint16_t queue_pop_len(void)
{
    uint16_t len = strlen((char*)pool[head]);
    head = (head + 1) % QUEUE_DEPTH;
    return len;
}
static void queue_push(char *s)
{
    uint16_t len = strlen(s);
    if (len >= IO_SIZE) len = IO_SIZE-1;

    uint8_t *slot = (uint8_t*)pool[tail];
    memcpy(slot, s, len);
    slot[len] = '\0';

    tail = (tail + 1) % QUEUE_DEPTH;         /* overwrite if full */
    if (tail == head) head = (head + 1) % QUEUE_DEPTH;
}

/* ---------- DMA fire-off ---------- */
static void kick_tx(void)
{
    if (tx_chan->CCR & DMA_CCR_EN) return;   /* busy */

    if (queue_empty()) return;
    active  = queue_front();
    act_len = queue_pop_len();

    tx_chan->CMAR  = (uint32_t)active;
    tx_chan->CNDTR = act_len;
    tx_chan->CCR  |= DMA_CCR_EN;
}

/* ---------- user API ---------- */
void UART_print(char *s)
{
    __disable_irq();
    queue_push(s);
    kick_tx();
    __enable_irq();
}

/* ---------- RX assembler ---------- */
static char line[IO_SIZE];
static uint16_t in_len = 0;

static void rx_byte(char c)
{
    /* Treat CR as an alias for LF so either terminator works                */
    if (c == '\r')
        c = '\n';

    /* If not newline, store the byte and return                             */
    if (c != '\n') {
        if (in_len < IO_SIZE - 1)
            line[in_len++] = c;
        return;
    }

    /* New-line received → terminate string, dispatch, reset                 */
    line[in_len] = '\0';                  /* NUL-terminate                  */
    UART_onInput(line, in_len);           /* user callback                  */
    in_len = 0;                           /* start fresh                    */
}

/* ---------- USART IRQ (1 & 2 share same body) ---------- */
static void usart_isr(USART_TypeDef *u)
{
    if (u->ISR & USART_ISR_RXNE)
        rx_byte((char)u->RDR);

    if (u->ISR & USART_ISR_TC) {             /* last char sent */
        u->ICR = USART_ICR_TCCF;
        tx_chan->CCR &= ~DMA_CCR_EN;         /* stop channel  */
        kick_tx();                           /* start next    */
    }
}
void USART1_IRQHandler(void){ usart_isr(USART1); }
void USART2_IRQHandler(void){ usart_isr(USART2); }
