#ifndef __BMP180_H
#define __BMP180_H

#include "stm32f4xx_hal.h"

void bmp180_init(void);
float bmp180_get_temp(void);
float bmp180_get_press(int oss);

#endif /* __BMP180_H */