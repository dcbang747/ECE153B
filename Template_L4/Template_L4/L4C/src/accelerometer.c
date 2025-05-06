#include "accelerometer.h"
#include "SPI.h"
#include <math.h>

/* ADXL345 register addresses */
#define REG_BW_RATE     0x2C
#define REG_POWER_CTL   0x2D
#define REG_DATA_FORMAT 0x31
#define REG_DATAX0      0x32   /* six data bytes start here */

static inline uint16_t make_cmd(uint8_t addr, uint8_t rw)
{
    /* rw = 1 for read, 0 for write; MB = 0 (single byte)                    */
    uint8_t cmd = (rw << 7) | (0 << 6) | (addr & 0x3F);
    return (uint16_t)cmd << 8;           /* command is sent first (MSB)      */
}

/* ——— low‑level single‑byte access ——— */
void accWrite(uint8_t addr, uint8_t val)
{
    uint16_t frame = make_cmd(addr, 0) | val;
    SPI_Transfer_Data(frame);            /* discard returned data            */
}

uint8_t accRead(uint8_t addr)
{
    uint16_t frame = make_cmd(addr, 1);  /* second byte is dummy 0x00        */
    uint16_t ret   = SPI_Transfer_Data(frame);
    return (uint8_t)(ret & 0x00FF);      /* keep second (LSB) byte           */
}

/* ——— device initialisation ——— */
void initAcc(void)
{
    accWrite(REG_BW_RATE,     0x0A);     /* 100 Hz ODR                      */
    accWrite(REG_DATA_FORMAT, 0x08);     /* full‑resolution, ±2 g           */
    accWrite(REG_POWER_CTL,   0x08);     /* measurement mode                */
}

/* ——— read and scale XYZ in g ——— */
void readValues(double *x, double *y, double *z)
{
    const double scale = 0.0039;         /* 4 mg/LSB in full‑res            */

    int16_t rawX = (int16_t)((accRead(REG_DATAX1) << 8) | accRead(REG_DATAX0));
    int16_t rawY = (int16_t)((accRead(REG_DATAX1 + 2) << 8) | accRead(REG_DATAX0 + 2));
    int16_t rawZ = (int16_t)((accRead(REG_DATAX1 + 4) << 8) | accRead(REG_DATAX0 + 4));

    *x = rawX * scale;
    *y = rawY * scale;
    *z = rawZ * scale;
}
