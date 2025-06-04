/*
 *  StepMotor.c  – half-step drive, non-blocking
 *  Coil-to-pin map (same labels as ULN2003 board):
 *      IN1 = PC5   IN2 = PC6   IN3 = PC8   IN4 = PC9
 */

#include "StepMotor.h"

#define IN1 (5U)
#define IN2 (6U)
#define IN3 (8U)
#define IN4 (9U)

#define MOTOR_PORT      GPIOC
#define MOTOR_CLK_EN()  (RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN)
#define MOTOR_MASK      ((1U<<IN1)|(1U<<IN2)|(1U<<IN3)|(1U<<IN4))

/* 8-step half-step truth table – arranged so (idx+dir)&7 walks CW/CCW */
static const uint16_t HalfStep[8] = {
    (1U<<IN1),                       /* 1000 */
    (1U<<IN1)|(1U<<IN2),             /* 1100 */
    (1U<<IN2),                       /* 0100 */
    (1U<<IN2)|(1U<<IN3),             /* 0110 */
    (1U<<IN3),                       /* 0010 */
    (1U<<IN3)|(1U<<IN4),             /* 0011 */
    (1U<<IN4),                       /* 0001 */
    (1U<<IN4)|(1U<<IN1)              /* 1001 */
};

static volatile StepDir_t g_dir  = STEPMOTOR_STOP;
static volatile uint8_t   g_idx  = 0;

/* ---------- GPIO setup ----------------------------------------------------- */
void StepMotor_Init(void)
{
    MOTOR_CLK_EN();

    MOTOR_PORT->MODER   &= ~( (3U<<(IN1*2)) | (3U<<(IN2*2)) |
                              (3U<<(IN3*2)) | (3U<<(IN4*2)) );
    MOTOR_PORT->MODER   |=  ( 1U<<(IN1*2) ) | (1U<<(IN2*2)) |
                            ( 1U<<(IN3*2) ) | (1U<<(IN4*2));
    MOTOR_PORT->OTYPER  &= ~MOTOR_MASK;         /* push-pull  */
    MOTOR_PORT->OSPEEDR |=  (2U<<(IN1*2)) | (2U<<(IN2*2)) |
                            (2U<<(IN3*2)) | (2U<<(IN4*2));
    MOTOR_PORT->PUPDR   &= ~( (3U<<(IN1*2)) | (3U<<(IN2*2)) |
                              (3U<<(IN3*2)) | (3U<<(IN4*2)) );
}

/* ---------- public helper -------------------------------------------------- */
void StepMotor_SetDir(StepDir_t dir)
{
    g_dir = dir;
}

/* ---------- called at 1 kHz from SysTick handler --------------------------- */
void StepMotor_RotateISR(void)
{
    /* tmin: 2 ms  → call every 2 ticks.  adjust if your motor stalls   */
    static uint8_t tick = 0;
    if (++tick < 2) return;
    tick = 0;

    if (g_dir == STEPMOTOR_STOP) return;

    /* advance index and output */
    g_idx = (g_idx + g_dir) & 7U;
    uint16_t pattern = HalfStep[g_idx];

    MOTOR_PORT->ODR = (MOTOR_PORT->ODR & ~MOTOR_MASK) | pattern;
}

/* ---------- tie into the vector table ------------------------------------- */
void SysTick_Handler(void)
{
    StepMotor_RotateISR();
}
