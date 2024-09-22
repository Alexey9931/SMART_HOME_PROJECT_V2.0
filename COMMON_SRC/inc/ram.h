#ifndef RAM_H_
#define RAM_H_

#include "eeprom.h"

// Структура байта-описания регистра WindDirection
typedef struct wind_direction_struct
{
	unsigned int north:1;
	unsigned int northeast:1;
	unsigned int east:1;
	unsigned int southeast:1;
	unsigned int south:1;
	unsigned int southwest:1;
	unsigned int west:1;
	unsigned int northwest:1;
}__attribute__((packed)) wind_direction;

// Структура с общими для всех устройств данными
// для хранения в ОЗУ
typedef struct common_ram_struct 
{
	eeprom_data 	mirrored_to_rom_regs;
	int 					num_rx_pack;
	int 					num_tx_pack;
	int         	work_time;
}__attribute__((packed)) common_ram;

// Структура с данными для хранения в ОЗУ
// характерными для контроллера газового котла
typedef struct gas_boiler_ram_struct 
{
	ds3231_time		start_time;
	ds3231_time 	sys_time;
	float					temperature;
	float 				humidity;
	uint8_t				rele_status;
}__attribute__((packed)) gas_boiler_ram;

// Структура с данными для хранения в ОЗУ
// характерными для уличной метеостанции
typedef struct strweathstat_ram_struct 
{
	uint8_t					reserv[14];
	float						temperature;
	float 					humidity;
	float						rainfall;
	float						wind_speed;
	wind_direction	wind_direct;
}__attribute__((packed)) strweathstat_ram;

// Структура с данными для хранения в ОЗУ
// характерными для панели управления
typedef struct control_panel_ram_struct 
{
	ds3231_time				start_time;
	ds3231_time 			sys_time;
	float							temperature;
	float 						humidity;
	float							pressure;
	// Здесь также содержатся данные других устройств
	common_ram				gas_boiler_common;
	gas_boiler_ram		gas_boiler_uniq;
	common_ram				str_weath_stat_common;
	strweathstat_ram	str_weath_stat_data;
}__attribute__((packed)) control_panel_ram;

// Обобщенная структура с уникальными данными 
// для хранения в ОЗУ
typedef union uniq_ram_struct 
{
	control_panel_ram		control_panel;
	gas_boiler_ram			gas_boiler;
	strweathstat_ram		str_weath_stat;
}__attribute__((packed)) uniq_ram_data;

// Cтруктура с данными для хранения в ОЗУ
typedef struct ram_struct 
{
	common_ram			common;
	uniq_ram_data		uniq;
}__attribute__((packed)) ram_data_struct;

#endif /* RAM_H_ */