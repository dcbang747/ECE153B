/*
 * ECE 153B – Project Part A
 * 28BYJ-48 half-step driver, GPIOC 5 6 8 9
 *
 * Coil map (matches your blocking demo):
 *   PC5  →  A
 *   PC9  →  B'
 *   PC6  →  A'
 *   PC8  →  B
 */

#include "stm32l476xx.h"
#include "motor.h"

/* ----- pin definitions ----------------- */
#define PIN_A   5u   /* PC5  */
#define PIN_Bp  9u   /* PC9  */
#define PIN_Ap  6u   /* PC6  */
#define PIN_B   8u   /* PC8  */

#define BIT_A   (1U << PIN_A )
#define BIT_Bp  (1U << PIN_Bp)
#define BIT_Ap  (1U << PIN_Ap)
#define BIT_B   (1U << PIN_B )

#define COIL_MASK (BIT_A | BIT_Bp | BIT_Ap | BIT_B)

/* ----- exact eight-step clockwise ----------- */
static const uint16_t HalfStep[8] = {
    BIT_A,                  /* 1: A          */
    BIT_A  | BIT_Bp,        /* 2: A  + B'    */
    BIT_Bp,                 /* 3:      B'    */
    BIT_Bp | BIT_Ap,        /* 4:      B'+A' */
    BIT_Ap,                 /* 5:        A'  */
    BIT_Ap | BIT_B,         /* 6:        A'+B*/
    BIT_B,                  /* 7:           B*/
    BIT_B  | BIT_A          /* 8:        B +A*/
};

/* ----- driver state ---------------------------------------------------- */
static volatile int8_t  dire = 0;   /* +1 = CW, −1 = CCW, 0 = stop */
static volatile uint8_t idx  = 0;   /* current table index         */

/* ----------------------------------------------------------------------- */
void Motor_Init(void)
{
    /* enable GPIOC clock */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    /* PC5,6,8,9 → output, push-pull, fast, no pull */
    GPIOC->MODER &=
        ~(3U<<(PIN_A*2) | 3U<<(PIN_Ap*2) | 3U<<(PIN_B*2) | 3U<<(PIN_Bp*2));
    GPIOC->MODER |=
         (1U<<(PIN_A*2) | 1U<<(PIN_Ap*2) | 1U<<(PIN_B*2) | 1U<<(PIN_Bp*2));

    GPIOC->OTYPER &= ~(BIT_A | BIT_Ap | BIT_B | BIT_Bp);

    GPIOC->OSPEEDR &=
        ~(3U<<(PIN_A*2) | 3U<<(PIN_Ap*2) | 3U<<(PIN_B*2) | 3U<<(PIN_Bp*2));
    GPIOC->OSPEEDR |=
         (2U<<(PIN_A*2) | 2U<<(PIN_Ap*2) | 2U<<(PIN_B*2) | 2U<<(PIN_Bp*2));

    GPIOC->PUPDR &=
        ~(3U<<(PIN_A*2) | 3U<<(PIN_Ap*2) | 3U<<(PIN_B*2) | 3U<<(PIN_Bp*2));

    /* de-energise coils */
    GPIOC->ODR &= ~COIL_MASK;
}

/* called from SysTick → one half-step every T_MIN_MS ms */
void rotate(void)
{
    if (dire == 0) {
        return;                      /* stopped */
    }

    /* advance or retreat exactly as your blocking code did */
    idx = (uint8_t)((idx + dire + 8) & 0x07);

    /* drive coils */
    GPIOC->ODR = (GPIOC->ODR & ~COIL_MASK) | HalfStep[idx];
}

void setDire(int8_t direction)   /* −1, 0, +1 */
{
    dire = direction;

    if (dire == 0) {
        /* immediately release coils */
        GPIOC->ODR &= ~COIL_MASK;
    } else {
        /* energise the current index so the first tick isn’t a double-jump */
        GPIOC->ODR = (GPIOC->ODR & ~COIL_MASK) | HalfStep[idx];
    }
}
