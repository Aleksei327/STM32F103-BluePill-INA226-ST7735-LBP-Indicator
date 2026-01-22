/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fonts.h"   // Сначала шрифты
#include "st7735.h"  // Потом дисплей
#include <stdio.h>   // Для sprintf
#include "ina226.h"
#include "soft_i2c.h"  // Добавь программный I2C!
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  //MX_I2C1_Init();

  /* USER CODE BEGIN 2 */
  ST7735_Init();


  // ИНИЦИАЛИЗАЦИЯ ПРОГРАММНОГО I2C (вместо MX_I2C1_Init)
  Soft_I2C_Init();

  char status_msg[64];

  // Проверка INA226
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_WriteString(0, 0, "Soft I2C Test", Font_7x10, ST7735_CYAN, ST7735_BLACK);

  if (Soft_I2C_IsDeviceReady(INA226_ADDR) == 0) {
      ST7735_WriteString(0, 12, "INA226 FOUND!", Font_7x10, ST7735_GREEN, ST7735_BLACK);
  } else {
      ST7735_WriteString(0, 12, "INA226 NOT FOUND", Font_7x10, ST7735_RED, ST7735_BLACK);
      while(1);  // Стоп если не найдено
  }
  //HAL_Delay(2000);

  // Инициализация INA226
  INA226_Init(NULL);  // hi2c не используется

  // Тестовое чтение
  uint16_t raw_v = INA226_ReadReg(INA226_REG_BUSVOLT);
  uint16_t raw_c = INA226_ReadReg(INA226_REG_CALIBRATION);

  sprintf(status_msg, "RawV: 0x%04X", raw_v);
  ST7735_WriteString(0, 24, status_msg, Font_7x10, ST7735_WHITE, ST7735_BLACK);

  sprintf(status_msg, "Cal: 0x%04X", raw_c);
  ST7735_WriteString(0, 36, status_msg, Font_7x10, ST7735_WHITE, ST7735_BLACK);

  float test_v = INA226_ReadBusVoltage();
  sprintf(status_msg, "Volt: %.3fV", test_v);
  ST7735_WriteString(0, 48, status_msg, Font_7x10, ST7735_GREEN, ST7735_BLACK);

  //HAL_Delay(3000);

  // Основной экран
  ST7735_FillScreen(ST7735_BLACK);

  char bottom_row[40];
  float v_val = 0.0f;
  float i_val = 0.0f;
  float i_raw = 0.0f;
  float power = 0.0f;
  float capacity_ah = 0.0f;
  char v_str[20], i_str[20];

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      v_val = INA226_ReadBusVoltage();
      //i_raw = INA226_ReadCurrent(); // Читаем чистый ток
      i_val = INA226_ReadCurrent(); // Читаем чистый ток
            // Выводим raw ток где-нибудь сбоку, чтобы понять реальную утечку
            //i_val = i_raw - (v_val * 0.00733f + 0.01667f);
            //if (i_val < 0) i_val = 0;

      power = INA226_ReadPower();

      //v_val = 9;
      //i_val = 0.6;
      //power = v_val * i_val;
      capacity_ah += (i_val * 0.1f) / 3600.0f;

      sprintf(v_str, "%2.3fV", v_val);
      sprintf(i_str, "%1.3fA", i_val);
      sprintf(bottom_row, "%.2fW %.3fAh", power, capacity_ah);

      ST7735_WriteString(20, 20, v_str, Font_16x26, 0xF800, ST7735_BLACK);
      ST7735_WriteString(20, 60, i_str, Font_16x26, ST7735_BLUE, ST7735_BLACK);
      ST7735_WriteString(10, 95, bottom_row, Font_11x18, ST7735_GREEN, ST7735_BLACK);

      HAL_Delay(100);
      /* USER CODE END WHILE */
      /* USER CODE 3*/
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/* Замени I2C_Scan в main.c на эту версию */

// Функция для программного сброса I2C если застрял
void I2C_SoftReset(I2C_HandleTypeDef *hi2c) {
    // Программный сброс периферии I2C
    hi2c->Instance->CR1 |= I2C_CR1_SWRST;
    HAL_Delay(5);
    hi2c->Instance->CR1 &= ~I2C_CR1_SWRST;
    HAL_Delay(5);

    // Снова включаем периферию
    hi2c->Instance->CR1 |= I2C_CR1_PE;
    HAL_Delay(5);
}

