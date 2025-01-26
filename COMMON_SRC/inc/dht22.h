/**
* API для взаимодействия с датчиком температуры/влажности dht22
 */
#ifndef __DHT22_H
#define __DHT22_H

#include "device_defs.h"

// Описание ошибок dht22
typedef enum
{
	DHT22_OK,
	DHT22_ERROR
} dht22_ret;

// Функция инициализации dht22
void dht22_init(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin);
// Функция получения температуры
dht22_ret dht22_get_temp(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin, float *temp);
// Функция получения влажности
dht22_ret dht22_get_hum(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin, float *hum);

#endif /* __DHT22_H */
