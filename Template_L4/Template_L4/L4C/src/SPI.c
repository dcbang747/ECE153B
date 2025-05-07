/*
 * SPI1 (PA4 NSS, PB3 SCK, PB4 MISO, PB5 MOSI) – master, 16‑bit, mode‑3
 */

 #include "SPI.h"
 #include "SysTimer.h"
 #include "stm32l476xx.h"
 
 /* ─────────────────────────────  GPIO  ───────────────────────────── */
 void SPI1_GPIO_Init(void)
 {
	 /* Clocks */
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;
 
	 /* ---------- PA4 : NSS ---------------------------------------- */
	 GPIOA->MODER   &= ~(3U << (4 * 2));
	 GPIOA->MODER   |=  (2U << (4 * 2));          /* AF mode          */
	 GPIOA->AFR[0]  &= ~(0xF << (4 * 4));
	 GPIOA->AFR[0]  |=  (5U << (4 * 4));          /* AF5 = SPI1       */
 
	 /* ---------- PB3/4/5 : SCK/MISO/MOSI -------------------------- */
	 GPIOB->MODER   &= ~((3U << (3 * 2)) |
						 (3U << (4 * 2)) |
						 (3U << (5 * 2)));
	 GPIOB->MODER   |=  (2U << (3 * 2)) |          /* AF mode          */
						(2U << (4 * 2)) |
						(2U << (5 * 2));
 
	 GPIOB->AFR[0]  &= ~((0xF << (3 * 4)) |
						 (0xF << (4 * 4)) |
						 (0xF << (5 * 4)));
	 GPIOB->AFR[0]  |=  (5U << (3 * 4)) |
						(5U << (4 * 4)) |
						(5U << (5 * 4));
 
	 /* Very‑high speed, push‑pull, no pull‑ups */
	 GPIOA->OSPEEDR |= 3U << (4 * 2);
	 GPIOB->OSPEEDR |= 3U << (3 * 2) | 3U << (4 * 2) | 3U << (5 * 2);
 }
 
 /* ─────────────────────────────  SPI  ────────────────────────────── */
 void SPI1_Init(void)
 {
	 /* Clock & reset */
	 RCC->APB2ENR  |=  RCC_APB2ENR_SPI1EN;
	 RCC->APB2RSTR |=  RCC_APB2RSTR_SPI1RST;
	 RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
 
	 SPI1->CR1 &= ~SPI_CR1_SPE;                       /* disable first      */
 
	 /* CR1 – baud, polarity, phase, master, prescaler ÷16 (80 MHz→5 MHz)  */
	 SPI1->CR1 = 0;
	 SPI1->CR1 |= SPI_CR1_MSTR |
				  SPI_CR1_CPOL | SPI_CR1_CPHA |       /* mode‑3             */
				  SPI_CR1_BR_2;                       /* BR=100b → ÷16      */
 
	 /* CR2 – 16‑bit frame, hardware NSS, FRXTH=0 (½), NSS pulse + SSOE    */
	 SPI1->CR2 = 0;
	 SPI1->CR2 |= (15U << SPI_CR2_DS_Pos) |           /* 16‑bit data        */
				  SPI_CR2_NSSP |                      /* NSS pulse          */
				  SPI_CR2_SSOE;                       /* NSS output enable  */
 
	 /* Enable */
	 SPI1->CR1 |= SPI_CR1_SPE;
 }
 
 uint16_t SPI_Transfer_Data(uint16_t w)
 {
	 while (!(SPI1->SR & SPI_SR_TXE));        /* TX buffer empty        */
	 *((__IO uint16_t *)&SPI1->DR) = w;       /* 16‑bit write           */
	 while (SPI1->SR &  SPI_SR_BSY);          /* wait not busy          */
	 while (!(SPI1->SR & SPI_SR_RXNE));       /* RX buffer full         */
	 return *((__IO uint16_t *)&SPI1->DR);    /* 16‑bit read            */
 }
 