
# Digital LBP (PS) Indicator (STM32F103 + INA226 + ST7735)

This project is a high-precision monitoring solution for Laboratory Power Supplies (LBP). The system displays real-time Voltage (V), Current (A), Instantaneous Power (W), and Accumulated Capacity (Ah).

## Key Feature: 100% Compatibility with "Chinese Clones"

This project was specifically designed for engineers who encounter issues with counterfeit or low-quality **STM32F103 (Blue Pill)** chips from AliExpress.

**The Problem:** In these clone chips, the hardware I2C peripheral is often unstable: the bus frequently hangs in a `BUSY` state, or the device fails to be detected entirely (`DEAD` status).
**The Solution:** This project utilizes **Software I2C (bit-banging)**. This approach guarantees 100% stable communication with the INA226 sensor regardless of the MCU's silicon quality, as we control the pins directly.

---

## Wiring Diagram

### ST7735 Display (1.8" SPI)

| Display Signal| STM32 Pin (Blue Pill) | Note |
| ------------- | -------- | ------------------------------ |
| **SCK (SCL)** | **PA5**  | SPI Clock                      |
| **SDA**       | **PA7**  | SPI MOSI                       |
| **RES**       | **PB0**  | Reset                          |
| **RS (DC)**   | **PB1**  | Register Select (Data/Command) |
| **CS**        | **PB10** | Chip Select                    |
| **LEDA**      | **3.3V** | Backlight                      |

### INA226 Sensor (I2C)

| Module Signal | STM32 Pin (Blue Pill) | Note |
| --- | --- | --- |
| **SCL** | **PB8** | Requires 4.7k Pull-up to 3.3V |
| **SDA** | **PB9** | Requires 4.7k Pull-up to 3.3V |

---

## Setup and Calibration

The project achieves very high accuracy (error margin as low as **0.6 mA**), but requires fine-tuning for your specific components:

1. **Shunt Configuration:** In the `Core/Inc/INA226.h` file, find the line `#define INA226_SHUNT_OHMS`. Enter the exact resistance of your shunt (e.g., `0.033f` for an R033 resistor).
2. **Measurement Correction:** Data processing logic is located in `Core/Src/main.c`.
* *Note:* Software Offset correction is currently disabled in the repository as high linear accuracy was achieved.
* If you need to adjust voltage or current readings, use multiplication coefficients within the `ReadBusVoltage` and `ReadCurrent` functions.



---

## How to Build and Flash

You will need **STM32CubeIDE** and **STM32CubeProgrammer**.

### 1. Preparation in STM32CubeIDE (Generating .bin file)

By default, CubeIDE does not generate a binary file. To enable this:

1. Right-click on your project name in the Project Explorer.
2. Select **Properties**.
3. Go to **C/C++ Build** -> **Settings**.
4. In the **Tool Settings** tab, locate **MCU Post build outputs**.
5. Check the box for **Convert to binary file (-O binary)**.
6. Click **Apply and Close** and build the project (hammer icon). A `.bin` file will appear in the `Debug` folder.

### 2. Flashing via STM32CubeProgrammer

If using a standard Chinese **ST-Link V2**:

1. Connect ST-Link to the Blue Pill (3.3V, GND, SWDIO, SWCLK).
2. Open **STM32CubeProgrammer**.
3. Select **ST-LINK** in the interface window on the right and click **Connect**.
4. Click the **Open file** tab and select your generated `.bin` file from the `Debug` folder.
5. Go to the **Erasing & Programming** tab (green down-arrow icon).
6. Click **Start Programming**.
7. Once finished, click **Disconnect** and reset your device.

---

## File Structure

* `Core/Src/soft_i2c.c` — Software I2C implementation (solving the clone chip issues).
* `Core/Src/INA226.c` — Sensor driver.
* `Core/Src/st7735.c` — Display driver.
* `Core/Src/main.c` — Main loop and calculation logic.

## About the Author

