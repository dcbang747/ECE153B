/*
 * ECE 153B – DMA initialisation for UART-TX
 *
 *  ‣ USART2_TX  → DMA1 Channel-7
 *  ‣ USART1_TX  → DMA1 Channel-4  (not used by default but ready)
 *
 * Only memory-to-peripheral transfers are needed, 8-bit, MINC, TC interrupt.
 */
#include "DMA.h"

static inline uint32_t chan_index(DMA_Channel_TypeDef *ch)
{
    return ((uint32_t)ch - (uint32_t)DMA1_Channel1) / 0x14U;   /* 0-7 */
}

void DMA_Init_UARTx(DMA_Channel_TypeDef *ch, USART_TypeDef *uart)
{
    /* clocks */
    RCC->AHB1ENR  |= RCC_AHB1ENR_DMA1EN;

    /* 1. Disable channel */
    ch->CCR &= ~DMA_CCR_EN;

    /* 2. Request mapping (DMAMUX on L4 via CSELR register) */
    uint32_t req;
    if      (uart == USART2) req = 4;          /* USART2_TX */
    else if (uart == USART1) req = 2;          /* USART1_TX */
    else                     return;           /* unsupported */

    uint32_t idx = chan_index(ch);
    DMA1_CSELR->CSELR &= ~(0xFU << (idx*4));
    DMA1_CSELR->CSELR |=  (req  << (idx*4));

    /* 3. Static part of CCR: mem→periph, 8-bit, MINC, TCIE                 */
    ch->CCR  = 0;
    ch->CCR |= DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_TCIE;

    /* 4. Peripheral address constant                                       */
    ch->CPAR = (uint32_t)&uart->TDR;

    /* 5. Enable NVIC line                                                  */
    if      (ch == DMA1_Channel7) NVIC_EnableIRQ(DMA1_Channel7_IRQn);
    else if (ch == DMA1_Channel4) NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

/* ---------- channel-7 (USART2_TX) ---------- */
void DMA1_Channel7_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF7) {
        DMA1->IFCR = DMA_IFCR_CTCIF7;          /* clear flag          */
        extern void on_complete_transfer(void);
        on_complete_transfer();
    }
}

/* (optional) channel-4 (USART1_TX) */
void DMA1_Channel4_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF4) {
        DMA1->IFCR = DMA_IFCR_CTCIF4;
        extern void on_complete_transfer(void);
        on_complete_transfer();
    }
}
