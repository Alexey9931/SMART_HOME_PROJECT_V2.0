/**
* API с реализацией алгоритма термостата
 */
#ifndef __THERMOSTAT_ALG_H
#define __THERMOSTAT_ALG_H

#include "device_defs.h"
#include "ram.h"

#define RELE_GPIO_PORT 	GPIOC
#define RELE_GPIO_PIN		GPIO_PIN_6

#define enable_rele		HAL_GPIO_WritePin(RELE_GPIO_PORT, RELE_GPIO_PIN, GPIO_PIN_SET);
#define disable_rele	HAL_GPIO_WritePin(RELE_GPIO_PORT, RELE_GPIO_PIN, GPIO_PIN_RESET);

// Реализация алгоритма термостата
void thermostat_task(void);

#endif /* __THERMOSTAT_ALG_H */
