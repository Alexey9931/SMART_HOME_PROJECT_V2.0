#include "gerkons.h"

void get_wind_direct(wind_direction *previous_value)
{
	wind_direction result = {0};
	
	if (HAL_GPIO_ReadPin(GPIO_PORT_N, GPIO_PIN_N) == GPIO_PIN_SET)
	{
		result.north = 1;
	}
	else if (HAL_GPIO_ReadPin(GPIO_PORT_S, GPIO_PIN_S) == GPIO_PIN_SET)
	{
		result.south = 1;
	}
	else if (HAL_GPIO_ReadPin(GPIO_PORT_W, GPIO_PIN_W) == GPIO_PIN_SET)
	{
		result.west = 1;
	}
	else if (HAL_GPIO_ReadPin(GPIO_PORT_E, GPIO_PIN_E) == GPIO_PIN_SET)
	{
		result.east = 1;
	}
	else if (HAL_GPIO_ReadPin(GPIO_PORT_N_E, GPIO_PIN_N_E) == GPIO_PIN_SET)
	{
		result.northeast = 1;
	}
	else if (HAL_GPIO_ReadPin(GPIO_PORT_N_W, GPIO_PIN_N_W) == GPIO_PIN_SET)
	{
		result.northwest = 1;
	}
	else if (HAL_GPIO_ReadPin(GPIO_PORT_S_W, GPIO_PIN_S_W) == GPIO_PIN_SET)
	{
		result.southwest = 1;
	}
	else if (HAL_GPIO_ReadPin(GPIO_PORT_S_E, GPIO_PIN_S_E) == GPIO_PIN_SET)
	{
		result.southeast = 1;
	}
	else
	{
		result = *previous_value;
	}
	
	*previous_value = result;
}
