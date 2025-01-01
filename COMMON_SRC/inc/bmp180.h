/**
* API для взаимодействия с bmp180
 */
#ifndef __BMP180_H
#define __BMP180_H

#include "device_defs.h"

#define BMP180_ADDRESS 0xEE
#define ATMPRESS 101325 //Pa

// Функция инициализации bmp180
void bmp180_init(I2C_HandleTypeDef* hi2c);
// Функция получения температуры
float bmp180_get_temp(I2C_HandleTypeDef* hi2c);
// Функция получения атм.давления
float bmp180_get_press(I2C_HandleTypeDef* hi2c, int oss);

#endif /* __BMP180_H */
