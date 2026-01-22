#include "INA226.h"
#include "soft_i2c.h"  // ПРОГРАММНЫЙ I2C вместо HAL!

#define CURRENT_LSB 0.001f

// Запись в регистр INA226
static HAL_StatusTypeDef INA226_WriteReg(uint8_t reg, uint16_t value) {
    uint8_t buf[2];
    buf[0] = (value >> 8) & 0xFF;  // Старший байт первым (Big-Endian)
    buf[1] = value & 0xFF;

    uint8_t result = Soft_I2C_WriteReg(INA226_ADDR, reg, buf, 2);
    return (result == 0) ? HAL_OK : HAL_ERROR;
}

// Чтение регистра INA226
uint16_t INA226_ReadReg(uint8_t reg) {
    uint8_t buf[2] = {0, 0};

    uint8_t result = Soft_I2C_ReadReg(INA226_ADDR, reg, buf, 2);

    if (result != 0) {
        return 0xDEAD;  // Ошибка чтения
    }

    return ((uint16_t)buf[0] << 8) | buf[1];
}

void INA226_Init(I2C_HandleTypeDef *hi2c) {
    (void)hi2c;  // Не используется - программный I2C

    // Проверяем что устройство отвечает
    if (Soft_I2C_IsDeviceReady(INA226_ADDR) != 0) {
        // Устройство не готово
        return;
    }

    HAL_Delay(10);

    // Программный сброс INA226
    INA226_WriteReg(INA226_REG_CONFIG, 0x8000);
    HAL_Delay(10);

    // Калибровка
    // CAL = 0.00512 / (Current_LSB * R_shunt)
    // CAL = 0.00512 / (0.001 * 0.033) = 155.15 ≈ 155
    uint16_t cal = (uint16_t)(0.00512f / (CURRENT_LSB * INA226_SHUNT_OHMS));
    INA226_WriteReg(INA226_REG_CALIBRATION, cal);
    HAL_Delay(5);

    // Конфигурация: 0x4127
    // Bits 11-9: AVG=100 (16 samples)
    // Bits 8-6: VBUSCT=100 (1.1ms)
    // Bits 5-3: VSHCT=001 (140us)
    // Bits 2-0: MODE=111 (continuous)
    INA226_WriteReg(INA226_REG_CONFIG, 0x4127);
    HAL_Delay(10);

    // Проверяем что записалось
    uint16_t readback = INA226_ReadReg(INA226_REG_CONFIG);
    (void)readback;  // Для отладки можно вывести
}

float INA226_ReadBusVoltage(void) {
    uint16_t raw = INA226_ReadReg(INA226_REG_BUSVOLT);
    if (raw == 0xDEAD || raw == 0xFFFF) return 0.0f;

    float v_raw = (float)raw * 0.00125f;
    return v_raw * 0.9963f; // Ваш вычисленный коэффициент
}

float INA226_ReadCurrent(void) {
    int16_t raw = (int16_t)INA226_ReadReg(INA226_REG_CURRENT);

    if (raw == (int16_t)0xDEAD || raw == (int16_t)0xFFFF) {
        return 0.0f;
    }

    // Current LSB = 1 mA
    return (float)raw * CURRENT_LSB;
}

float INA226_ReadPower(void) {
    uint16_t raw = INA226_ReadReg(INA226_REG_POWER);

    if (raw == 0xDEAD || raw == 0xFFFF) {
        return 0.0f;
    }

    // Power LSB = 25 * Current_LSB
    return (float)raw * CURRENT_LSB * 25.0f;
}

float INA226_ReadShuntVoltage(void) {
    int16_t raw = (int16_t)INA226_ReadReg(INA226_REG_SHUNTVOLT);

    if (raw == (int16_t)0xDEAD) {
        return 0.0f;
    }

    // Shunt Voltage LSB = 2.5 uV
    return (float)raw * 0.0000025f;
}
