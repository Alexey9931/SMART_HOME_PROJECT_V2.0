/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "modbus.h"
#include "eeprom.h"
#include "ram.h"
#include "ds18b20.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_2
#define LED_GPIO_Port GPIOA
#define DS18B20_Pin GPIO_PIN_3
#define DS18B20_GPIO_Port GPIOA
#define ETH1_SCS_Pin GPIO_PIN_4
#define ETH1_SCS_GPIO_Port GPIOA
#define ETH1_RST_Pin GPIO_PIN_0
#define ETH1_RST_GPIO_Port GPIOB
#define ETH1_INT_Pin GPIO_PIN_1
#define ETH1_INT_GPIO_Port GPIOB
#define gerkon_1_Pin GPIO_PIN_12
#define gerkon_1_GPIO_Port GPIOB
#define gerkon_2_Pin GPIO_PIN_13
#define gerkon_2_GPIO_Port GPIOB
#define gerkon_3_Pin GPIO_PIN_14
#define gerkon_3_GPIO_Port GPIOB
#define gerkon_4_Pin GPIO_PIN_15
#define gerkon_4_GPIO_Port GPIOB
#define gerkon_5_Pin GPIO_PIN_8
#define gerkon_5_GPIO_Port GPIOA
#define gerkon_6_Pin GPIO_PIN_9
#define gerkon_6_GPIO_Port GPIOA
#define gerkon_7_Pin GPIO_PIN_10
#define gerkon_7_GPIO_Port GPIOA
#define gerkon_8_Pin GPIO_PIN_11
#define gerkon_8_GPIO_Port GPIOA
#define hall_Pin GPIO_PIN_15
#define hall_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
