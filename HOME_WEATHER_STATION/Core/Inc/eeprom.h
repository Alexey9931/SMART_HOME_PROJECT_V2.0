/**
 * API для взаимодействия с ПЗУ
 */
#ifndef EEPROM_H_
#define EEPROM_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Адреса микросхем ПЗУ
#define EEPROM_ADDR_1 0xA0
#define EEPROM_ADDR_2 0xA1
// Параметры микросхем ПЗУ
#define PAGE_SIZE 64     //Размер страницы
#define PAGE_NUM  512    //Кол-во страниц

// Имя устройства
#define DEVICE_NAME "Control Panel"

// Структура с данными для хранения в ПЗУ
typedef struct eeprom_struct 
{
	uint8_t 	device_name[32];	//Имя устройства
	uint8_t 	ip_addr[4];				//IP адрес
	uint8_t 	ip_gate[4];				//IP маршрутизатора
	uint8_t 	ip_mask[4];				//Маскирование
	uint8_t 	mac_addr[6];			//MAC адрес
	uint32_t 	local_port;				//Порт соединения (сокета)
}__attribute__((packed)) eeprom_data;

// Функция первичной инициализации микросхем ПЗУ (выполняется один раз)
void eeproms_first_ini();
// Универсальная функция записи в ПЗУ (пока сделана только возможность записи в 1-ую микросхему)
void eeprom_write(uint16_t addr, uint8_t *data, uint16_t size);
// Универсальная функция чтения из ПЗУ (пока сделана только возможность чтения из 1-ой микросхемы)
void eeprom_read(uint16_t addr, uint8_t *data, uint16_t size);
// Функция записи в ПЗУ
void _eeprom_write(uint8_t rom_num, uint16_t page, uint16_t offset, uint8_t *data, uint16_t size);
// Функция чтения из ПЗУ
void _eeprom_read(uint8_t rom_num, uint16_t page, uint16_t offset, uint8_t *data, uint16_t size);
// Функция очистки страниц ПЗУ
void eeprom_page_erase(uint8_t rom_num, uint16_t page);

#endif /* EEPROM_H_ */