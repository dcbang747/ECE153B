#include "SPI.h"
#include "SysTimer.h"
#include "accelerometer.h"

void accWrite(uint8_t addr, uint8_t val){
	// TODO access SPI_Transfer_Data
}

uint8_t accRead(uint8_t addr){
	// access SPI_Transfer_Data
	return 0; // TODO
}

void initAcc(void){
	// set full range mode
	// enable measurement
}

void readValues(double* x, double* y, double* z){
	// TODO
	// find scaler from data sheet
	// read values into x,y,z using accRead
}
