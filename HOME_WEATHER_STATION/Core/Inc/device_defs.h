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

// Карта сети устройств
typedef struct network_map
{
	uint8_t device_name[32];	//Имя устройства
	uint8_t dev_addr;					//Адрес устройства
	uint8_t is_inited;				//Статус инициализации
}__attribute__((packed)) network_map[1];

// Макрос типа устройства
#define _CONTR_PANEL_

// Используемый I2C
#define USED_I2C hi2c1

#endif /* DEVICE_DEFS_H_ */
