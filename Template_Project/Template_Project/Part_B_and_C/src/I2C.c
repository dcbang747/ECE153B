#include "I2C.h"
extern void Error_Handler(void);

// Inter-integrated Circuit Interface (I2C)
// up to 100 Kbit/s in the standard mode, 
// up to 400 Kbit/s in the fast mode, and 
// up to 3.4 Mbit/s in the high-speed mode.

// Recommended external pull-up resistance is 
// 4.7 kOmh for low speed, 
// 3.0 kOmh for the standard mode, and 
// 1.0 kOmh for the fast mode
	
//===============================================================================
//                        I2C GPIO Initialization
//===============================================================================
void I2C_GPIO_Init(void)
	{
		/* Enable GPIOB clock */
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

		/* PB8 & PB9 → Alternate‑function mode */
		GPIOB->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
		GPIOB->MODER |=  (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1);

		/* Open‑drain */
		GPIOB->OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9;

		/* Very‑high speed */
		GPIOB->OSPEEDR |= (3U << (8 * 2)) | (3U << (9 * 2));

		/* Pull‑ups */
		GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);
		GPIOB->PUPDR |=  (GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0);

		/* AF4 for I²C1 (pins 8 and 9 are in AFR[1]) */
		GPIOB->AFR[1] &= ~((0xF << 0) | (0xF << 4));
		GPIOB->AFR[1] |=  (4U << 0) | (4U << 4);
	}
	
#define I2C_TIMINGR_PRESC_POS	28
#define I2C_TIMINGR_SCLDEL_POS	20
#define I2C_TIMINGR_SDADEL_POS	16
#define I2C_TIMINGR_SCLH_POS	8
#define I2C_TIMINGR_SCLL_POS	0

#define PRESC   7U     // Prescaler = 7 ? 10 MHz I2C clock
#define SCLDEL  9U     // 1000 ns (tSCLDEL = (9+1)*100ns = 1000ns)
#define SDADEL  12U    // 1250 ns (tSDADEL = (12+1)*100ns = 1300ns > 1250ns)
#define SCLH    39U    // 4.0 �s (40 * 100ns)
#define SCLL    46U    // 4.7 �s (47 * 100ns)

//===============================================================================
//                          I2C Initialization
//===============================================================================
void I2C_Initialization(void)
{
    uint32_t OwnAddr = 0x52;          /* arbitrary 7‑bit node address */

    /* 1.  Clocking */
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;      /* enable peripheral clk     */

    /* Select SYSCLK (80 MHz) for I2C1SEL bits [13:12] = 01                   */
    RCC->CCIPR &= ~(3U << 12);
    RCC->CCIPR |=  (1U << 12);

    /* Reset then release I²C1                                                */
    RCC->APB1RSTR1 |=  RCC_APB1RSTR1_I2C1RST;
    RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C1RST;

    /* 2.  Configure registers (while PE = 0)                                */
    I2C1->CR1 &= ~I2C_CR1_PE;                         /* disable peripheral  */

    /* Enable analog filter (ANFOFF=0), disable digital filter (DNF=0),
       enable error interrupts and clock stretching, 7‑bit addr mode.        */
    I2C1->CR1 |= I2C_CR1_ERRIE;                       /* error IRQ enable    */
    I2C1->CR1 &= ~I2C_CR1_ANFOFF;                     /* analog filter on    */
    I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;                  /* clock stretching    */
    I2C1->CR1 &= ~I2C_CR1_DNF;                        /* digital filter 0    */

    /* Automatic STOP and NACK generation (helps simple master‑Rx)           */
    I2C1->CR2 |= I2C_CR2_AUTOEND;
    I2C1->CR2 |= I2C_CR2_NACK;

    /* 3.  Timing register (meets TC74 spec at ~100 kHz)                     */
    I2C1->TIMINGR =
          (PRESC  << I2C_TIMINGR_PRESC_POS) |
          (SCLDEL << I2C_TIMINGR_SCLDEL_POS) |
          (SDADEL << I2C_TIMINGR_SDADEL_POS) |
          (SCLH   << I2C_TIMINGR_SCLH_POS) |
          (SCLL   << I2C_TIMINGR_SCLL_POS);

    /* 4.  Own‑address 1                                                     */
    I2C1->OAR1 &= ~I2C_OAR1_OA1EN;                    /* disable first       */
    I2C1->OAR1 &= ~I2C_OAR1_OA1MODE;                  /* 7‑bit mode          */
    I2C1->OAR1  = (I2C1->OAR1 & ~0x7F) | (OwnAddr & 0x7F) << 1;
    I2C1->OAR1 |= I2C_OAR1_OA1EN;                     /* enable              */

    /* 5.  Enable I²C peripheral                                             */
    I2C1->CR1 |= I2C_CR1_PE;
}
//===============================================================================
//                           I2C Start
// Master generates START condition:
//    -- Slave address: 7 bits
//    -- Automatically generate a STOP condition after all bytes have been transmitted 
// Direction = 0: Master requests a write transfer
// Direction = 1: Master requests a read transfer
//=============================================================================== 
int8_t I2C_Start(I2C_TypeDef * I2Cx, uint32_t DevAddress, uint8_t Size, uint8_t Direction) {	
	
	// Direction = 0: Master requests a write transfer
	// Direction = 1: Master requests a read transfer
	
	uint32_t tmpreg = 0;
	
	// This bit is set by software, and cleared by hardware after the Start followed by the address
	// sequence is sent, by an arbitration loss, by a timeout error detection, or when PE = 0.
	tmpreg = I2Cx->CR2;
	
	tmpreg &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));
	
	if (Direction == READ_FROM_SLAVE)
		tmpreg |= I2C_CR2_RD_WRN;  // Read from Slave
	else
		tmpreg &= ~I2C_CR2_RD_WRN; // Write to Slave
		
	tmpreg |= (uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) | (((uint32_t)Size << 16 ) & I2C_CR2_NBYTES));
	
	tmpreg |= I2C_CR2_START;
	
	I2Cx->CR2 = tmpreg; 
	
   	return 0;  // Success
}

