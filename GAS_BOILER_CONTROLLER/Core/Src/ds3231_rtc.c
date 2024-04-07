#include "ds3231_rtc.h"

extern I2C_HandleTypeDef hi2c1;
ds3231_time sys_time;

uint8_t dec_to_binary(int val)
{
  return (uint8_t)( (val/10*16) + (val%10) );
}

int binary_to_dec(uint8_t val)
{
  return (int)( (val/16*10) + (val%16) );
}

void set_time(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year)
{
	uint8_t set_time[7];
	
	set_time[0] = dec_to_binary(sec);
	set_time[1] = dec_to_binary(min);
	set_time[2] = dec_to_binary(hour);
	set_time[3] = dec_to_binary(dow);
	set_time[4] = dec_to_binary(dom);
	set_time[5] = dec_to_binary(month);
	set_time[6] = dec_to_binary(year);
	HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, 0x00, 1, set_time, 7, 1000);
}

void get_time(void)
{
	uint8_t get_time[7];
	
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x00, 1, get_time, 7, 1000);
	sys_time.seconds = binary_to_dec(get_time[0]);
	sys_time.minutes = binary_to_dec(get_time[1]);
	sys_time.hour = binary_to_dec(get_time[2]);
	sys_time.dayofweek = binary_to_dec(get_time[3]);
	sys_time.dayofmonth = binary_to_dec(get_time[4]);
	sys_time.month = binary_to_dec(get_time[5]);
	sys_time.year = binary_to_dec(get_time[6]);
}

float get_temp(void)
{
	uint8_t temp[2];

	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x11, 1, temp, 2, 1000);
	
	return ((temp[0])+(temp[1]>>6)/4.0);
}

void force_temp_conv(void)
{
	uint8_t status=0;
	uint8_t control=0;
	
	HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x0F, 1, &status, 1, 100);  // read status register
	if (!(status&0x04))
	{
		HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x0E, 1, &control, 1, 100);  // read control register
		HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, 0x0E, 1, (uint8_t *)(control|(0x20)), 1, 100);
	}
}