void I2C_Scan(I2C_HandleTypeDef *hi2c) {
    HAL_StatusTypeDef res;
    char msg[64];
    uint8_t devices_found = 0;
    int y_pos = 0;

    ST7735_FillScreen(ST7735_BLACK);
    ST7735_WriteString(0, y_pos, "I2C Scan v2", Font_7x10, ST7735_CYAN, ST7735_BLACK);
    y_pos += 12;

    // Показываем регистры ПЕРЕД сканированием
    sprintf(msg, "CR1:0x%04X SR1:0x%04X", (unsigned int)I2C1->CR1, (unsigned int)I2C1->SR1);
    ST7735_WriteString(0, y_pos, msg, Font_7x10, ST7735_WHITE, ST7735_BLACK);
    y_pos += 10;

    sprintf(msg, "CR2:0x%04X SR2:0x%04X", (unsigned int)I2C1->CR2, (unsigned int)I2C1->SR2);
    ST7735_WriteString(0, y_pos, msg, Font_7x10, ST7735_WHITE, ST7735_BLACK);
    y_pos += 12;

    // Проверка GPIO
    GPIO_PinState scl = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);
    GPIO_PinState sda = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);
    sprintf(msg, "SCL:%d SDA:%d", scl, sda);
    ST7735_WriteString(0, y_pos, msg, Font_7x10, ST7735_YELLOW, ST7735_BLACK);
    y_pos += 12;

    ST7735_WriteString(0, y_pos, "Scanning...", Font_7x10, ST7735_GREEN, ST7735_BLACK);
    y_pos += 12;

    // Сканирование с автоматическим recovery
    for (uint16_t i = 1; i < 128; i++) {
        // ВАЖНО: Короткий таймаут и только 1 попытка!
        res = HAL_I2C_IsDeviceReady(hi2c, (uint16_t)(i << 1), 1, 2);

        if (res == HAL_OK) {
            // УСТРОЙСТВО НАЙДЕНО!
            sprintf(msg, ">>> 0x%02X (8bit:0x%02X)", i, i << 1);
            ST7735_WriteString(0, y_pos, msg, Font_7x10, ST7735_GREEN, ST7735_BLACK);
            y_pos += 10;
            devices_found++;
        }
        else if (res == HAL_BUSY) {
            // При BUSY делаем софтверный сброс
            sprintf(msg, "BUSY at 0x%02X, reset", i);
            ST7735_WriteString(0, y_pos, msg, Font_7x10, ST7735_RED, ST7735_BLACK);
            y_pos += 10;

            I2C_SoftReset(hi2c);

            sprintf(msg, "After reset SR1:0x%04X", (unsigned int)I2C1->SR1);
            ST7735_WriteString(0, y_pos, msg, Font_7x10, ST7735_YELLOW, ST7735_BLACK);
            y_pos += 10;

            // Продолжаем сканирование
            continue;
        }
        else if (res == HAL_ERROR) {
            // ERROR - это нормально, значит NACK (устройства нет)
            // Ничего не выводим
        }
        else if (res == HAL_TIMEOUT) {
            // Таймаут - тоже нормально
        }
    }

    // Итоги
    y_pos += 2;
    if (devices_found == 0) {
        ST7735_WriteString(0, y_pos, "NO DEVICES!", Font_7x10, ST7735_RED, ST7735_BLACK);
    } else {
        sprintf(msg, "Found: %d device(s)", devices_found);
        ST7735_WriteString(0, y_pos, msg, Font_7x10, ST7735_GREEN, ST7735_BLACK);
    }
}

/* Тест конкретного адреса - с автоматическим recovery */
void I2C_TestAddress(I2C_HandleTypeDef *hi2c, uint8_t addr_7bit) {
    char msg[64];
    int y_pos = 0;

    ST7735_FillScreen(ST7735_BLACK);
    sprintf(msg, "Test 0x%02X (8bit:0x%02X)", addr_7bit, addr_7bit << 1);
    ST7735_WriteString(0, y_pos, msg, Font_7x10, ST7735_CYAN, ST7735_BLACK);
    y_pos += 14;

    // Показываем регистры ДО
    sprintf(msg, "BEFORE: SR1:0x%04X", (unsigned int)I2C1->SR1);
    ST7735_WriteString(0, y_pos, msg, Font_7x10, ST7735_WHITE, ST7735_BLACK);
    y_pos += 10;
    sprintf(msg, "        SR2:0x%04X", (unsigned int)I2C1->SR2);
    ST7735_WriteString(0, y_pos, msg, Font_7x10, ST7735_WHITE, ST7735_BLACK);
    y_pos += 14;

    // Пробуем подключиться
    uint8_t addr_8bit = addr_7bit << 1;
    HAL_StatusTypeDef res = HAL_I2C_IsDeviceReady(hi2c, addr_8bit, 3, 10);

    // Показываем результат
    if (res == HAL_OK) {
        ST7735_WriteString(0, y_pos, ">>> FOUND! <<<", Font_7x10, ST7735_GREEN, ST7735_BLACK);
        y_pos += 12;
        ST7735_WriteString(0, y_pos, "Device is ready!", Font_7x10, ST7735_GREEN, ST7735_BLACK);
    } else if (res == HAL_BUSY) {
        ST7735_WriteString(0, y_pos, "I2C BUSY", Font_7x10, ST7735_RED, ST7735_BLACK);
        y_pos += 10;
        ST7735_WriteString(0, y_pos, "Doing reset...", Font_7x10, ST7735_YELLOW, ST7735_BLACK);
        I2C_SoftReset(hi2c);
    } else if (res == HAL_TIMEOUT) {
        ST7735_WriteString(0, y_pos, "TIMEOUT - no device", Font_7x10, ST7735_YELLOW, ST7735_BLACK);
    } else if (res == HAL_ERROR) {
        ST7735_WriteString(0, y_pos, "NACK - no device", Font_7x10, ST7735_YELLOW, ST7735_BLACK);
    }
    y_pos += 14;

    // Показываем регистры ПОСЛЕ
    sprintf(msg, "AFTER:  SR1:0x%04X", (unsigned int)I2C1->SR1);
    ST7735_WriteString(0, y_pos, msg, Font_7x10, ST7735_WHITE, ST7735_BLACK);
    y_pos += 10;
    sprintf(msg, "        SR2:0x%04X", (unsigned int)I2C1->SR2);
    ST7735_WriteString(0, y_pos, msg, Font_7x10, ST7735_WHITE, ST7735_BLACK);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
