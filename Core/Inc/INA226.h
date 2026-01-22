#ifndef __INA226_H
#define __INA226_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

// Адрес INA226 (A0=GND, A1=GND)
#define INA226_ADDR 0x80  // 8-битный адрес

// Регистры
#define INA226_REG_CONFIG      0x00
#define INA226_REG_SHUNTVOLT   0x01
#define INA226_REG_BUSVOLT     0x02
#define INA226_REG_POWER       0x03
#define INA226_REG_CURRENT     0x04
#define INA226_REG_CALIBRATION 0x05

// Параметры шунта
#define INA226_SHUNT_OHMS 0.0343f

// Функции
void INA226_Init(I2C_HandleTypeDef *hi2c);
float INA226_ReadBusVoltage(void);
float INA226_ReadCurrent(void);
float INA226_ReadPower(void);
float INA226_ReadShuntVoltage(void);
uint16_t INA226_ReadReg(uint8_t reg);

#endif
