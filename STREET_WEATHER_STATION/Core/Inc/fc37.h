/**
* API для взаимодействия c датчиком осадков FC37
 */
#ifndef FC37_H_
#define FC37_H_

#include "device_defs.h"

#define ADC_MAX 1600	//Сырое значение АЦП для уровня 0%
#define ADC_MIN 500		//Сырое значение АЦП для уровня 100%

// Функция получения кол-ва осадков
float get_rain_fall(ADC_HandleTypeDef *hadc);

#endif /* FC37_H_ */
