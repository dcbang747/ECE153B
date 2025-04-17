/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 2C
 */
 
 #include "stm32l476xx.h"

 #include "LED.h"
 #include "RTC.h"
 #include "SysClock.h"
 
 char strTime[12] = {0};
 char strDate[12] = {0};
 
 /* ------------------------------------------------
  *  Replace the three TODO sections in Main.c
  * ------------------------------------------------*/
 
  void RTC_Set_Alarm(void) {
     /*——— disable write‑protection ———*/
     RTC_Disable_Write_Protection();
 
     /* 1. Disable both alarms while we program them */
     RTC->CR &= ~(RTC_CR_ALRAE | RTC_CR_ALRBE);
     /*    wait until the hardware says registers are writable             */
     while ((RTC->ISR & (RTC_ISR_ALRAWF | RTC_ISR_ALRBWF))
            != (RTC_ISR_ALRAWF | RTC_ISR_ALRBWF));
 
     /* ------------------------------------------------
      * Alarm A – triggers when seconds == 30
      *   (all other fields “don’t‑care”)
      * ------------------------------------------------*/
     uint32_t alrmar = 0;
     alrmar |= 3U << RTC_POSITION_TR_ST;           /* seconds tens = 3 (30 s) */
     /* seconds units = 0 – bit positions are already 0 */
     alrmar |= RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4;
     RTC->ALRMAR = alrmar;
 
     /* ------------------------------------------------
      * Alarm B – triggers every second
      *   (mask *all* fields)
      * ------------------------------------------------*/
     RTC->ALRMBR = RTC_ALRMBR_MSK1 | RTC_ALRMBR_MSK2 |
                   RTC_ALRMBR_MSK3 | RTC_ALRMBR_MSK4;
 
     /* 2. Enable interrupts for both alarms, then the alarms themselves */
     RTC->CR |=  RTC_CR_ALRAIE | RTC_CR_ALRBIE;    /* interrupt enable   */
     RTC->CR |=  RTC_CR_ALRAE  | RTC_CR_ALRBE;     /* alarm enable       */
 
     /*——— restore write‑protection ———*/
     RTC_Enable_Write_Protection();
 }
 
 void RTC_Alarm_Enable(void) {
     /* EXTI line 18 routes RTC alarm events to the NVIC */
 
     /* 1 | IMR: unmask  2 | EMR: create an event  3 | RTSR: rising edge */
     EXTI->IMR1  |= (1U << 18);
     EXTI->EMR1  |= (1U << 18);
     EXTI->RTSR1 |= (1U << 18);
     EXTI->FTSR1 &= ~(1U << 18);   /* rising only */
 
     /* 4 | clear any stale pending flag */
     EXTI->PR1 = (1U << 18);
 
     /* 5 | NVIC setup */
     NVIC_SetPriority(RTC_Alarm_IRQn, 0);
     NVIC_EnableIRQ  (RTC_Alarm_IRQn);
 }
 
 void RTC_Alarm_IRQHandler(void) {
     /* —— clear flags, then toggle LED —— */
     RTC_Disable_Write_Protection();
 
     uint32_t isr = RTC->ISR;
 
     if (isr & RTC_ISR_ALRAF) {          /* Alarm A fired */
         RTC->ISR &= ~RTC_ISR_ALRAF;     /* clear flag  */
         Green_LED_Toggle();
     }
     if (isr & RTC_ISR_ALRBF) {          /* Alarm B fired */
         RTC->ISR &= ~RTC_ISR_ALRBF;     /* clear flag  */
         Green_LED_Toggle();
     }
 
     RTC_Enable_Write_Protection();
 
     /* clear EXTI pending bit */
     EXTI->PR1 = (1U << 18);
 }
 
 int main(void) {
     System_Clock_Init();               /* 80 MHz core  */
     LED_Init();
 
     RTC_Init();
     RTC_Alarm_Enable();
     RTC_Set_Alarm();
 
     while (1) {
         /* update the watch‑window strings once per loop */
         Get_RTC_Calendar(strTime, strDate);
         __WFI();                       /* nap until an interrupt */
     }
 }
 