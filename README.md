# RATIO Bench — STM32F103 firmware

Firmware for **RATIO Bench**, a low-cost four-channel colourimeter that measures the ratio of two chromoprotein-expressing cell populations in a mixed culture.

A TCS34725 sensing module (with its own white LED) sits inside a 3D-printed light-tight shell with a sample well on top. The module reads the transmitted light on four channels (Red / Green / Blue / Clear); an STM32F103C8 reads it over I²C and drives a 0.96" OLED display. From the absorbance ratio `y = A_B/A_G` the cell fraction is recovered.

## Hardware

| Part | Role |
|---|---|
| STM32F103C8 ("Blue Pill") | MCU |
| TCS34725 module | RGB colour sensor (I²C, address 0x29) |
| 0.96" OLED (SSD1306) | display (I²C) |
| RATIO Bench: shell | 3D-printed, light-blocking enclosure |

Sensor on **I²C2**, display on **I²C1**.

## Firmware behaviour

- Integration time **700 ms**, gain **4×** — long integration for a lower noise floor, reduced gain to avoid saturating the Clear channel.
- **N = 10 independent readings** per measurement, reported as mean ± standard deviation.
- OLED page 1 shows the four channel means; page 2 shows the standard deviations.

## Files

```
Core/       STM32Cube-generated code (main.c, HAL init, interrupts)
Hardware/   TCS3472.c/.h (sensor driver), oled.c/.h, oledfont.c/.h (display)
Drivers/    ST HAL + CMSIS (third-party)
MDK-ARM/    Keil MDK project (igem_version1.uvprojx)
```

The application code lives in `Core/Src/main.c` and `Hardware/`.

## Build

1. Open `MDK-ARM/igem_version1.uvprojx` in Keil MDK (uVision 5).
2. Build the `igem_version1` target and flash over ST-Link.

> Note: the source files use GBK encoding (Chinese comments). Set the editor encoding to GBK/ANSI if the comments appear garbled.

## Measurement procedure

1. **Blank calibration** — measure the pure medium, record R₀ / G₀ / B₀ / C₀.
2. **Sample measurement** — place the sample in the well, read mean ± std-dev from the OLED (≈ 7 s).
3. **Compute** — A_G = −log₁₀(G/G₀), A_B = −log₁₀(B/B₀), y = A_B/A_G.
4. **Convert** — substitute y into the calibration curve to obtain the blue fraction.

## Related

- Pre-experiment protocol: `protocol_hardware_1.md` (dye concentration, ratio gradients)
- Pre-experiment data: `实验数据Hardware预实验.xlsx`
- Wiki pages: `hardware.md`, `engineering.md`
