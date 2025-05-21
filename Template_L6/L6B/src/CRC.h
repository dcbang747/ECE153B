/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6B
 */

#ifndef __STM32L476G_DISCOVERY_CRC_H
#define __STM32L476G_DISCOVERY_CRC_H

#include "stm32l476xx.h"


#define BUFFER_SIZE    512

extern const uint32_t DataBuffer[BUFFER_SIZE];

void CRC_Init(void);


/**
  * @brief  Computes the 32-bit CRC of a given buffer of data word(32-bit).
  * @param  pBuffer: pointer to the buffer containing the data to be computed
  * @param  BufferLength: length of the buffer to be computed					
  * @retval 32-bit CRC
  */
uint32_t CRC_CalcBlockCRC(const uint32_t * pBuffer, uint32_t BufferLength);

#endif /* __STM32L476G_DISCOVERY_CRC_H */
