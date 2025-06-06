/*
 * ECE 153B – Part-B Garage-Door controller
 *
 *  – 28BYJ-48 stepper (motor.c)
 *  – ADXL345 accel  (SPI)
 *  – TC74 temp-sensor (I²C, addr 0x48)
 *  – UART2 + DMA prints / console
 *
 *  Door logic:
 *      closed   ↔ |Ay| > 0.95 g
 *      open     ↔ |Az| > 0.95 g
 *      temp_hi  = 28 °C → open
 *      temp_lo  = 25 °C → close
 */

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "motor.h"
#include "SPI.h"
#include "I2C.h"
#include "accelerometer.h"
#include "UART.h"
#include "LED.h"

/* ---------- temperature sensor (TC74) ---------- */
static float readTempC(void)
{
    uint8_t t;
    I2C_ReceiveData(I2C1, 0x48<<1, &t, 1);   /* 7-bit addr <<1 */
    return (float)(int8_t)t;                 /* TC74 gives °C   */
}

/* ---------- door state machine ---------- */
typedef enum { CLOSED, OPEN, MOVING_UP, MOVING_DOWN } DoorState;
static DoorState door = CLOSED;

static void door_stop  (void){ setDire(0); }
static void door_up    (void){ setDire(+1); door = MOVING_UP; }
static void door_down  (void){ setDire(-1); door = MOVING_DOWN; }

/* ---------- globals ---------- */
static char  out[IO_SIZE];
static float lastTemp   = -1000.f;
static int32_t hold_ms  = 0;                 /* manual override timer */

void UART_onInput(char *cmd, uint32_t n)
{
    (void)n;
    if (!strcmp(cmd,"open") || !strcmp(cmd,"o")){
        UART_print("Console: opening door\r\n");
        door_down();
        hold_ms = 6000;
    } else if (!strcmp(cmd,"close") || !strcmp(cmd,"c")){
        UART_print("Console: closing door\r\n");
        door_up();
        hold_ms = 6000;
    } else if (!strcmp(cmd,"stop") || !strcmp(cmd,"s")){
        UART_print("Console: stop\r\n");
        door_stop();
        hold_ms = 6000;
    } else {
        UART_print("Commands: open|close|stop\r\n");
    }
}

int main(void)
{
    System_Clock_Init();           /* 80 MHz            */
    Motor_Init();
    SysTick_Init();

    UART2_Init();

    LED_Init();
    SPI1_GPIO_Init();
    SPI1_Init();
    initAcc();

    I2C_GPIO_Init();
    I2C_Initialization();

    UART_print("Garage-door firmware ready\r\n");

    /* timers */
    uint32_t ms_100  = 0;          /* 0.1 s sensor poll */
    uint32_t ms_1000 = 0;          /* 1 s status print  */

    while (1) {

        /* ----- 0.1 s tasks ------------------------------------------ */
        readValues( & (double){0}, & (double){0}, & (double){0} ); /* dummy first call to ensure SPI ready */

        if (ms_100 >= 100) {
            ms_100 = 0;

            double ax, ay, az;
            readValues(&ax,&ay,&az);

            /* manual-override window */
            if (hold_ms > 0) hold_ms -= 100;

            /* door-moving end-stops */
            if (door == MOVING_UP   && fabs(az) > 1.00) { door = CLOSED;   door_stop(); UART_print("Door closed\r\n"); }
            if (door == MOVING_DOWN && fabs(ay) > 1.15) { door = OPEN; door_stop(); UART_print("Door opened\r\n"); }

            /* automatic control */
            float T = readTempC();
            if (fabsf(T - lastTemp) >= 0.5f) {
                sprintf(out,"Temp = %.1fC\r\n", T);
                UART_print(out);
                lastTemp = T;
            }

            if (hold_ms == 0) {
                if (door == CLOSED && T >= 28.0f) {
                    UART_print("Temperature high - opening\r\n");
                    door_down();
                } else if (door == OPEN && T <= 25.0f) {
                    UART_print("Temperature low - closing\r\n");
                    door_up();
                }
            }
        }

        /* ----- 1 s heartbeat ---------------------------------------- */
        if (ms_1000 >= 1000) {
            ms_1000 = 0;
            LED_Toggle();
        }

        delay(1);                  /* 1 ms granularity */
        ms_100  += 1;
        ms_1000 += 1;
    }
}
