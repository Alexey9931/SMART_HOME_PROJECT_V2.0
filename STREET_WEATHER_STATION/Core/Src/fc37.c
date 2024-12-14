#include "fc37.h"

float get_rain_fall(ADC_HandleTypeDef *hadc)
{
	float result = 0.0f;
	uint32_t raw_val = 0;
	
	HAL_ADC_Start(hadc);
  HAL_ADC_PollForConversion(hadc, 100);
  raw_val = HAL_ADC_GetValue(hadc);
  HAL_ADC_Stop(hadc);
	if (raw_val > ADC_MAX)
	{
		result = 0.0f;
	}
	else if (raw_val < ADC_MIN)
	{
		result = 100.0f;
	}
	else
	{
		result = 100 * (ADC_MAX - raw_val) / (ADC_MAX - ADC_MIN);
	}
	
	return result;
}
