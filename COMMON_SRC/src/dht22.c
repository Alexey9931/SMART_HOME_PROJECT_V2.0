#include "dht22.h"
#include "common.h"

void dht22_init(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin)
{
	HAL_Delay(2000);
	set_pin_output(dht22_gpio_port, dht22_gpio_pin);
	HAL_GPIO_WritePin(dht22_gpio_port, dht22_gpio_pin, 1);
}

uint8_t dht22_get_data(GPIO_TypeDef* dht22_gpio_port, uint16_t dht22_gpio_pin, uint8_t *data)
{
	uint8_t i, j = 0;
	
	set_pin_output(dht22_gpio_port, dht22_gpio_pin);
	//reset port
	HAL_GPIO_WritePin(dht22_gpio_port, dht22_gpio_pin, 0);
	HAL_GPIO_WritePin(dht22_gpio_port, dht22_gpio_pin, 1);
	HAL_Delay(100);
	HAL_GPIO_WritePin(dht22_gpio_port, dht22_gpio_pin, 0);
	HAL_Delay(18);
	HAL_GPIO_WritePin(dht22_gpio_port, dht22_gpio_pin, 1);
	set_pin_input(dht22_gpio_port, dht22_gpio_pin);
	//дождемся ответа датчика
	delay_us(60);//20-40 мкс
	//если датчик нам не ответил притягиванием шины, то ошибка
	if(HAL_GPIO_ReadPin(dht22_gpio_port, dht22_gpio_pin))
	{
		return 1;
	}
	delay_us(80);
	//если датчик не отпустил шину, то ошибка
	if(!HAL_GPIO_ReadPin(dht22_gpio_port, dht22_gpio_pin))
	{
		return 1;
	}
	delay_us(80);
	//читаем данные (записываем байты в массив наоборот,
	//так как сначала передаётся старший, чтобы потом
	//не переворачивать двухбайтовый результат)
	for (j = 0; j < 5; j++)
	{
		data[4-j]=0;
		for(i=0; i < 8; i++)
		{
			while(!HAL_GPIO_ReadPin(dht22_gpio_port, dht22_gpio_pin));
			delay_us(35);
			if(HAL_GPIO_ReadPin(dht22_gpio_port, dht22_gpio_pin))
				//если шина за это время не притянулась к земле, то значит это единица, а если притянулась, то ноль
				data[4-j] |= (1<<(7-i));
			while(HAL_GPIO_ReadPin(dht22_gpio_port, dht22_gpio_pin));
		}
	}
	
	return 0;	
}
