#include "ds18b20.h"

uint8_t ds18b20_reset(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin)
{
  uint16_t status;
	
	set_pin_output(ds18b20_gpio_port, ds18b20_gpio_pin);
	HAL_GPIO_WritePin(ds18b20_gpio_port, ds18b20_gpio_pin, GPIO_PIN_RESET);
  delay_us(485);//задержка как минимум на 480 микросекунд
	HAL_GPIO_WritePin(ds18b20_gpio_port, ds18b20_gpio_pin, GPIO_PIN_SET);
  delay_us(65);//задержка как минимум на 60 микросекунд
	set_pin_input(ds18b20_gpio_port, ds18b20_gpio_pin);
	status = HAL_GPIO_ReadPin(ds18b20_gpio_port, ds18b20_gpio_pin);
  delay_us(500);//задержка как минимум на 480 микросекунд

  return (status ? 1 : 0);//вернём результат
}

uint8_t ds18b20_read_bit(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin)
{
  uint8_t bit = 0;
	
	set_pin_output(ds18b20_gpio_port, ds18b20_gpio_pin);
	HAL_GPIO_WritePin(ds18b20_gpio_port, ds18b20_gpio_pin, GPIO_PIN_RESET);
  delay_us(2);
	HAL_GPIO_WritePin(ds18b20_gpio_port, ds18b20_gpio_pin, GPIO_PIN_SET);
	delay_us(13);
	set_pin_input(ds18b20_gpio_port, ds18b20_gpio_pin);
	bit = (HAL_GPIO_ReadPin(ds18b20_gpio_port, ds18b20_gpio_pin) ? 1 : 0);//проверяем уровень	
	delay_us(45);
	
  return bit;
}

uint8_t ds18b20_read_byte(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin)
{
  uint8_t data = 0;
	
  for (uint8_t i = 0; i <= 7; i++)
	{
		data += ds18b20_read_bit(ds18b20_gpio_port, ds18b20_gpio_pin) << i;
	}
	
  return data;
}

void ds18b20_write_bit(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint8_t bit)
{
	set_pin_output(ds18b20_gpio_port, ds18b20_gpio_pin);
	HAL_GPIO_WritePin(ds18b20_gpio_port, ds18b20_gpio_pin, GPIO_PIN_RESET);
  delay_us(bit ? 3 : 65);
	HAL_GPIO_WritePin(ds18b20_gpio_port, ds18b20_gpio_pin, GPIO_PIN_SET);
  delay_us(bit ? 65 : 3);
}

void ds18b20_write_byte(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint8_t dt)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    ds18b20_write_bit(ds18b20_gpio_port, ds18b20_gpio_pin, dt >> i & 1);
    delay_us(5);
  }
}

uint8_t ds18b20_init(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint8_t mode)
{
	if(ds18b20_reset(ds18b20_gpio_port, ds18b20_gpio_pin))
	{
		return 1;
  }
	if(mode==SKIP_ROM)
  {
		//SKIP ROM
		ds18b20_write_byte(ds18b20_gpio_port, ds18b20_gpio_pin, 0xCC);
		//WRITE SCRATCHPAD
		ds18b20_write_byte(ds18b20_gpio_port, ds18b20_gpio_pin, 0x4E);
		//TH REGISTER 100 градусов
		ds18b20_write_byte(ds18b20_gpio_port, ds18b20_gpio_pin, 0x64);
		//TL REGISTER - 30 градусов
		ds18b20_write_byte(ds18b20_gpio_port, ds18b20_gpio_pin, 0x9E);
		//Resolution 12 bit
		ds18b20_write_byte(ds18b20_gpio_port, ds18b20_gpio_pin, RESOLUTION_12BIT);
  }
	
  return 0;
}

void ds18b20_measure_temp_cmd(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint8_t mode, uint8_t DevNum)
{
  ds18b20_reset(ds18b20_gpio_port, ds18b20_gpio_pin);
  if(mode==SKIP_ROM)
  {
    //SKIP ROM
    ds18b20_write_byte(ds18b20_gpio_port, ds18b20_gpio_pin, 0xCC);
  }
  //CONVERT T
  ds18b20_write_byte(ds18b20_gpio_port, ds18b20_gpio_pin, 0x44);
}

void ds18b20_read_strat_cpad(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint8_t mode, uint8_t *Data, uint8_t DevNum)
{
  uint8_t i;
	
  ds18b20_reset(ds18b20_gpio_port, ds18b20_gpio_pin);
  if(mode==SKIP_ROM)
  {
    //SKIP ROM
    ds18b20_write_byte(ds18b20_gpio_port, ds18b20_gpio_pin, 0xCC);
  }
  //READ SCRATCHPAD
  ds18b20_write_byte(ds18b20_gpio_port, ds18b20_gpio_pin, 0xBE);
  for(i=0;i<8;i++)
  {
    Data[i] = ds18b20_read_byte(ds18b20_gpio_port, ds18b20_gpio_pin);
  }
}

uint8_t ds18b20_get_sign(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint16_t dt)
{
  //Проверим 11-й бит
  if (dt&(1<<11)) return 1;
  else return 0;
}

float ds18b20_convert(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin, uint16_t dt)
{
  float t;
	
  t = (float) ((dt&0x07FF)>>4); //отборосим знаковые и дробные биты
  //Прибавим дробную часть
  t += (float)(dt&0x000F) / 16.0f;
  
	return t;
}
float ds18b20_get_temp(GPIO_TypeDef* ds18b20_gpio_port, uint16_t ds18b20_gpio_pin)
{
	uint8_t dt[8];
	uint16_t raw_temper;
	float temp;
	
	ds18b20_measure_temp_cmd(ds18b20_gpio_port, ds18b20_gpio_pin, SKIP_ROM, 0);
	ds18b20_read_strat_cpad(ds18b20_gpio_port, ds18b20_gpio_pin, SKIP_ROM, dt, 0);
	raw_temper = ((uint16_t)dt[1]<<8)|dt[0];
	temp = ds18b20_convert(ds18b20_gpio_port, ds18b20_gpio_pin, raw_temper);
	if (ds18b20_get_sign(ds18b20_gpio_port, ds18b20_gpio_pin, raw_temper))	
	{
		temp *= -1.0f;
	}
	if ((temp == 85.0f)||(raw_temper == 0xFFFF)) return 0.0f;
	
	return temp;
}
