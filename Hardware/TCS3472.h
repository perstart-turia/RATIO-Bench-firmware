#ifndef __TCS3472_H
#define __TCS3472_H

#include "stm32f1xx_hal.h"
#include "main.h"

// I2C device address TCS34725
#define TCS3472_ADDR        0x29
#define TCS3472_ADDR_WR     (TCS3472_ADDR << 1)    // 0x52
#define TCS3472_ADDR_RD     ((TCS3472_ADDR << 1)|1) // 0x53

// COMMAND register modes
#define CMD_SINGLE_RW      0x80    // Bit7=1 TYPE=00 single-byte r/w
#define CMD_AUTO_INC       0xA0    // Bit7=1 TYPE=01 auto-increment burst read
#define CMD_CLEAR_INT      0xE6    // special clear-interrupt command

// Register addresses
#define REG_ENABLE     0x00
#define REG_ATIME      0x01
#define REG_WTIME      0x03
#define REG_AILTL      0x04
#define REG_AILTH      0x05
#define REG_AIHTL      0x06
#define REG_AIHTH      0x07
#define REG_PERS       0x0C
#define REG_CONFIG     0x0D
#define REG_CONTROL    0x0F
#define REG_ID         0x12
#define REG_STATUS     0x13
#define REG_CDATAL     0x14
#define REG_CDATAH     0x15
#define REG_RDATAL     0x16
#define REG_RDATAH     0x17
#define REG_GDATAL     0x18
#define REG_GDATAH     0x19
#define REG_BDATAL     0x1A
#define REG_BDATAH     0x1B

// ENABLE register bits
#define EN_PON     (1 << 0) // power on
#define EN_AEN     (1 << 1) // ADC enable
#define EN_INT     (1 << 4) // interrupt enable

// Gain (CONTROL register)
#define GAIN_1X    0x00
#define GAIN_4X    0x01
#define GAIN_16X   0x02
#define GAIN_60X   0x03

// Integration time (ATIME). time = (256 - ATIME) * 2.4 ms
#define ATIME_2_4MS 0xFF
#define ATIME_24MS  0xF6
#define ATIME_101MS 0xD5
#define ATIME_154MS 0xC0
#define ATIME_700MS 0x00

// Raw RGB data struct
typedef struct
{
    uint16_t clear;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} TCS_RGBData_t;

// Statistics struct: mean + std-dev of N independent readings
typedef struct
{
    uint16_t clear_mean, red_mean, green_mean, blue_mean;
    uint16_t clear_std,  red_std,  green_std,  blue_std;
} TCS_Stats_t;

// Function declarations
HAL_StatusTypeDef TCS_WriteReg(uint8_t reg, uint8_t data);
uint8_t TCS_ReadReg(uint8_t reg);
HAL_StatusTypeDef TCS_ReadAllRGB(TCS_RGBData_t *rgb_buf);
void TCS_ClearInterrupt(void);
uint8_t TCS_Init(void);
void TCS_Measure(uint8_t n, TCS_Stats_t *stats);

#endif
