/**
* API для взаимодействия c датчиком направления ветра (на базе герконов)
 */
#ifndef GERKONS_H_
#define GERKONS_H_

#include "device_defs.h"
#include "ram.h"

#define GPIO_PIN_N			GPIO_PIN_8	//GERKON5
#define GPIO_PORT_N			GPIOA
#define GPIO_PIN_S			GPIO_PIN_12	//GERKON1
#define GPIO_PORT_S			GPIOB
#define GPIO_PIN_W			GPIO_PIN_10	//GERKON7
#define GPIO_PORT_W			GPIOA
#define GPIO_PIN_E			GPIO_PIN_14	//GERKON3
#define GPIO_PORT_E			GPIOB
#define GPIO_PIN_N_E		GPIO_PIN_15	//GERKON4
#define GPIO_PORT_N_E		GPIOB
#define GPIO_PIN_N_W		GPIO_PIN_9	//GERKON6
#define GPIO_PORT_N_W		GPIOA
#define GPIO_PIN_S_W		GPIO_PIN_11	//GERKON8
#define GPIO_PORT_S_W		GPIOA
#define GPIO_PIN_S_E		GPIO_PIN_13	//GERKON2
#define GPIO_PORT_S_E		GPIOB

// Функция получения текущего направления ветра
void get_wind_direct(wind_direction *previous_value);

#endif /* GERKONS_H_ */
