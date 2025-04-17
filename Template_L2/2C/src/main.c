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

/* ------------ EXTI line 18 (RTC Alarm) enable --------------------------------- */
void RTC_Alarm_Enable(void)
{
    EXTI->IMR1  |= (1u << 18);      /* un‑mask interrupt           */
    EXTI->EMR1  |= (1u << 18);      /* enable event (optional)     */
    EXTI->RTSR1 |= (1u << 18);      /* rising‑edge trigger         */
    EXTI->FTSR1 &= ~(1u << 18);     /* falling edge disabled       */
    EXTI->PR1    = (1u << 18);      /* clear any pending flag      */

    NVIC_SetPriority(RTC_Alarm_IRQn, 0);
    NVIC_EnableIRQ  (RTC_Alarm_IRQn);
}


/* ------------ program Alarm A (every 30 s) and Alarm B (every second) ---------- */
void RTC_Set_Alarm(void)
{

#define ENABLE_ALARM_A   0   /* set to 0 if you want Alarm A OFF */
#define ENABLE_ALARM_B   1   /* set to 1 if you want Alarm B ON  */

    RTC_Disable_Write_Protection();

    /* 1.  Disable both alarms and wait until they are writable ------------ */
    RTC->CR &= ~(RTC_CR_ALRAE | RTC_CR_ALRBE);
    while (!(RTC->ISR & RTC_ISR_ALRAWF));
    while (!(RTC->ISR & RTC_ISR_ALRBWF));

    /* 2.  -----------  Alarm A : trigger when seconds == 30  -------------- */
#if ENABLE_ALARM_A
    {
        uint32_t alrmar = 0;
        alrmar |= RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK2; /* ignore date/hr/min */

        uint8_t secs = 0x30;                       /* 30 s in BCD */
        alrmar |= (((secs >> 4) & 0x07) << 4) |    /* ST[6:4] */
                  (( secs       & 0x0F) << 0);     /* SU[3:0] */

        RTC->ALRMAR = alrmar;
    }
#endif

    /* 3.  -----------  Alarm B : fire every second  ----------------------- */
#if ENABLE_ALARM_B
    RTC->ALRMBR = RTC_ALRMBR_MSK4 | RTC_ALRMBR_MSK3 |
                  RTC_ALRMBR_MSK2 | RTC_ALRMBR_MSK1;  /* all “don’t‑care” */
#endif

    /* 4.  Enable only the chosen alarms & their interrupts ---------------- */
    uint32_t crMask = 0;
#if ENABLE_ALARM_A
    crMask |= RTC_CR_ALRAE | RTC_CR_ALRAIE;
#endif
#if ENABLE_ALARM_B
    crMask |= RTC_CR_ALRBE | RTC_CR_ALRBIE;
#endif
    RTC->CR |= crMask;

    RTC_Enable_Write_Protection();
}



/* ------------ ISR : toggle the green LED whenever a chosen alarm fires -------- */
void RTC_Alarm_IRQHandler(void)
{
    if (RTC->ISR & RTC_ISR_ALRAF) {          /* Alarm A? */
        RTC->ISR &= ~RTC_ISR_ALRAF;          /* clear flag                */
        EXTI->PR1  = (1u << 18);             /* clear EXTI line 18        */
        Green_LED_Toggle();
    }
    if (RTC->ISR & RTC_ISR_ALRBF) {          /* Alarm B? */
        RTC->ISR &= ~RTC_ISR_ALRBF;
        EXTI->PR1  = (1u << 18);
        Green_LED_Toggle();
    }
}


int main(void) {	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	
	LED_Init();
	
	RTC_Init();
	RTC_Alarm_Enable();
	RTC_Set_Alarm();
	
	while(1) {
		// [TODO]
	}
}
