/**
 * API для взаимодействия с ПЗУ
 */
#ifndef EEPROM_H_
#define EEPROM_H_

#include "device_defs.h"
#include "ds3231_rtc.h"

// Адреса микросхем ПЗУ
#define EEPROM_ADDR_1 0xA0
#define EEPROM_ADDR_2 0xA2

// Параметры микросхем ПЗУ
#define PAGE_SIZE 64     //Размер страницы
#define PAGE_NUM  512    //Кол-во страниц

// Структура с общими для всех устройств данными
// для хранения в ПЗУ
typedef struct common_eeprom_struct 
{
	uint8_t 	device_name[32];	//Имя устройства
	uint8_t 	ip_addr_1[4];			//IP адрес 1-го порта
	uint8_t 	ip_addr_2[4];			//IP адрес 2-го порта
	uint8_t 	ip_gate[4];				//IP маршрутизатора
	uint8_t 	ip_mask[4];				//Маскирование
	uint8_t 	mac_addr_1[6];		//MAC адрес 1-го порта
	uint8_t 	mac_addr_2[6];		//MAC адрес 2-го порта
	uint32_t 	local_port[2];		//Порты соединения (сокета)
	float			temp_correction;	//Калибровочная поправка для температуры
	float			hum_correction;		//Калибровочная поправка для влажности
}__attribute__((packed)) common_eeprom;

// Структура с данными для хранения в ПЗУ, 
// характерными для панели управления
typedef struct control_panel_eeprom_struct 
{
	uint8_t reserv[9];		//Резерв
}__attribute__((packed)) control_panel_eeprom;

// Структура с данными для хранения в ПЗУ,
// характерными для контроллера газового котла
typedef struct gas_boiler_eeprom_struct 
{
	float		temp_setpoint;		//Уставка температуры
	float		temp_range;				//Нижний предел уставки температуры
	uint8_t temp_source;			//Выбор датчика для измерения температуры
}__attribute__((packed)) gas_boiler_eeprom;

// Структура с данными для хранения в ПЗУ,
// характерными для уличной метеостанции
typedef struct strweathstat_eeprom_struct 
{
	uint8_t reserv[9];		//Резерв
}__attribute__((packed)) strweathstat_eeprom;

// Обобщенная структура уникальных данных
typedef union eeprom_uniq_struct
{
	control_panel_eeprom 	control_panel;
	gas_boiler_eeprom			gas_boiler;
	strweathstat_eeprom		str_weath_stat;
} __attribute__((packed)) uniq_eeprom;

// Обобщенная структура для всех устройств
typedef struct eeprom_struct
{
	common_eeprom		common;	//Общие данные для всех устройств
	uniq_eeprom			unig;		//Уникальные данные устройства
}__attribute__((packed)) eeprom_data;

// Функция первичной инициализации микросхем ПЗУ (выполняется один раз)
void eeproms_first_ini(I2C_HandleTypeDef* hi2c);
// Универсальная функция записи в ПЗУ (пока сделана только возможность записи в 1-ую микросхему)
void eeprom_write(I2C_HandleTypeDef* hi2c, uint16_t addr, uint8_t *data, uint16_t size);
// Универсальная функция чтения из ПЗУ (пока сделана только возможность чтения из 1-ой микросхемы)
void eeprom_read(I2C_HandleTypeDef* hi2c, uint16_t addr, uint8_t *data, uint16_t size);
// Функция записи в ПЗУ
void _eeprom_write(I2C_HandleTypeDef* hi2c, uint8_t rom_num, uint16_t page, uint16_t offset, uint8_t *data, uint16_t size);
// Функция чтения из ПЗУ
void _eeprom_read(I2C_HandleTypeDef* hi2c, uint8_t rom_num, uint16_t page, uint16_t offset, uint8_t *data, uint16_t size);
// Функция очистки страниц ПЗУ
void eeprom_page_erase(I2C_HandleTypeDef* hi2c, uint8_t rom_num, uint16_t page);

#endif /* EEPROM_H_ */
