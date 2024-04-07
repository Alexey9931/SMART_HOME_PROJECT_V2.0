#ifndef __BMP180_H
#define __BMP180_H

#include "stm32f4xx_hal.h"

#define BMP180_I2C &hi2c1
#define BMP180_ADDRESS 0xEE
#define ATMPRESS 101325 //Pa

void bmp180_init(void);
float bmp180_get_temp(void);
float bmp180_get_press(int oss);

#endif /* __BMP180_H */