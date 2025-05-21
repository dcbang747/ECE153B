/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6A
 */

#ifndef __STM32L476G_DISCOVERY_CRC_H
#define __STM32L476G_DISCOVERY_CRC_H

#include "stm32l476xx.h"


#define BUFFER_SIZE   512 /* data buffer size */
#define LOWER 				0 /* lower limit */
#define UPPER 				BUFFER_SIZE /* upper limit */
#define STEP 					1 /* step size */
#define CRC_SHIFT 		1

#define POLYSIZE 			CRC_PolSize_32
#define POLYNOME 			0x04C11DB7
#define INITIAL_CRC_VALUE 0xFFFFFFFF

extern const uint32_t DataBuffer[BUFFER_SIZE];

uint32_t CrcSoftwareFunc(uint32_t Crc, uint32_t Data, uint32_t POLY);

#endif /* __STM32L476G_DISCOVERY_CRC_H */
