/*
 * ECE 153B – Project Part A
 * GPIOC5, 6, 8, 9 half-step driver (non-blocking)
 *
 * Pin map (ULN2003 → NEMA-17 coils):
 *   IN1 → PC5   IN2 → PC6   IN3 → PC8   IN4 → PC9
 *
 *  dire :  +1 = CW   –1 = CCW   0 = stop
 */

#include "stm32l476xx.h"
#include "motor.h"

/* ────────────────────── GPIO helpers ────────────────────── */
#define P1_PIN      5u
#define P2_PIN      6u
#define P3_PIN      8u
#define P4_PIN      9u

#define P1_BIT      (1u << P1_PIN)      /* GPIO_ODR bit for PC5  */
#define P2_BIT      (1u << P2_PIN)      /* GPIO_ODR bit for PC6  */
#define P3_BIT      (1u << P3_PIN)      /* GPIO_ODR bit for PC8  */
#define P4_BIT      (1u << P4_PIN)      /* GPIO_ODR bit for PC9  */

#define COIL_MASK   (P1_BIT | P2_BIT | P3_BIT | P4_BIT)
#define CLEAR_MASK  (~COIL_MASK)

/* ────────────────── 8-step half-step table (CCW order) ────────────────── */
static const uint32_t HalfStep[8] = {
    P1_BIT | P4_BIT,  /* 1001 */
    P1_BIT,           /* 1000 */
    P1_BIT | P3_BIT,  /* 1100 */
    P3_BIT,           /* 0100 */
    P2_BIT | P3_BIT,  /* 0110 */
    P2_BIT,           /* 0010 */
    P2_BIT | P4_BIT,  /* 0011 */
    P4_BIT            /* 0001 */
};

/* ───────────────────────── driver state ───────────────────────── */
static volatile int8_t dire = 0;      /* +1 CW, –1 CCW, 0 stop    */
static volatile uint8_t step = 0;     /* index in HalfStep[]      */

/* ───────────────────────── API functions ─────────────────────── */
void Motor_Init(void)
{
    /* Enable GPIOC clock */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    /* ---- MODER:   01 (general-purpose output) ---- */
    GPIOC->MODER &=
        ~((3u << (P1_PIN*2)) | (3u << (P2_PIN*2)) |
          (3u << (P3_PIN*2)) | (3u << (P4_PIN*2)));
    GPIOC->MODER |=
        (1u << (P1_PIN*2)) | (1u << (P2_PIN*2)) |
        (1u << (P3_PIN*2)) | (1u << (P4_PIN*2));

    /* ---- OTYPER: push-pull (reset state = 0) ---- */
    GPIOC->OTYPER &=
        ~((1u << P1_PIN) | (1u << P2_PIN) |
          (1u << P3_PIN) | (1u << P4_PIN));

    /* ---- OSPEEDR: 10 (fast) ---- */
    GPIOC->OSPEEDR &=
        ~((3u << (P1_PIN*2)) | (3u << (P2_PIN*2)) |
          (3u << (P3_PIN*2)) | (3u << (P4_PIN*2)));
    GPIOC->OSPEEDR |=
        (2u << (P1_PIN*2)) | (2u << (P2_PIN*2)) |
        (2u << (P3_PIN*2)) | (2u << (P4_PIN*2));

    /* ---- PUPDR: 00 (no pull) ---- */
    GPIOC->PUPDR &=
        ~((3u << (P1_PIN*2)) | (3u << (P2_PIN*2)) |
          (3u << (P3_PIN*2)) | (3u << (P4_PIN*2)));

    /* Make sure the coils are de-energised */
    GPIOC->ODR &= ~COIL_MASK;
}

void rotate(void)             /* Called from SysTick – keep it tiny */
{
    if (dire == 0) {
        return;               /* motor stopped                      */
    }

    /* HalfStep[] is CCW; subtracting dire performs CW when dire = +1 */
    step = (uint8_t)((step - dire) & 0x07);

    GPIOC->ODR = (GPIOC->ODR & CLEAR_MASK) | HalfStep[step];
}

void setDire(int8_t direction)   /* direction ∈ {–1, 0, +1} */
{
    dire = direction;
}
