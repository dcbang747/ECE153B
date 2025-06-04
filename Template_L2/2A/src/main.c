/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 2A
 */
#include "stm32l4xx.h"  // or the appropriate device header

#define DELAY      1000          // Adjust for your desired step speed
#define PIN_A      (5)            // PC5
#define PIN_Bp     (9)            // PC9
#define PIN_Ap     (6)            // PC6
#define PIN_B      (8)            // PC8

// Simple blocking delay
static inline void Delay(void) {
    for (volatile int i = 0; i < DELAY; i++) {
        // just burn cycles
    }
}

void GPIO_Init(void){	
	// [TODO]
	// 1. Enable the clock for GPIO port C
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	// 2. Configure PC5, PC6, PC8, and PC9 as output (MODER = 01 for output mode)
	GPIOC->MODER &= ~((3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (8 * 2)) | (3U << (9 * 2))); // Clear
	GPIOC->MODER |=  ((1U << (5 * 2)) | (1U << (6 * 2)) | (1U << (8 * 2)) | (1U << (9 * 2)));  // Set to 01

	// 3. Set the output speed of the pins to fast (OSPEEDR = 10 for fast speed)
	GPIOC->OSPEEDR &= ~((3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (8 * 2)) | (3U << (9 * 2))); // Clear
	GPIOC->OSPEEDR |=  ((2U << (5 * 2)) | (2U << (6 * 2)) | (2U << (8 * 2)) | (2U << (9 * 2)));  // Set to 10

	// 4. Set output type to push-pull (OTYPER = 0 for push-pull)
	GPIOC->OTYPER &= ~((1U << 5) | (1U << 6) | (1U << 8) | (1U << 9)); // Clear bits to 0

	// 5. Set the pins to no pull-up, no pull-down (PUPDR = 00)
	GPIOC->PUPDR &= ~((3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (8 * 2)) | (3U << (9 * 2))); // Clear to 00
	}

	void Full_Stepping_Clockwise(void){
		// Each item in the sequence is which bits to SET in ODR
		static const uint16_t STEP_SEQ[4] = {
			(1 << PIN_A) | (1 << PIN_Bp), // Step 1: A=1, B'=1, A'=0, B=0
			(1 << PIN_Bp) | (1 << PIN_Ap),// Step 2: B'=1, A'=1, A=0, B=0
			(1 << PIN_Ap) | (1 << PIN_B), // Step 3: A'=1, B=1,  A=0, B'=0
			(1 << PIN_B)  | (1 << PIN_A)  // Step 4: B=1,  A=1,  A'=0, B'=0
		};
		
		// We need 512 repeats of the 4-step pattern = 2048 internal steps = 360°
		for (int i = 0; i < 512; i++) {
			for (int step = 0; step < 4; step++) {
				// 1) Clear bits for all four pins
				GPIOC->ODR &= ~((1 << PIN_A) | (1 << PIN_Bp) | (1 << PIN_Ap) | (1 << PIN_B));
				// 2) Set the bits needed for the current step
				GPIOC->ODR |= STEP_SEQ[step];
				// 3) Delay to let the motor rotate to that step
				Delay();
			}
		}
	}
	
	void Full_Stepping_CounterClockwise(void){
		static const uint16_t STEP_SEQ[4] = {
			(1 << PIN_B)  | (1 << PIN_A),  // Opposite order of stepping
			(1 << PIN_Ap) | (1 << PIN_B),  
			(1 << PIN_Bp) | (1 << PIN_Ap),
			(1 << PIN_A)  | (1 << PIN_Bp)
		};
		
		for (int i = 0; i < 512; i++) {
			for (int step = 0; step < 4; step++) {
				GPIOC->ODR &= ~((1 << PIN_A) | (1 << PIN_Bp) | (1 << PIN_Ap) | (1 << PIN_B));
				GPIOC->ODR |= STEP_SEQ[step];
				Delay();
			}
		}
	}
	

	void Half_Stepping_Clockwise(void){
		// 8-step half-stepping sequence
		static const uint16_t STEP_SEQ[8] = {
			(1 << PIN_A),                  // Step 1:  A=1
			(1 << PIN_A) | (1 << PIN_Bp),  // Step 2:  A=1, B'=1
			(1 << PIN_Bp),                 // Step 3:  B'=1
			(1 << PIN_Bp) | (1 << PIN_Ap), // Step 4:  B'=1, A'=1
			(1 << PIN_Ap),                 // Step 5:  A'=1
			(1 << PIN_Ap) | (1 << PIN_B),  // Step 6:  A'=1, B=1
			(1 << PIN_B),                  // Step 7:  B=1
			(1 << PIN_B)  | (1 << PIN_A)   // Step 8:  B=1, A=1
		};
	
		// Repeat 512 times (8 * 512 = 4096 steps = 360°)
		for (int i = 0; i < 512; i++) {
			for (int step = 0; step < 8; step++) {
				GPIOC->ODR &= ~((1 << PIN_A) | (1 << PIN_Bp) | (1 << PIN_Ap) | (1 << PIN_B));
				GPIOC->ODR |= STEP_SEQ[step];
				Delay();
			}
		}
	}
	
	void Half_Stepping_CounterClockwise(void){
		static const uint16_t STEP_SEQ[8] = {
			(1 << PIN_B)  | (1 << PIN_A), 
			(1 << PIN_B),
			(1 << PIN_B)  | (1 << PIN_Ap),
			(1 << PIN_Ap),
			(1 << PIN_Ap) | (1 << PIN_Bp),
			(1 << PIN_Bp),
			(1 << PIN_Bp) | (1 << PIN_A),
			(1 << PIN_A)
		};
	
		for (int i = 0; i < 512; i++) {
			for (int step = 0; step < 8; step++) {
				GPIOC->ODR &= ~((1 << PIN_A) | (1 << PIN_Bp) | (1 << PIN_Ap) | (1 << PIN_B));
				GPIOC->ODR |= STEP_SEQ[step];
				Delay();
			}
		}
	}
	


	int main(void){
		GPIO_Init();  
	
		// Full-step one full revolution clockwise
		Full_Stepping_Clockwise();
	
		// Pause
		for (volatile int i = 0; i < 500000; i++);
		
		Full_Stepping_CounterClockwise();

		for (volatile int i = 0; i < 500000; i++);
		
		Half_Stepping_Clockwise();
		
		for (volatile int i = 0; i < 500000; i++);		
	
		// Then do one full revolution counterclockwise using half-stepping
		Half_Stepping_CounterClockwise();
	
		while(1){
			// Loop forever
		}
	}
	
