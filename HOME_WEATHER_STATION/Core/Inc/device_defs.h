/**
* Файл с определениями и подключаемыми библиотеками для конкретного устройства
 */
#ifndef DEVICE_DEFS_H_
#define DEVICE_DEFS_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

extern I2C_HandleTypeDef hi2c1;

// Максимальное кол-во устр-в в сети (без сервера)
#define MAX_NET_SIZE		3

// Карта сети клиентских устройств
typedef struct client_network_map
{
	uint8_t device_name[32];	//Имя устройства
	uint8_t dev_addr;					//Адрес устройства
	uint8_t is_inited;				//Статус инициализации
}__attribute__((packed)) client_network_map;

// Карта сети всех устройств (с сервером)
typedef struct network_map
{
	client_network_map client_devs[MAX_NET_SIZE-1]; //Клиентские устройства
	uint8_t is_server_connected;										//Статус соединения с сервером
	uint8_t serv_addr;															//Адрес сервера
}__attribute__((packed)) network_map;

// Макрос типа устройства
#define _CONTR_PANEL_

// Используемый I2C
#define USED_I2C hi2c1

#endif /* DEVICE_DEFS_H_ */
