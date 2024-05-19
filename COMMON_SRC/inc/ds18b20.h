/**
* API для взаимодействия датчиком температуры ds18b20
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

uint8_t ds18b20_init(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint8_t mode);
void ds18b20_measure_temp_cmd(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint8_t mode, uint8_t DevNum);
void ds18b20_read_strat_cpad(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint8_t mode, uint8_t *Data, uint8_t DevNum);
uint8_t ds18b20_get_sign(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint16_t dt);
float ds18b20_convert(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint16_t dt);
float ds18b20_get_temp(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin);

#endif /* __DS18B20_H */