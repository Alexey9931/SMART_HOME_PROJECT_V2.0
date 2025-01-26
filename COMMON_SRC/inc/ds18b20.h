/**
* API для взаимодействия с датчиком температуры ds18b20
 */
#ifndef __DS18B20_H
#define __DS18B20_H

#include "device_defs.h"
#include "common.h"

#define SKIP_ROM 0
#define NO_SKIP_ROM 1
#define RESOLUTION_9BIT 0x1F
#define RESOLUTION_10BIT 0x3F
#define RESOLUTION_11BIT 0x5F
#define RESOLUTION_12BIT 0x7F

// Описание ошибок ds18b20
typedef enum
{
	DS18B20_OK,
	DS18B20_ERROR
} ds18b20_ret;

// Функция инициализации ds18b20
uint8_t ds18b20_init(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint8_t mode);
// Функция получения знака температуры
uint8_t ds18b20_get_sign(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint16_t dt);
// Функция получения температуры
ds18b20_ret ds18b20_get_temp(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, float *temp);

#endif /* __DS18B20_H */
