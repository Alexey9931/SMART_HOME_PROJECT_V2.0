#include "htu21d.h"

// Адрес микросхемы htu21d
#define HTU21D_ADDR 0x80

// Поддерживаемые команды
#define READ_TEMP_CMD 0xF3
#define READ_HUM_CMD 0xF5

float HTU21D_get_temperature(I2C_HandleTypeDef* hi2c)
{
	float temp = 0.0f;
	uint8_t result[3] = {0};
	uint16_t raw_result = 0;
	uint8_t data = READ_TEMP_CMD;
	
	HAL_I2C_Master_Transmit(hi2c, HTU21D_ADDR, &data, sizeof(data), 1000);
	HAL_Delay(85);

	HAL_I2C_Master_Receive(hi2c, HTU21D_ADDR, result, sizeof(result)/sizeof(result[0]), 100);
	
	raw_result = ((uint16_t)(result[0] << 8) | (result[1]));
	temp = ((float)raw_result * 175.72f / 65536.0f) - 46.85f;

	return temp;
}

float HTU21D_get_humidity(I2C_HandleTypeDef* hi2c)
{
	float hum = 0.0f;
	uint8_t result[3] = {0};
	uint16_t raw_result = 0;
	uint8_t data = READ_HUM_CMD;
	
	HAL_I2C_Master_Transmit(hi2c, HTU21D_ADDR, &data, sizeof(data), 1000);
	HAL_Delay(85);

	HAL_I2C_Master_Receive(hi2c, HTU21D_ADDR, result, sizeof(result)/sizeof(result[0]), 100);
	
	raw_result = ((uint16_t)(result[0] << 8) | (result[1]));
	hum = ((float)raw_result * 125.0f / 65536.0f) - 6.0f;

	return hum;
}
