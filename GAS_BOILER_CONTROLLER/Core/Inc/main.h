/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "modbus.h"
#include "eeprom.h"
#include "ram.h"
#include "ds3231_rtc.h"
#include "dht22.h"
#include "ds18b20.h"
#include "lcd.h"
#include "thermostat_alg.h"
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
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOE
#define LED2_Pin GPIO_PIN_3
#define LED2_GPIO_Port GPIOE
#define LED3_Pin GPIO_PIN_4
#define LED3_GPIO_Port GPIOE
#define LED4_Pin GPIO_PIN_5
#define LED4_GPIO_Port GPIOE
#define LED5_Pin GPIO_PIN_6
#define LED5_GPIO_Port GPIOE
#define ETH1_SS_Pin GPIO_PIN_4
#define ETH1_SS_GPIO_Port GPIOA
#define ETH1_RST_Pin GPIO_PIN_4
#define ETH1_RST_GPIO_Port GPIOC
#define ETH1_INT_Pin GPIO_PIN_5
#define ETH1_INT_GPIO_Port GPIOC
#define ETH1_INT_EXTI_IRQn EXTI9_5_IRQn
#define button_down_Pin GPIO_PIN_0
#define button_down_GPIO_Port GPIOB
#define button_down_EXTI_IRQn EXTI0_IRQn
#define button_up_Pin GPIO_PIN_1
#define button_up_GPIO_Port GPIOB
#define button_up_EXTI_IRQn EXTI1_IRQn
#define button_ok_Pin GPIO_PIN_7
#define button_ok_GPIO_Port GPIOE
#define button_ok_EXTI_IRQn EXTI9_5_IRQn
#define ETH2_SS_Pin GPIO_PIN_12
#define ETH2_SS_GPIO_Port GPIOB
#define ETH2_RST_Pin GPIO_PIN_13
#define ETH2_RST_GPIO_Port GPIOB
#define LCD_NSS_Pin GPIO_PIN_12
#define LCD_NSS_GPIO_Port GPIOD
#define SD_NSS_Pin GPIO_PIN_13
#define SD_NSS_GPIO_Port GPIOD
#define ds18b20_Pin GPIO_PIN_14
#define ds18b20_GPIO_Port GPIOD
#define dht22_Pin GPIO_PIN_15
#define dht22_GPIO_Port GPIOD
#define MOSFET_Pin GPIO_PIN_6
#define MOSFET_GPIO_Port GPIOC
#define NRF24_SS_Pin GPIO_PIN_15
#define NRF24_SS_GPIO_Port GPIOA
#define NRF24_CE_Pin GPIO_PIN_1
#define NRF24_CE_GPIO_Port GPIOD
#define led_link_Pin GPIO_PIN_6
#define led_link_GPIO_Port GPIOD
#define led_error_Pin GPIO_PIN_7
#define led_error_GPIO_Port GPIOD
#define led_rx_Pin GPIO_PIN_3
#define led_rx_GPIO_Port GPIOB
#define led_tx_Pin GPIO_PIN_4
#define led_tx_GPIO_Port GPIOB
#define led_status_Pin GPIO_PIN_5
#define led_status_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
