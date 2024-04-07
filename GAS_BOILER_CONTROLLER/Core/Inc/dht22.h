#ifndef __DHT22_H
#define __DHT22_H

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define dht22_gpio_port GPIOD
#define dht22_gpio_pin GPIO_PIN_15

void dht22_init(void);
uint8_t dht22_get_data(uint8_t *data);

#endif /* __DHT22_H */