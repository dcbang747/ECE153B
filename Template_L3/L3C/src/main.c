/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 3C
 */

 #include <stdio.h>
 #include "stm32l476xx.h"
 
 /* ───────── globals used by capture ISR ───────── */
 volatile uint32_t currentValue = 0;
 volatile uint32_t lastValue    = 0;
 volatile uint32_t overflowCount = 0;
 volatile uint32_t timeInterval  = 0;          /* echo-high width, µs        */
 volatile uint8_t  edge          = 0;          /* 0 = waiting for rising     */
 
 /* ───────────────────────── PA9 → TIM1_CH2 : 10 µs trigger every 66 ms ── */
 void Trigger_Setup(void) {
	 /* PA9 alternate-function setup (AF1 = TIM1_CH2) ------------------- */
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;            /* GPIO-A clock         */
 
	 GPIOA->MODER   &= ~(3U << (9 * 2));
	 GPIOA->MODER   |=  (2U << (9 * 2));             /* AF mode             */
	 GPIOA->OSPEEDR |=  (3U << (9 * 2));             /* very-high speed     */
	 GPIOA->PUPDR   &= ~(3U << (9 * 2));             /* no PU/PD            */
	 GPIOA->AFR[1]  &= ~(0xF << ((9 - 8) * 4));
	 GPIOA->AFR[1]  |=  (0x1 << ((9 - 8) * 4));      /* AF1                 */
 
	 /* TIM1 configuration ---------------------------------------------- */
	 RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;             /* enable TIM1 clock   */
 
	 TIM1->PSC = 15;                                 /* 16 MHz/16 = 1 MHz   */
	 TIM1->ARR = 0xFFFF;                             /* ~65.5 ms period     */
	 TIM1->CCR2 = 10;                                /* 10 µs pulse width   */
 
	 /* PWM mode 1 on CH2, preload enable */
	 TIM1->CCMR1 &= ~(7U << 12);
	 TIM1->CCMR1 |=  (6U << 12) | TIM_CCMR1_OC2PE;
 
	 TIM1->CCER  |= TIM_CCER_CC2E;                   /* enable CH2 output   */
 
	 /* Main output enable + safe-state bits */
	 TIM1->BDTR  |= TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI;
 
	 TIM1->EGR   |= TIM_EGR_UG;                      /* update registers    */
	 TIM1->CR1   |= TIM_CR1_CEN;                     /* start counter       */
 }
 
 /* ───────────────────────── PB6 → TIM4_CH1 : echo capture ────────────── */
 void Input_Capture_Setup(void) {
	 /* PB6 alternate-function setup (AF2 = TIM4_CH1) ------------------- */
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;            /* GPIO-B clock        */
 
	 GPIOB->MODER   &= ~(3U << (6 * 2));
	 GPIOB->MODER   |=  (2U << (6 * 2));             /* AF mode             */
	 GPIOB->OSPEEDR |=  (3U << (6 * 2));             /* very-high speed     */
	 GPIOB->PUPDR   &= ~(3U << (6 * 2));             /* no PU/PD            */
	 GPIOB->AFR[0]  &= ~(0xF << (6 * 4));
	 GPIOB->AFR[0]  |=  (0x2 << (6 * 4));            /* AF2                 */
 
	 /* TIM4 configuration ---------------------------------------------- */
	 RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;           /* enable TIM4 clock   */
 
	 TIM4->PSC = 15;                                 /* 1 MHz = 1 µs/tick   */
	 TIM4->ARR = 0xFFFF;                             /* free-run 16-bit     */
	 TIM4->CR1 |= TIM_CR1_ARPE;                      /* ARR preload         */
 
	 /* Capture input on TI1 */
	 TIM4->CCMR1 &= ~TIM_CCMR1_CC1S;
	 TIM4->CCMR1 |=  TIM_CCMR1_CC1S_0;               /* CC1 mapped to TI1   */
 
	 /* Capture on both edges */
	 TIM4->CCER |=  TIM_CCER_CC1P | TIM_CCER_CC1NP;  /* rising & falling    */
	 TIM4->CCER |=  TIM_CCER_CC1E;                   /* enable capture      */
 
	 /* Interrupts: capture + overflow */
	 TIM4->DIER |= TIM_DIER_CC1IE | TIM_DIER_UIE;
 
	 TIM4->EGR  |= TIM_EGR_UG;                       /* update registers    */
	 TIM4->CR1  |= TIM_CR1_CEN;                      /* start counter       */
 
	 NVIC_SetPriority(TIM4_IRQn, 2);
	 NVIC_EnableIRQ(TIM4_IRQn);
 }
 
 /* ───────────────────────── TIM4 interrupt service routine ───────────── */
 void TIM4_IRQHandler(void) {
	 /* ------ counter overflow (UIF) ---------------------------------- */
	 if (TIM4->SR & TIM_SR_UIF) {
		 TIM4->SR &= ~TIM_SR_UIF;                    /* clear flag          */
		 ++overflowCount;
	 }
 
	 /* ------ capture event (CC1IF) ----------------------------------- */
	 if (TIM4->SR & TIM_SR_CC1IF) {
		 currentValue = TIM4->CCR1;                  /* reading clears flag */
 
		 if (edge == 0) {            /* rising edge → start of pulse     */
			 lastValue      = currentValue;
			 overflowCount  = 0;
			 edge           = 1;
		 } else {                   /* falling edge → end of pulse       */
			 /* total ticks = Δ + (overflows×65536) */
			 timeInterval = (overflowCount << 16) + (currentValue - lastValue);
			 edge          = 0;                       /* wait for next rise  */
		 }
	 }
 }
 
 /* ───────────────────────── system clock: 16 MHz HSI ─────────────────── */
 static void SystemClock_HSI16(void) {
	 RCC->CR   |= RCC_CR_HSION;
	 while ((RCC->CR & RCC_CR_HSIRDY) == 0) { }
	 RCC->CFGR &= ~RCC_CFGR_SW;
	 RCC->CFGR |=  RCC_CFGR_SW_HSI;
	 while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) { }
 }
 
 /* ────────────────────────────────── main ─────────────────────────────── */
 int main(void) {
	 SystemClock_HSI16();   /* 16 MHz core / 1 µs timer ticks */
 
	 Input_Capture_Setup();
	 Trigger_Setup();
 
	 while (1) {
		 /* convert pulse width to distance (cm) : d = t / 58 */
		 uint32_t distanceCM =
			 (timeInterval == 0 || timeInterval > 38000) ? 0 : (timeInterval / 58);
 
		 /* ─── place a breakpoint or watch ‘distanceCM’ to observe value ─── */
		 (void)distanceCM;       /* keeps the variable on the stack        */
	 }
 }
 