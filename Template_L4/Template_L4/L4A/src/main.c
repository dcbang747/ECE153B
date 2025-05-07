/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 4A
 */


 #include "stm32l476xx.h"
 #include "LED.h"
 #include "SysClock.h"
 #include "UART.h"
 #include <stdio.h>
 
 /* *** Choose the interface here ***
  *    2 → USART2  (USB virtual COM port, Termite on PC)
  *    1 → USART1  (HC 05 Bluetooth module)
  */
 static const int ACTIVE_USART = 1;   /* change to 1 for Bluetooth testing     */
 
 /* ------------------------------------------------------------------------- */
 static void Init_USARTx(int x)
 {
	 if (x == 1)
	 {
		 UART1_Init();
		 UART1_GPIO_Init();
		 USART_Init(USART1);
	 }
	 else if (x == 2)
	 {
		 UART2_Init();
		 UART2_GPIO_Init();
		 USART_Init(USART2);
	 }
 }
 
 int main(void)
 {
	 System_Clock_Init();   /* 80MHz system clock                             */
	 LED_Init();            /* configure PA5                                   */
 
	 Init_USARTx(ACTIVE_USART);
 
	 char cmd;
 
	 while (1)
	 {
		 printf("\r\nEnter command (Y/y/N/n) ");
 
		 /* The leading space skips CR/LF and other whitespace characters        */
		 scanf(" %c", &cmd);
 
		 switch (cmd)
		 {
			 case 'Y':
			 case 'y':
				 Green_LED_On();
				 printf("Green LED turned **ON**.\r\n");
				 break;
 
			 case 'N':
			 case 'n':
				 Green_LED_Off();
				 printf("Green LED turned **OFF**.\r\n");
				 break;
 
			 default:
				 printf("Unrecognized command '%c'.  Please try Y/y or N/n.\r\n", cmd);
				 break;
		 }
 
		 /* Extra safety for the HC 05: re init USART1 each cycle to clear any
			potential overflow/OR errors.                                        */
		 if (ACTIVE_USART == 1)
		 {
			 USART_Init(USART1);
		 }
	 }
 }