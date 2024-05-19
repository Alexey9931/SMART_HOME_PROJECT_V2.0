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

// Структура с данными для хранения в ОЗУ
typedef struct ram_struct 
{
	eeprom_data 		mirrored_to_rom_regs;
	int 						num_rx_pack;
	int 						num_tx_pack;
	int         		work_time;
	uint8_t					reserv[12];
	float						temperature;
	float 					humidity;
	float						rainfall;
	float						wind_speed;
	wind_direction	wind_direct;
}__attribute__((packed)) ram_data_struct;

#endif /* RAM_H_ */