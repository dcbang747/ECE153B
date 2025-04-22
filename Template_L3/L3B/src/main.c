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

void LED_Pin_Init(void){
  // Enable GPIO Clocks
	// Initialize Green LED
	// GPIO Mode
	// Alternative Function
	//Set I/O output speed value as very high speed
	// GPIO Push-Pull: No pull-up, no pull-down
}
	

void TIM2_CH1_Init(void){
	// Enable the timer clock
	// Counting direction: 0 = up-counting, 1 = down-counting
  // Prescaler
  // Auto-reload
	// Disable output compare mode
	// PWM mode 1
	// Output 1 preload enable
	// Select output polarity: active high
  // Enable output for ch1
	// Output Compare Register for channel 1 
	// Enable counter
}

void SERVO_Pin_Init(void){
	// Set mode as Alternative Function 1
	//Set I/O output speed value as very high speed
	//Set I/O as no pull-up pull-down 
}

void TIM5_CH1_Init(void){
	// Enable the timer clock
	// Counting direction: 0 = up-counting, 1 = down-counting
  // Prescaler
  // Auto-reload
	// Disable output compare mode
	// PWM mode 1
	// Output 1 preload enable
	// Select output polarity: active high
  // Enable output for ch1
	// Output Compare Register for channel 1 
	// Enable counter
}

int main(void) {
	int i;
	int led = 0;
	int dire = 1;
	
	LED_Pin_Init();
	SERVO_Pin_Init();
	TIM2_CH1_Init();
	TIM5_CH1_Init();
		
    while(1) {
		// TODO
		TIM2->CCR1 = 0; //TODO
		TIM5->CCR1 = 0;//TODO
		
		for(i=0;i<300;i++);  		// delay
    }
}
