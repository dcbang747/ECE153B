#include "SysClock.h"

void System_Clock_Init(void) {
    /* Enable MSI */
    RCC->CR |= RCC_CR_MSION;
    while (!(RCC->CR & RCC_CR_MSIRDY));

    /* Select 8 MHz range (Range 6) and make MSIRANGE effective */
    RCC->CR &= ~RCC_CR_MSIRANGE;
    RCC->CR |=  RCC_CR_MSIRANGE_4;    /* 1 MHz */
    RCC->CR |=  RCC_CR_MSIRGSEL;

    /* Switch SYSCLK to MSI */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |=  RCC_CFGR_SW_MSI;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI);
}