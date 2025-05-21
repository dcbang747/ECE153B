/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6B
 */
 
#include "CRC.h"

/**
  * @brief  Computes the 32-bit CRC of a given buffer of data word(32-bit).
  * @param  pBuffer: pointer to the buffer containing the data to be computed
  * @param  BufferLength: length of the buffer to be computed					
  * @retval 32-bit CRC
  */
uint32_t CRC_CalcBlockCRC(const uint32_t * pBuffer, uint32_t BufferLength) {
  uint32_t index = 0;
  // write all data into data register
	for(int i=0;i<BufferLength;i++){
		CRC->DR = pBuffer[i];
	}
  return CRC->DR;  // read CRC from data register
}	

void CRC_Init(void) {
    RCC->AHB1ENR  |= RCC_AHB1ENR_CRCEN;            /* enable peripheral clock */
    CRC->CR        = CRC_CR_RESET;                 /* soft reset              */
    CRC->POL       = 0x104C11DB7;                     /* generator polynomial    */
    CRC->INIT      = CRC_INIT_INIT;                 /* starting value          */
    CRC->CR       &= ~CRC_CR_POLYSIZE;             /* 00 → 32-bit polynomial  */
}

