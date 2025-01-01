#ifndef __COMMON_H
#define __COMMON_H

#include "stm32f1xx_hal.h"

void delay_us(uint16_t us);
void set_pin_output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void set_pin_input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

#endif /* __COMMON_H */
