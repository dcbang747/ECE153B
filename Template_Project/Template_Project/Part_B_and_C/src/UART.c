 #include "UART.h"
 #include "stm32l476xx.h"
 
 #define SYSCLK_HZ   80000000UL   // System‑core frequency after SysClock.c config
 #define BAUDRATE    9600UL
 
 /* ————————————————————————————————————————  Private helpers ———————————————————————————————————————— */
 static uint32_t computeBRR(uint32_t periph_clk, uint32_t baud)
 {
	 /* For oversampling‑by‑16:
		  USARTDIV = fCK / baud
		BRR[15:4] = USARTDIV mantissa, BRR[3:0] = fraction
		Here we round USARTDIV to the nearest integer and let hardware
		split mantissa/fraction automatically.                                   */
	 return (periph_clk + baud / 2U) / baud;
 }
 
 /* ————————————————————————————————————————  RCC / clock ———————————————————————————————————————— */
 
 void UART1_Init(void)
 {
	 /* Enable peripheral clock on APB2                                           */
	 RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
 
	 /* Select SYSCLK (80 MHz) as USART1 clock source → CCIPR[3:2] = 0b11         */
	 RCC->CCIPR &= ~RCC_CCIPR_USART1SEL;
	 RCC->CCIPR |=  (3U << 2);
 }
 
 void UART2_Init(void)
 {
	 /* Enable peripheral clock on APB1                                           */
	 RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
 
	 /* Select SYSCLK (80 MHz) as USART2 clock source → CCIPR[5:4] = 0b11         */
	 RCC->CCIPR &= ~RCC_CCIPR_USART2SEL;
	 RCC->CCIPR |=  (3U << 4);
 }
 
 /* ————————————————————————————————————————  GPIO ————————————————————————————————————————— */
 
 void UART1_GPIO_Init(void)
 {
	 /* PB6 = TX, PB7 = RX  → alternate‑function 7                                */
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
 
	 /* MODER: alternate (10)                                                     */
	 GPIOB->MODER &= ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	 GPIOB->MODER |=  (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
 
	 /* OTYPER: push‑pull                                                         */
	 GPIOB->OTYPER &= ~(GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);
 
	 /* OSPEEDR: very‑high speed (11)                                             */
	 GPIOB->OSPEEDR |= (3U << (6 * 2)) | (3U << (7 * 2));
 
	 /* PUPDR: pull‑up (01)                                                       */
	 GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);
	 GPIOB->PUPDR |=  (GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0);
 
	 /* AFRL: AF7                                                                 */
	 GPIOB->AFR[0] &= ~((0xF << (6 * 4)) | (0xF << (7 * 4)));
	 GPIOB->AFR[0] |=  (7U << (6 * 4)) | (7U << (7 * 4));
 }
 
 void UART2_GPIO_Init(void)
 {
	 /* PA2 = TX, PA3 = RX  → alternate‑function 7                                */
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
 
	 GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	 GPIOA->MODER |=  (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);
 
	 GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
 
	 GPIOA->OSPEEDR |= (3U << (2 * 2)) | (3U << (3 * 2));
 
	 GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
	 GPIOA->PUPDR |=  (GPIO_PUPDR_PUPD2_0 | GPIO_PUPDR_PUPD3_0);
 
	 GPIOA->AFR[0] &= ~((0xF << (2 * 4)) | (0xF << (3 * 4)));
	 GPIOA->AFR[0] |=  (7U << (2 * 4)) | (7U << (3 * 4));
 }
 
void USART_Init(USART_TypeDef* USARTx) {
	// Disable USART before configuring settings
	USARTx->CR1 &= ~USART_CR1_UE;
	
	// Set Communication Parameters
	USARTx->CR1 &= ~(USART_CR1_M);     // 00 -> 8 Data Bits
	USARTx->CR1 &= ~(USART_CR1_OVER8); // 0 -> Oversampling by 16
	USARTx->CR2 &= ~(USART_CR2_STOP);  // 00 -> 1 Stop Bit
	
	// Set Baud Rate
	// f_CLK = 80 MHz, Baud Rate = 9600 = 80 MHz / DIV -> DIV = 8333 = 0x208D
	USARTx->BRR = 0x208D;
	
	// Enable Transmitter/Receiver
	USARTx->CR1 |= USART_CR1_TE | USART_CR1_RE;
	
	// Enable USART
	USARTx->CR1 |= USART_CR1_UE;
}

uint8_t USART_Read (USART_TypeDef * USARTx) {
	// SR_RXNE (Read data register not empty) bit is set by hardware
	while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
	// USART resets the RXNE flag automatically after reading DR
	return ((uint8_t)(USARTx->RDR & 0xFF));
	// Reading USART_DR automatically clears the RXNE flag 
}

void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	int i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR 
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));   	// wait until TXE (TX empty) bit is set
		// Writing USART_DR automatically clears the TXE flag 	
		USARTx->TDR = buffer[i] & 0xFF;
		USART_Delay(300);
	}
	while (!(USARTx->ISR & USART_ISR_TC));   		  // wait until TC bit is set
	USARTx->ISR &= ~USART_ISR_TC;
}   

void USART_Delay(uint32_t us) {
	uint32_t time = 100*us/7;    
	while(--time);   
}
