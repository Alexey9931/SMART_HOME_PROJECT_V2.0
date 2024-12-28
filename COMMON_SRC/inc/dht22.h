/**
* API для взаимодействия с датчиком температуры/влажности dht22
 */
#ifndef __DHT22_H
#define __DHT22_H

#include "device_defs.h"

void dht22_init(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin);
float dht22_get_temp(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin);
float dht22_get_hum(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin);

#endif /* __DHT22_H */
