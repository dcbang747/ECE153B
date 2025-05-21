/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6C
 */
 
#include "DMA.h"
#include "CRC.h"

void DMA_Init(void) {
	//TODO
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;		//enable dma clock
	delay(20);
	
	DMA1_Channel6->CCR &= ~DMA_CCR_EN;			//disable channel 6 
	DMA1_Channel6->CCR |=DMA_CCR_MEM2MEM;		//set to memory to memeory mode
	DMA1_Channel6->CCR &= ~DMA_CCR_PL;
	DMA1_Channel6->CCR |= DMA_CCR_PL_1;			//chanel priopority to high
	DMA1_Channel6->CCR |= DMA_CCR_PSIZE_1;  //set chanel to 32 bit
	DMA1_Channel6->CCR &= ~DMA_CCR_PSIZE_0;  // ditto
	
	DMA1_Channel6->CCR &= ~DMA_CCR_MSIZE;			//set memory to size 32
	DMA1_Channel6->CCR |= DMA_CCR_MSIZE_1;
	DMA1_Channel6->CCR &= ~DMA_CCR_PINC;			//disable perphial increment
	DMA1_Channel6->CCR &= ~DMA_CCR_MINC;			//diable memory increment
	
	DMA1_Channel6->CCR &= ~DMA_CCR_CIRC; 			//disable circular mode
	DMA1_Channel6->CCR |= DMA_CCR_DIR;				//read from memory, data direction is memory to periphal
	
	DMA1_Channel6->CMAR = (uint32_t)DataBuffer
	
	//todo, set data source to data buffer/ set data destination to data register of CRC block
	
	DMA1->ISR &= ~DMA_ISR_HTIF6;							//disable channel 6 half transfer interrupt
	DMA1->ISR &= ~DMA_ISR_TCIF6;							//
	
}

void DMA1_Channel6_IRQHandler(void){ 
	// TODO
}

