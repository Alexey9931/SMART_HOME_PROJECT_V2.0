#ifndef RAM_H_
#define RAM_H_

#include "eeprom.h"

// Структура с данными для хранения в ОЗУ
typedef struct ram_struct 
{
	eeprom_data mirrored_to_rom_regs;
	int 				num_rx_pack;
	int 				num_tx_pack;
	int         work_time;
	ds3231_time	start_time;
	ds3231_time sys_time;
	float				temperature;
	float 			humidity;
	float				pressure;
}__attribute__((packed)) ram_data_struct;

#endif /* RAM_H_ */