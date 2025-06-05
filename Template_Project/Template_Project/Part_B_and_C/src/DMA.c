/*
 * DMA-to-UART TX  –  STM32L476RG
 *  USART1_TX → DMA1 Ch-4, DMAMUX req = 1
 *  USART2_TX → DMA1 Ch-7, DMAMUX req = 2
 */
#include "DMA.h"

static inline uint32_t chan_idx(DMA_Channel_TypeDef *ch)
{ return ((uint32_t)ch - (uint32_t)DMA1_Channel1) / 0x14U; }   /* 0…7 */

void DMA_UART_TX_Init(DMA_Channel_TypeDef *ch, USART_TypeDef *uart)
{
    /* enable DMA1 clk */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    /* ---- permanent channel config ---- */
    ch->CCR  = 0;                               /* disable + clear          */
    ch->CCR |= DMA_CCR_DIR    |                 /* mem → periph             */
               DMA_CCR_MINC   |                 /* mem inc                  */
               DMA_CCR_TCIE   |                 /* TC interrupt             */
               DMA_CCR_PL_1;                    /* high priority            */

    /* 8-bit sizes */
    /* PSIZE + MSIZE already 00 after reset      */

    /* peripheral addr = &USARTx->TDR */
    ch->CPAR = (uint32_t)&uart->TDR;

    /* ---- DMAMUX request number ---- */
    uint32_t req = (uart == USART1) ? 1 : 2;    /* see RM0394 Table 39 */
    uint32_t idx = chan_idx(ch);
    DMA1_CSELR->CSELR &= ~(0xFU << (idx * 4));
    DMA1_CSELR->CSELR |=  (req  << (idx * 4));

    /* enable NVIC line */
    if (ch == DMA1_Channel4) NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    else                      NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

/* ---------- shared ISR body ---------- */
static void dma_tx_isr(DMA_Channel_TypeDef *ch, uint32_t ifcr_mask)
{
    if (DMA1->ISR & (ifcr_mask << 1)) {         /* TCIFx */
        DMA1->IFCR = (ifcr_mask << 1);          /* clear TC flag */
    }
    ch->CCR &= ~DMA_CCR_EN;                     /* ensure channel off */
}

/* USART1_TX → Ch-4 */
void DMA1_Channel4_IRQHandler(void)
{ dma_tx_isr(DMA1_Channel4, DMA_IFCR_CGIF4); }

/* USART2_TX → Ch-7 */
void DMA1_Channel7_IRQHandler(void)
{ dma_tx_isr(DMA1_Channel7, DMA_IFCR_CGIF7); }
