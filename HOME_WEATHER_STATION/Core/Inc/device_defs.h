/**
* Файл с определениями и подключаемыми библиотеками для конкретного устройства
 */
#ifndef DEVICE_DEFS_H_
#define DEVICE_DEFS_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>

extern I2C_HandleTypeDef hi2c1;

// Макрос типа устройства
#define _CONTR_PANEL_

// Стандартная инициализация EEPROM (делается 1 раз)
//#define EEPROM_DEFAULT_INIT

// Стандартная инициализация RTC (делается 1 раз)
//#define RTC_DEFAULT_INIT

// Выбор используемого порта ETHERNET
#define ETHERNET_PORT1
//#define ETHERNET_PORT2

// Выбор используемого датчика температуры
//#define DHT22_DEFAULT_SENS

// Максимальное кол-во устр-в в сети (без сервера)
#define MAX_NET_SIZE		3

// Используемый I2C
#define USED_I2C hi2c1

typedef struct write_cmd_entry
{
	uint8_t 	status;
	uint16_t	reg_addr;
	void* 		value;
	uint16_t 	value_size;
}__attribute__((packed)) write_cmd_entry;

// Карта сети клиентских устройств
typedef struct client_network_map
{
	uint8_t 					device_name[32];		//Имя устройства
	uint8_t 					dev_addr;						//Адрес устройства
	uint8_t 					is_inited;					//Статус инициализации
	write_cmd_entry		write_entries[32];	//Набор записей с инфо для команд WRITE и CONFIG
}__attribute__((packed)) client_network_map;

// Карта сети всех устройств (с сервером)
typedef struct network_map
{
	client_network_map client_devs[MAX_NET_SIZE-1]; //Клиентские устройства
	uint8_t is_server_connected;										//Статус соединения с сервером
	uint8_t serv_addr;															//Адрес сервера
}__attribute__((packed)) network_map;

extern network_map dev_net_map;

write_cmd_entry* get_free_write_entry(client_network_map* client_dev);

write_cmd_entry* get_loaded_write_entry(client_network_map* client_dev);

void set_write_entry(write_cmd_entry* entry, uint16_t	reg_addr, void* value, uint16_t value_size);

void free_write_entry(write_cmd_entry* entry);

#endif /* DEVICE_DEFS_H_ */
