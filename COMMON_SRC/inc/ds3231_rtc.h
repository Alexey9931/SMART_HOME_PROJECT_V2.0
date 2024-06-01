/**
* API для взаимодействия с микросхемой ds3231
 */
#ifndef __DS3231_RTC_H
#define __DS3231_RTC_H

#include "device_defs.h"

#define DS3231_ADDRESS 0xD0

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hour;
	uint8_t dayofweek;
	uint8_t dayofmonth;
	uint8_t month;
	uint8_t year;
} __attribute__((packed)) ds3231_time;

uint8_t dec_to_binary(int val); 
int binary_to_dec(uint8_t val);
void set_time(I2C_HandleTypeDef* hi2c, uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year);
void get_time(I2C_HandleTypeDef* hi2c);
float get_temp(I2C_HandleTypeDef* hi2c);
void force_temp_conv(I2C_HandleTypeDef* hi2c);

#endif /* __DS3231_RTC_H */
