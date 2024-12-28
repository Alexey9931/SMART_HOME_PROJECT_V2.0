#include "dht22.h"
#include "common.h"

void dht22_init(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin)
{
	set_pin_output(dht22_gpio_port, dht22_gpio_pin);
	HAL_GPIO_WritePin(dht22_gpio_port, dht22_gpio_pin, GPIO_PIN_RESET);
	HAL_Delay(1200);
	
	HAL_GPIO_WritePin(dht22_gpio_port, dht22_gpio_pin, GPIO_PIN_SET);
	delay_us(20);
	
	set_pin_input(dht22_gpio_port, dht22_gpio_pin);
}

uint8_t dht22_check_response(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin)
{
	uint8_t response = 0;
	
	set_pin_output(dht22_gpio_port, dht22_gpio_pin);
	HAL_GPIO_WritePin(dht22_gpio_port, dht22_gpio_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(dht22_gpio_port, dht22_gpio_pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(dht22_gpio_port, dht22_gpio_pin, GPIO_PIN_RESET);
	HAL_Delay(18);
	HAL_GPIO_WritePin(dht22_gpio_port, dht22_gpio_pin, GPIO_PIN_SET);
	set_pin_input(dht22_gpio_port, dht22_gpio_pin);
	delay_us(40);
	if (!(HAL_GPIO_ReadPin(dht22_gpio_port, dht22_gpio_pin)))
	{
		delay_us(80);
		if ((HAL_GPIO_ReadPin(dht22_gpio_port, dht22_gpio_pin))) response = 1;
	}

	while (HAL_GPIO_ReadPin (dht22_gpio_port, dht22_gpio_pin));
	return response;
}

uint8_t dht22_get_data(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin)
{
	uint8_t i, j;
	
	for (j = 0; j < 8; j++)
	{
		while (!(HAL_GPIO_ReadPin(dht22_gpio_port, dht22_gpio_pin)));
		delay_us(40);

		if (!(HAL_GPIO_ReadPin(dht22_gpio_port, dht22_gpio_pin)))
		{
			i &= ~(1<<(7-j));
		}
		else
		{
			i |= (1<<(7-j));
		}
		while(HAL_GPIO_ReadPin(dht22_gpio_port, dht22_gpio_pin));
	}

	return i;
}

float dht22_get_temp(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin)
{
	float temp = 0.0f;
	uint8_t raw_data[5] = {0};
	
	if (!dht22_check_response(dht22_gpio_port, dht22_gpio_pin)) return temp;
	
	for (uint8_t i = 0; i < 5; i++)
	{
		raw_data[i] = dht22_get_data(dht22_gpio_port, dht22_gpio_pin);
	}
	
	temp = (float)(((raw_data[2]<<8)|raw_data[3])/10.0f);
	
	return temp;
}

float dht22_get_hum(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin)
{
	float hum = 0.0f;
	uint8_t raw_data[5] = {0};
	
	if (!dht22_check_response(dht22_gpio_port, dht22_gpio_pin)) return hum;
	
	for (uint8_t i = 0; i < 5; i++)
	{
		raw_data[i] = dht22_get_data(dht22_gpio_port, dht22_gpio_pin);
	}
	
	hum = (float)(((raw_data[0]<<8)|raw_data[1])/10.0f);
	
	return hum;
}
