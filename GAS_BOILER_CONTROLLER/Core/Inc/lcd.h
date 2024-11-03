/**
 * API для взаимодействия с дисплеем
 */
#ifndef LCD_H_
#define LCD_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define CS_LCD_GPIO_PORT    GPIOD
#define CS_LCD_PIN          GPIO_PIN_12

#define SS_LCD_SELECT 	HAL_GPIO_WritePin(CS_LCD_GPIO_PORT, CS_LCD_PIN, GPIO_PIN_RESET);
#define SS_LCD_DESELECT HAL_GPIO_WritePin(CS_LCD_GPIO_PORT, CS_LCD_PIN, GPIO_PIN_SET);

void send_max7219(char rg, char dt);
void max7219_init();
void max7219_clear();
void number_max7219(volatile long n);
uint8_t print_temp_max7219(int current_temp, int setpoint_temp, uint8_t is_lcd_on);

#endif /* LCD_H_ */