/*
 * ECE 153B – Project Part A
 * Non-blocking half-step step-motor driver (GPIOC 5, 6, 8, 9)
 *
 * Pin map (ULN2003 → NEMA-17):
 *   IN1 → PC5   IN2 → PC6   IN3 → PC8   IN4 → PC9
 *
 *  dire:  +1 = clockwise   -1 = counter-clockwise   0 = stop
 *
 *  HalfStep[] is listed in CCW order; stepping “backwards”
 *  (idx – dire) therefore yields CW when dire = +1.
 */

#include "stm32l476xx.h"
#include "motor.h"

/* ---- GPIO bit helpers -------------------------------------------------- */
#define COIL1  GPIO_ODR_OD5
#define COIL2  GPIO_ODR_OD6
#define COIL3  GPIO_ODR_OD8
#define COIL4  GPIO_ODR_OD9
#define COIL_MASK  (COIL1 | COIL2 | COIL3 | COIL4)

/* Mask with 0 s on motor pins – used to clear those bits quickly           */
static const uint32_t CLEAR_MASK = ~COIL_MASK;

/* 8-step half-stepping table ( **counter-clockwise** order )               */
static const uint32_t HalfStep[8] = {
    COIL1 | COIL4,   /* 1001 */
    COIL1,           /* 1000 */
    COIL1 | COIL3,   /* 1100 */
    COIL3,           /* 0100 */
    COIL2 | COIL3,   /* 0110 */
    COIL2,           /* 0010 */
    COIL2 | COIL4,   /* 0011 */
    COIL4            /* 0001 */
};

/* ---- driver state ------------------------------------------------------ */
static volatile int8_t dire = 0;   /* +1 CW, -1 CCW, 0 stop                */
static volatile int8_t step = 0;   /* current index in HalfStep[]          */

/* ---- public API -------------------------------------------------------- */
void Motor_Init(void)
{
    /* Enable GPIOC clock */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    /* PC5,6,8,9 → general-purpose output (01) */
    GPIOC->MODER &= ~(GPIO_MODER_MODE5_Msk | GPIO_MODER_MODE6_Msk |
                      GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE9_Msk);
    GPIOC->MODER |=  (GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0 |
                      GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0);

    /* Push-pull, fast speed */
    GPIOC->OTYPER  &= ~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 |
                        GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);

    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5_Msk | GPIO_OSPEEDR_OSPEED6_Msk |
                        GPIO_OSPEEDR_OSPEED8_Msk | GPIO_OSPEEDR_OSPEED9_Msk);
    GPIOC->OSPEEDR |=  (GPIO_OSPEEDR_OSPEED5_1 | GPIO_OSPEEDR_OSPEED6_1 |
                        GPIO_OSPEEDR_OSPEED8_1 | GPIO_OSPEEDR_OSPEED9_1);

    /* No pull-ups / pull-downs */
    GPIOC->PUPDR  &= ~(GPIO_PUPDR_PUPD5_Msk | GPIO_PUPDR_PUPD6_Msk |
                       GPIO_PUPDR_PUPD8_Msk | GPIO_PUPDR_PUPD9_Msk);

    /* Ensure coils are de-energised */
    GPIOC->ODR &= ~COIL_MASK;
}

void rotate(void)
/* Called from SysTick — must be **very** fast                              */
{
    if (dire == 0) {
        return;                         /* motor stopped                    */
    }

    /* Update index: HalfStep is CCW, so “-dire” moves CW when dire = +1    */
    step = (step - dire + 8) & 0x07;

    /* Output next pattern */
    GPIOC->ODR = (GPIOC->ODR & CLEAR_MASK) | HalfStep[step];
}

void setDire(int8_t direction)
/* direction ∈ {-1, 0, +1}                                                  */
{
    dire = direction;
}
