/**
* API для взаимодействия c датчиком влажности htu21d
 */
#ifndef __HTU21D_H
#define __HTU21D_H

#include "device_defs.h"

float HTU21D_get_temperature(I2C_HandleTypeDef* hi2c);
float HTU21D_get_humidity(I2C_HandleTypeDef* hi2c);

#endif /* __HTU21D_H */
