#include "UART.h"

#define SYSCLK_HZ 80000000UL
#define BAUDRATE  9600UL

uint32_t SystemCoreClock = 80000000;

static uint32_t brr_val(uint32_t pclk, uint32_t baud)
{
    return (pclk + baud/2U) / baud;
}

/* GPIO: PA2 TX, PA3 RX (AF7) */
void UART2_GPIO_Init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->MODER  &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
    GPIOA->MODER  |=  (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
    GPIOA->OSPEEDR|=  (3U << (2*2)) | (3U << (3*2));
    GPIOA->PUPDR  &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);

    GPIOA->AFR[0] &= ~((0xF << 8) | (0xF << 12));
    GPIOA->AFR[0] |=  (7U << 8) | (7U << 12);
}

void UART2_Init(void)
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    RCC->CCIPR &= ~(3U << 4);          /* SYSCLK source */
    RCC->CCIPR |=  (3U << 4);

    USART2->CR1 &= ~USART_CR1_UE;      /* disable       */
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE;
    USART2->CR2 = 0;                   /* 1 stop        */
    USART2->BRR = brr_val(SYSCLK_HZ, BAUDRATE);
    USART2->CR1 |= USART_CR1_UE;       /* enable        */
}


void USART_Init(USART_TypeDef *USARTx)
{
    /* 1. Disable while configuring */
    USARTx->CR1 &= ~USART_CR1_UE;

    /* 2. Word length = 8 bits, no parity, oversample‑by‑16 */
    USARTx->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1 |
                     USART_CR1_PCE | USART_CR1_OVER8);

    /* 3. 1 stop bit */
    USARTx->CR2 &= ~USART_CR2_STOP;

    /* 4. Baud rate = 9600 (core clock = 80 MHz) */
    uint32_t brr = (SystemCoreClock + 9600/2U) / 9600U;
    USARTx->BRR = brr;

    /* 5. Enable transmitter & receiver */
    USARTx->CR1 |= USART_CR1_TE | USART_CR1_RE;

    /* 6. Enable peripheral */
    USARTx->CR1 |= USART_CR1_UE;

    /* Wait for acknowledge flags (optional) */
    while (!(USARTx->ISR & USART_ISR_TEACK));
    while (!(USARTx->ISR & USART_ISR_REACK));
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
