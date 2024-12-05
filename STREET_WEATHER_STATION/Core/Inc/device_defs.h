/**
* Файл с определениями и подключаемыми библиотеками для конкретного устройства
 */
#ifndef DEVICE_DEFS_H_
#define DEVICE_DEFS_H_

#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

extern I2C_HandleTypeDef hi2c1;

// Макрос типа устройства
#define _STREET_WEATH_ST_

// Стандартная инициализация EEPROM (делается 1 раз)
//#define EEPROM_DEFAULT_INIT

// Используемый I2C
#define USED_I2C hi2c1

#endif /* DEVICE_DEFS_H_ */
