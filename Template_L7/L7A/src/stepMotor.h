#ifndef STEPMOTOR_H
#define STEPMOTOR_H

#include "stm32l476xx.h"

/* public direction enum ----------------------------------------------------- */
typedef enum {
    STEPMOTOR_STOP = 0,
    STEPMOTOR_CW   = +1,
    STEPMOTOR_CCW  = -1
} StepDir_t;

/* API ----------------------------------------------------------------------- */
void StepMotor_Init(void);
void StepMotor_SetDir(StepDir_t dir);

/* called from SysTick (in StepMotor.c) */
void StepMotor_RotateISR(void);

#endif
