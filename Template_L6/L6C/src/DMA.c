/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6C
 */
 
#include "DMA.h"
#include "CRC.h"

void DMA_Init(void)
{
    /* 1 – 2  enable DMA1 and give the hardware 20 µs to settle                */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    for (volatile uint32_t i = 0; i < 1600; ++i) __NOP();   /* ≈20 µs @80 MHz   */

    /* 3 – 11  configure Channel 6 exactly as required                         */
    DMA1_Channel6->CCR &= ~DMA_CCR_EN;                      /* channel off     */
    DMA1_Channel6->CCR  = 0;                                /* clear all bits  */
    DMA1_Channel6->CCR |= DMA_CCR_MEM2MEM;                  /* mem-to-mem mode */
    DMA1_Channel6->CCR |= DMA_CCR_PL_1;                     /* high priority   */
    DMA1_Channel6->CCR |= DMA_CCR_PSIZE_1;                  /* 32-bit periph   */
    DMA1_Channel6->CCR |= DMA_CCR_MSIZE_1;                  /* 32-bit memory   */
    DMA1_Channel6->CCR &= ~DMA_CCR_PINC;                    /* no PINC         */
    DMA1_Channel6->CCR |= DMA_CCR_MINC;                     /* memory INC      */
    DMA1_Channel6->CCR &= ~DMA_CCR_CIRC;                    /* no circular     */
    DMA1_Channel6->CCR |= DMA_CCR_DIR;                      /* mem→periph dir  */

    /* 12 – 13  set addresses                                                  */
    DMA1_Channel6->CMAR = (uint32_t)DataBuffer;             /* source          */
    DMA1_Channel6->CPAR = (uint32_t)&CRC->DR;               /* destination     */

    /* 14 – 18  interrupt setup                                                */
    DMA1_Channel6->CCR &= ~(DMA_CCR_HTIE | DMA_CCR_TEIE);   /* HT, TE off      */
    DMA1_Channel6->CCR |=  DMA_CCR_TCIE;                    /* TC on           */
    NVIC_SetPriority(DMA1_Channel6_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

void DMA1_Channel6_IRQHandler(void)
{
    NVIC_ClearPendingIRQ(DMA1_Channel6_IRQn);

    if (DMA1->ISR & DMA_ISR_TCIF6)                          /* xfer complete   */
    {
        DMA1->IFCR = DMA_IFCR_CTCIF6;                       /* clear TC flag   */
        completeCRC(CRC->DR);                               /* hand result up  */
    }

    DMA1->IFCR = DMA_IFCR_CGIF6;                            /* clear global    */
}
