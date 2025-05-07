#include "SPI.h"
#include "SysTimer.h"

void SPI1_GPIO_Init(void) {
	// Enable the GPIO Clock
	// Set PA4, PB3, PB4, and PB5 to Alternative Functions, and configure their AFR to SPI1
	// Set GPIO Pins to: Very High Output speed, Output Type Push-Pull, and No Pull-Up/Down
}


void SPI1_Init(void){
	// Enable SPI clock and Reset SPI
	// Disable SPI
	// Configure for Full Duplex Communication
	// Configure for 2-line Unidirectional Data Mode
	// Disable Output in Bidirectional Mode
	// Set Frame Format: MSB First, 16-bit, Motorola Mode
	// Configure Clock. Read DataSheet for required value
	// Set Baud Rate Prescaler to 16
	// Disable Hardware CRC Calculation
	// Set as Master
	// Disable Software Slave Management
	// Enable NSS Pulse Management
	// Enable Output
	// Set FIFO Reception Threshold to 1/2
	// Enable SPI
}

uint16_t SPI_Transfer_Data(uint16_t write_data){
	// Wait for TXE (Transmit buffer empty)
	// Write data
	// Wait for not busy
	// Wait for RXNE (Receive buffer not empty)
	// Read data
	return 0; // TODO
}
