/**
* Файл с определениями и подключаемыми библиотеками для конкретного устройства
 */
#ifndef DEVICE_DEFS_H_
#define DEVICE_DEFS_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

extern I2C_HandleTypeDef hi2c1;

// Макрос типа устройства
#define _GAS_BOILER_CONTR_

// Используемый I2C
#define USED_I2C hi2c1
// Максимально возможная уставка температуры
#define MAX_TEMP_SET 35.0f
// Минимально возможная уставка температуры
#define MIN_TEMP_SET 0.0f
// Шаг декремента/инкремента уставки температуры
#define TEMP_SET_INC 0.5f

#endif /* DEVICE_DEFS_H_ */
