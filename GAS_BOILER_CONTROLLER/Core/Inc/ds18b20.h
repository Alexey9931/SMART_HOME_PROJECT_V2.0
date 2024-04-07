#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

#define ds18b20_gpio_port GPIOD
#define ds18b20_gpio_pin GPIO_PIN_14

#define SKIP_ROM 0
#define NO_SKIP_ROM 1
#define RESOLUTION_9BIT 0x1F
#define RESOLUTION_10BIT 0x3F
#define RESOLUTION_11BIT 0x5F
#define RESOLUTION_12BIT 0x7F

uint8_t ds18b20_init(uint8_t mode);
void ds18b20_measure_temp_cmd(uint8_t mode, uint8_t DevNum);
void ds18b20_read_strat_cpad(uint8_t mode, uint8_t *Data, uint8_t DevNum);
uint8_t ds18b20_get_sign(uint16_t dt);
float ds18b20_convert(uint16_t dt);
float ds18b20_get_temp();

#endif /* __DS18B20_H */