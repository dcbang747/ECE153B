/*
 * SPI1 – PA4 (NSS)  PB3 (SCK)  PB4 (MISO)  PB5 (MOSI)
 * Full‑duplex, 16‑bit frame, Mode‑3, master, prescaler /16 (≈5 MHz @80 MHz core)
 */

 #include "SPI.h"
 #include "stm32l476xx.h"
 #include "SysTimer.h"
 
 /* ——————————————————— GPIO for SPI1 ——————————————————— */
 void SPI1_GPIO_Init(void)
 {
	 /* Enable GPIOA & GPIOB clocks */
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;
 
	 /* ------------ PA4 (NSS) ------------- */
	 GPIOA->MODER   &= ~(3U << (4 * 2));
	 GPIOA->MODER   |=  (2U << (4 * 2));         /* alternate‑function      */
	 GPIOA->OTYPER  &= ~(1U << 4);               /* push‑pull               */
	 GPIOA->OSPEEDR |=  (3U << (4 * 2));         /* very‑high               */
	 GPIOA->PUPDR   &= ~(3U << (4 * 2));         /* no PU/PD                */
	 GPIOA->AFR[0]  &= ~(0xF << (4 * 4));
	 GPIOA->AFR[0]  |=  (5U << (4 * 4));         /* AF5 – SPI1              */
 
	 /* ------------ PB3 / PB4 / PB5 -------- */
	 for (uint8_t pin = 3; pin <= 5; ++pin) {
		 GPIOB->MODER   &= ~(3U << (pin * 2));
		 GPIOB->MODER   |=  (2U << (pin * 2));   /* alternate               */
		 GPIOB->OTYPER  &= ~(1U << pin);         /* push‑pull               */
		 GPIOB->OSPEEDR |=  (3U << (pin * 2));   /* very‑high               */
		 GPIOB->PUPDR   &= ~(3U << (pin * 2));   /* no PU/PD                */
		 GPIOB->AFR[0]  &= ~(0xF << (pin * 4));
		 GPIOB->AFR[0]  |=  (5U << (pin * 4));   /* AF5                     */
	 }
 }
 
 /* ——————————————————— SPI peripheral ——————————————————— */
 void SPI1_Init(void)
 {
	 /* Enable clock and reset */
	 RCC->APB2ENR  |= RCC_APB2ENR_SPI1EN;
	 RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
	 RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
 
	 /* Disable SPI before configuration */
	 SPI1->CR1 &= ~SPI_CR1_SPE;
 
	 /* CR1 ------------------------------------------------------------------ */
	 SPI1->CR1 = 0;
	 SPI1->CR1 |= SPI_CR1_MSTR;              /* master                        */
	 SPI1->CR1 |= SPI_CR1_BR_2;              /* BR = 16  (0b100)              */
	 SPI1->CR1 |= SPI_CR1_CPOL | SPI_CR1_CPHA;   /* Mode‑3                      */
	 /* MSB first, full duplex, 2‑line unidirectional => defaults already 0   */
 
	 /* CR2 ------------------------------------------------------------------ */
	 SPI1->CR2 = 0;
	 SPI1->CR2 |= (0xF << 8);                /* DS[3:0] = 0b1111 → 16‑bit     */
	 SPI1->CR2 |= SPI_CR2_NSSP;              /* NSS pulse generation          */
	 SPI1->CR2 |= SPI_CR2_SSOE;              /* NSS output enable             */
	 /* FRXTH = 0 -> 1/2 FIFO threshold (required for 16‑bit)                 */
 
	 /* Enable SPI */
	 SPI1->CR1 |= SPI_CR1_SPE;
 }
 
 /* ——————————————————— 16‑bit transfer helper ——————————————————— */
 uint16_t SPI_Transfer_Data(uint16_t write_data)
 {
	 /* Wait for TX buffer empty */
	 while (!(SPI1->SR & SPI_SR_TXE));
	 *(volatile uint16_t *)&SPI1->DR = write_data;
 
	 /* Wait until transfer finished */
	 while (SPI1->SR & SPI_SR_BSY);
	 while (!(SPI1->SR & SPI_SR_RXNE));
 
	 return (uint16_t)SPI1->DR;
 }
 