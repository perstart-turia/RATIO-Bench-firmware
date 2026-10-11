#include "tcs3472.h"
#include <math.h>

extern I2C_HandleTypeDef hi2c2;

HAL_StatusTypeDef TCS_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t cmd = CMD_SINGLE_RW | reg;

    return HAL_I2C_Mem_Write(&hi2c2, TCS3472_ADDR_WR, cmd, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}

uint8_t TCS_ReadReg(uint8_t reg)
{
    uint8_t val = 0;
    uint8_t cmd = CMD_SINGLE_RW | reg;
    HAL_I2C_Mem_Read(&hi2c2, TCS3472_ADDR_RD, cmd, I2C_MEMADD_SIZE_8BIT, &val, 1, 100);
    return val;
}

HAL_StatusTypeDef TCS_ReadAllRGB(TCS_RGBData_t *rgb_buf)
{
    uint8_t buf[8] = {0};
    uint8_t cmd = CMD_AUTO_INC | REG_CDATAL;   // start address 0x14
    HAL_StatusTypeDef ret;

    ret = HAL_I2C_Mem_Read(&hi2c2, TCS3472_ADDR_RD, cmd, I2C_MEMADD_SIZE_8BIT, buf, 8, 200);
    if(ret != HAL_OK)
        return ret;

    rgb_buf->clear  = (buf[1] << 8) | buf[0];
    rgb_buf->red    = (buf[3] << 8) | buf[2];
    rgb_buf->green  = (buf[5] << 8) | buf[4];
    rgb_buf->blue   = (buf[7] << 8) | buf[6];
    return HAL_OK;
}

void TCS_ClearInterrupt(void)
{
    uint8_t cmd = CMD_CLEAR_INT;
    HAL_I2C_Master_Transmit(&hi2c2, TCS3472_ADDR_WR, &cmd, 1, 100);
}

uint8_t TCS_Init(void)
{
    uint8_t id = TCS_ReadReg(REG_ID);
    if(id != 0x44)
        return 1;

    /* Integration time increased 154ms -> 700ms for higher sensitivity
       and lower shot noise (more photons per reading).
       Gain reduced 16x -> 4x so the Clear channel does NOT saturate:
       at 700ms/16x the Clear channel would exceed 65535. */
    TCS_WriteReg(REG_ATIME, ATIME_700MS);
    TCS_WriteReg(REG_CONTROL, GAIN_4X);
    TCS_WriteReg(REG_ENABLE, EN_PON | EN_AEN);
    HAL_Delay(10);
    return 0;
}

/* Take n independent readings and compute mean + std-dev for each channel.
   Each reading is one full integration cycle (700ms), so a call with n=10
   takes ~7s. The std-dev is the noise estimate: mean +/- std is what the
   experiment protocol uses to judge whether two ratios are separable. */
void TCS_Measure(uint8_t n, TCS_Stats_t *stats)
{
    double sum_c = 0, sum_r = 0, sum_g = 0, sum_b = 0;
    double sq_c  = 0, sq_r  = 0, sq_g  = 0, sq_b  = 0;
    double mean_c, mean_r, mean_g, mean_b;
    uint8_t i;

    if(n == 0)
        return;

    for(i = 0; i < n; i++)
    {
        TCS_RGBData_t d;
        HAL_Delay(710);              // wait one full integration cycle + margin
        TCS_ReadAllRGB(&d);
        sum_c += d.clear;  sq_c += (double)d.clear * d.clear;
        sum_r += d.red;    sq_r += (double)d.red * d.red;
        sum_g += d.green;  sq_g += (double)d.green * d.green;
        sum_b += d.blue;   sq_b += (double)d.blue * d.blue;
    }

    mean_c = sum_c / n;  mean_r = sum_r / n;
    mean_g = sum_g / n;  mean_b = sum_b / n;

    stats->clear_mean = (uint16_t)mean_c;
    stats->red_mean   = (uint16_t)mean_r;
    stats->green_mean = (uint16_t)mean_g;
    stats->blue_mean  = (uint16_t)mean_b;

    /* population std-dev = sqrt( E[x^2] - E[x]^2 ) */
    stats->clear_std = (uint16_t)sqrt(sq_c/n - mean_c*mean_c);
    stats->red_std   = (uint16_t)sqrt(sq_r/n - mean_r*mean_r);
    stats->green_std = (uint16_t)sqrt(sq_g/n - mean_g*mean_g);
    stats->blue_std  = (uint16_t)sqrt(sq_b/n - mean_b*mean_b);
}
