/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 3B
 */

 #include "stm32l476xx.h"

 void LED_Pin_Init(void);
 void TIM2_CH1_Init(void);
 void SERVO_Pin_Init(void);
 void TIM5_CH1_Init(void);
 
 /*  ----------------------------------------------------
	 GPIO - PA5  *  TIM2_CH1  *  LED
	 ---------------------------------------------------- */
 void LED_Pin_Init(void) {
 
	 /* 1. Enable GPIO-A clock */
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
 
	 /* 2. PA5  -->  Alternate-function mode (10) */
	 GPIOA->MODER &= ~GPIO_MODER_MODE5;          /* clear bits          */
	 GPIOA->MODER |=  GPIO_MODER_MODE5_1;        /* AF mode             */
 
	 /* 3. Very-high output speed (11) */
	 GPIOA->OSPEEDR |= (0x3UL << (5 * 2));
 
	 /* 4. No pull-up / pull-down (00) */
	 GPIOA->PUPDR  &= ~GPIO_PUPDR_PUPD5;
 
	 /* 5. Select AF1  (TIM2_CH1)  -->  AFRL[23:20] */
	 GPIOA->AFR[0] &= ~(0xFUL << (5 * 4));
	 GPIOA->AFR[0] |=  (0x1UL << (5 * 4));       /* AF1 */
 }
 
 /*  ----------------------------------------------------
	 Timer-2 channel-1  *  1kHz PWM for LED
	 ---------------------------------------------------- */
 void TIM2_CH1_Init(void) {
 
	 /* 1. Enable TIM2 clock */
	 RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
 
	 /* 2. Up-counting, CK_CNT = 4MHz / (PSC+1) = 1MHz          */
	 TIM2->PSC = 3;            /* 4MHz / 4 = 1MHz               */
	 TIM2->ARR = 999;          /* 1000ticks --> 1kHz PWM period   */
 
	 /* 3. PWM mode-1 on OC1 with preload */
	 TIM2->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
	 TIM2->CCMR1 |=  (6 << TIM_CCMR1_OC1M_Pos); /* 110: PWM1          */
	 TIM2->CCMR1 |=  TIM_CCMR1_OC1PE;           /* preload enable     */
 
	 /* 4. Output active-high & enable */
	 TIM2->CCER  &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
	 TIM2->CCER  |=  TIM_CCER_CC1E;
 
	 /* 5. Enable ARR buffer & force update so registers latch */
	 TIM2->CR1  |= TIM_CR1_ARPE;
	 TIM2->EGR  |= TIM_EGR_UG;
 
	 /* 6. Start timer */
	 TIM2->CR1  |= TIM_CR1_CEN;
 }
 
 /*  ----------------------------------------------------
	 GPIO - PA0  *  TIM5_CH1  *  Servo (50Hz)
	 ---------------------------------------------------- */
 void SERVO_Pin_Init(void) {
 
	 /* 1. Enable GPIO-A clock (already enabled, but harmless) */
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
 
	 /* 2. PA0 --> Alternate-function mode */
	 GPIOA->MODER &= ~GPIO_MODER_MODE0;
	 GPIOA->MODER |=  GPIO_MODER_MODE0_1;
 
	 /* 3. Very-high speed */
	 GPIOA->OSPEEDR |= (0x3UL << (0 * 2));
 
	 /* 4. No pull-up / pull-down */
	 GPIOA->PUPDR  &= ~GPIO_PUPDR_PUPD0;
 
	 /* 5. AF2 = TIM5_CH1 (AFRL[3:0]) */
	 GPIOA->AFR[0] &= ~(0xFUL << (0 * 4));
	 GPIOA->AFR[0] |=  (0x2UL << (0 * 4));      /* AF2 */
 }
 
 /*  ----------------------------------------------------
	 Timer-5 channel-1 * 50Hz PWM for hobby-servo
	 ---------------------------------------------------- */
 void TIM5_CH1_Init(void) {
 
	 /* 1. Enable TIM5 clock */
	 RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
 
	 /* 2. Up-counting, CK_CNT = 4MHz / 80 = 50kHz */
	 TIM5->PSC = 79;           /* divisor = 80                    */
	 TIM5->ARR = 999;          /* 1000ticks @50kHz --> 20ms      */
 
	 /* 3. PWM mode-1 on OC1 with preload */
	 TIM5->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
	 TIM5->CCMR1 |=  (6 << TIM_CCMR1_OC1M_Pos);
	 TIM5->CCMR1 |=  TIM_CCMR1_OC1PE;
 
	 /* 4. Output active-high & enable */
	 TIM5->CCER  &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
	 TIM5->CCER  |=  TIM_CCER_CC1E;
 
	 /* 5. Enable ARR buffer & latch registers */
	 TIM5->CR1  |= TIM_CR1_ARPE;
	 TIM5->EGR  |= TIM_EGR_UG;
 
	 /* 6. Start timer */
	 TIM5->CR1  |= TIM_CR1_CEN;
 }
 
 /*  ----------------------------------------------------
	 Main - breathing LED + servo sweep
	 ---------------------------------------------------- */
 int main(void) {
 
	 int i;
	 int duty   = 0;      /* TIM2 duty in counts (0-1000) */
	 int step   = 10;     /* fade step per loop           */
	 int dirUp  = 1;      /* 1 = brighten, 0 = dim        */
 
	 LED_Pin_Init();
	 SERVO_Pin_Init();
	 TIM2_CH1_Init();
	 TIM5_CH1_Init();
 
	 while (1) {
 
		 /* --- LED brightness (TIM2) --- */
		 TIM2->CCR1 = duty;                    /* update duty-cycle   */
 
		 /* --- Servo angle (TIM5) ---                                         *
		  * Map LED duty 0-1000 --> pulse 1-2ms. Every 20us per count:        *
		  *  1ms = 50 counts, 2ms = 100 counts                               */
		 TIM5->CCR1 = 50 + (duty / 20);        /* 0 -->50, 1000 -->100  */
 
		 /* --- Update duty for next loop --- */
		 if (dirUp) {
			 duty += step;
			 if (duty >= 1000) { duty = 1000; dirUp = 0; }
		 } else {
			 duty -= step;
			 if (duty <= 0)    { duty = 0;    dirUp = 1; }
		 }
 
		 /* crude delay - slows fade rate a bit */
		 for (i = 0; i < 3000; ++i) __NOP();
	 }
 }
 