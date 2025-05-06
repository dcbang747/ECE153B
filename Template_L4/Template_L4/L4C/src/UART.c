#include "UART.h"
#include "stm32l476xx.h"

#define SYSCLK_HZ 80000000UL
#define BAUDRATE  9600UL

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
    /* already fully initialised for USART2; nothing extra needed here */
}
