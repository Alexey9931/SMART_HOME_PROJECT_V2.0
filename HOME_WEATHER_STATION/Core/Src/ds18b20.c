#include "ds18b20.h"

uint8_t ds18b20_reset(void)
{
  uint16_t status;
	
	set_pin_output(ds18b20_gpio_port, ds18b20_gpio_pin);
	HAL_GPIO_WritePin(ds18b20_gpio_port, ds18b20_gpio_pin, 0);
  delay_us(485);//задержка как минимум на 480 микросекунд
	HAL_GPIO_WritePin(ds18b20_gpio_port, ds18b20_gpio_pin, 1);
  delay_us(65);//задержка как минимум на 60 микросекунд
	set_pin_input(ds18b20_gpio_port, ds18b20_gpio_pin);
	status = HAL_GPIO_ReadPin(ds18b20_gpio_port, ds18b20_gpio_pin);
  delay_us(500);//задержка как минимум на 480 микросекунд

  return (status ? 1 : 0);//вернём результат
}

uint8_t ds18b20_read_bit(void)
{
  uint8_t bit = 0;
	
	set_pin_output(ds18b20_gpio_port, ds18b20_gpio_pin);
	HAL_GPIO_WritePin(ds18b20_gpio_port, ds18b20_gpio_pin, 0);
  delay_us(2);
	HAL_GPIO_WritePin(ds18b20_gpio_port, ds18b20_gpio_pin, 1);
	delay_us(13);
	set_pin_input(ds18b20_gpio_port, ds18b20_gpio_pin);
	bit = (HAL_GPIO_ReadPin(ds18b20_gpio_port, ds18b20_gpio_pin) ? 1 : 0);//проверяем уровень	
	delay_us(45);
	
  return bit;
}

uint8_t ds18b20_read_byte(void)
{
  uint8_t data = 0;
	
  for (uint8_t i = 0; i <= 7; i++)
	{
		data += ds18b20_read_bit() << i;
	}
	
  return data;
}

void ds18b20_write_bit(uint8_t bit)
{
	set_pin_output(ds18b20_gpio_port, ds18b20_gpio_pin);
	HAL_GPIO_WritePin(ds18b20_gpio_port, ds18b20_gpio_pin, 0);
  delay_us(bit ? 3 : 65);
	HAL_GPIO_WritePin(ds18b20_gpio_port, ds18b20_gpio_pin, 1);
  delay_us(bit ? 65 : 3);
}

void ds18b20_write_byte(uint8_t dt)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    ds18b20_write_bit(dt >> i & 1);
    delay_us(5);
  }
}

uint8_t ds18b20_init(uint8_t mode)
{
	if(ds18b20_reset())
	{
		return 1;
  }
	if(mode==SKIP_ROM)
  {
		//SKIP ROM
		ds18b20_write_byte(0xCC);
		//WRITE SCRATCHPAD
		ds18b20_write_byte(0x4E);
		//TH REGISTER 100 градусов
		ds18b20_write_byte(0x64);
		//TL REGISTER - 30 градусов
		ds18b20_write_byte(0x9E);
		//Resolution 12 bit
		ds18b20_write_byte(RESOLUTION_12BIT);
  }
	
  return 0;
}

void ds18b20_measure_temp_cmd(uint8_t mode, uint8_t DevNum)
{
  ds18b20_reset();
  if(mode==SKIP_ROM)
  {
    //SKIP ROM
    ds18b20_write_byte(0xCC);
  }
  //CONVERT T
  ds18b20_write_byte(0x44);
}

void ds18b20_read_strat_cpad(uint8_t mode, uint8_t *Data, uint8_t DevNum)
{
  uint8_t i;
	
  ds18b20_reset();
  if(mode==SKIP_ROM)
  {
    //SKIP ROM
    ds18b20_write_byte(0xCC);
  }
  //READ SCRATCHPAD
  ds18b20_write_byte(0xBE);
  for(i=0;i<8;i++)
  {
    Data[i] = ds18b20_read_byte();
  }
}

uint8_t ds18b20_get_sign(uint16_t dt)
{
  //Проверим 11-й бит
  if (dt&(1<<11)) return 1;
  else return 0;
}

float ds18b20_convert(uint16_t dt)
{
  float t;
	
  t = (float) ((dt&0x07FF)>>4); //отборосим знаковые и дробные биты
  //Прибавим дробную часть
  t += (float)(dt&0x000F) / 16.0f;
  
	return t;
}
float ds18b20_get_temp()
{
	uint8_t dt[8];
	uint16_t raw_temper;
	float temp;
	
	ds18b20_measure_temp_cmd(SKIP_ROM, 0);
	ds18b20_read_strat_cpad(SKIP_ROM, dt, 0);
	raw_temper = ((uint16_t)dt[1]<<8)|dt[0];
	temp = ds18b20_convert(raw_temper);
	if (ds18b20_get_sign(raw_temper))	
	{
		temp *= -1.0f;
	}
	
	return temp;
}