//===============================================================================
//                           I2C Stop
//=============================================================================== 
void I2C_Stop(I2C_TypeDef * I2Cx){
	// Master: Generate STOP bit after the current byte has been transferred 
	I2Cx->CR2 |= I2C_CR2_STOP;								
	// Wait until STOPF flag is reset
	while( (I2Cx->ISR & I2C_ISR_STOPF) == 0 ); 
}

//===============================================================================
//                           Wait for the bus is ready
//=============================================================================== 
void I2C_WaitLineIdle(I2C_TypeDef * I2Cx){
	// Wait until I2C bus is ready
	while( (I2Cx->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY );	// If busy, wait
}

//===============================================================================
//                           I2C Send Data
//=============================================================================== 
int8_t I2C_SendData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if (Size <= 0 || pData == NULL) return -1;
	
	I2C_WaitLineIdle(I2Cx);
	
	if (I2C_Start(I2Cx, DeviceAddress, Size, WRITE_TO_SLAVE) < 0 ) return -1;

	// Send Data
	// Write the first data in DR register
	// while((I2Cx->ISR & I2C_ISR_TXE) == 0);
	// I2Cx->TXDR = pData[0] & I2C_TXDR_TXDATA;  

	for (i = 0; i < Size; i++) {
		// TXE is set by hardware when the I2C_TXDR register is empty. It is cleared when the next
		// data to be sent is written in the I2C_TXDR register.
		// while( (I2Cx->ISR & I2C_ISR_TXE) == 0 ); 

		// TXIS bit is set by hardware when the I2C_TXDR register is empty and the data to be
		// transmitted must be written in the I2C_TXDR register. It is cleared when the next data to be
		// sent is written in the I2C_TXDR register.
		// The TXIS flag is not set when a NACK is received.
		while((I2Cx->ISR & I2C_ISR_TXIS) == 0 );
		I2Cx->TXDR = pData[i] & I2C_TXDR_TXDATA;  // TXE is cleared by writing to the TXDR register.
	}
	
	// Wait until TC flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0 && (I2Cx->ISR & I2C_ISR_NACKF) == 0);
	
	if( (I2Cx->ISR & I2C_ISR_NACKF) != 0 ) return -1;

	I2C_Stop(I2Cx);
	return 0;
}


//===============================================================================
//                           I2C Receive Data
//=============================================================================== 
int8_t I2C_ReceiveData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if(Size <= 0 || pData == NULL) return -1;

	I2C_WaitLineIdle(I2Cx);

	I2C_Start(I2Cx, DeviceAddress, Size, READ_FROM_SLAVE); // 0 = sending data to the slave, 1 = receiving data from the slave
						  	
	for (i = 0; i < Size; i++) {
		// Wait until RXNE flag is set 	
		while( (I2Cx->ISR & I2C_ISR_RXNE) == 0 );
		pData[i] = I2Cx->RXDR & I2C_RXDR_RXDATA;
	}
	
	// Wait until TCR flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0);
	
	I2C_Stop(I2Cx);
	
	return 0;
}
