/**
* API для взаимодействия c датчиком влажности htu21d
 */
#ifndef __HTU21D_H
#define __HTU21D_H

#include "device_defs.h"

// Описание ошибок htu21d
typedef enum
{
	HTU21D_OK,
	HTU21D_ERROR
} htu21d_ret;

// Функция получения температуры
htu21d_ret HTU21D_get_temperature(I2C_HandleTypeDef* hi2c, float *temp);
// Функция получения влажности
htu21d_ret HTU21D_get_humidity(I2C_HandleTypeDef* hi2c, float *hum);

#endif /* __HTU21D_H */
