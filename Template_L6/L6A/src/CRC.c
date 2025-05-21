/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 6A
 */
 
#include "CRC.h"

/**
  * @brief  Algorithm implementation of the CRC
  * @param  Crc: specifies the previous Crc data
  * @param  Data: specifies the input data
  * @retval Crc: the CRC result of the input data
  */


#include "CRC.h"

uint32_t CrcSoftwareFunc(uint32_t Crc, uint32_t Data, uint32_t POLY)
{
    Crc ^= Data;                                         /* initial XOR     */
    for (uint8_t i = 0; i < 32; ++i)
        Crc = (Crc & 0x80000000UL) ? (Crc << 1) ^ POLY : (Crc << 1);
    return Crc;
}
