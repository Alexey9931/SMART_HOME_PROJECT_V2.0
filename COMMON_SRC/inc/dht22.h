/**
* API для взаимодействия с датчиком температуры/влажности dht22
 */
#ifndef __DHT22_H
#define __DHT22_H

#include "device_defs.h"

void dht22_init(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin);
uint8_t dht22_get_data(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin, uint8_t *data);

#endif /* __DHT22_H */
