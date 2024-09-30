#ifndef __DWIN_H
#define __DWIN_H

#include "stm32f4xx_hal.h"
#include "ram.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define DWIN_UART huart1
#define BUF_SIZE 	400

#define HEADER 0xA55A

// Поддерживаемые протоколом команды
typedef enum
{
	none = 0x00,
	write_reg = 0x80,
	read_reg = 0x81,
	write_variable = 0x82,
	read_variable = 0x83
} dwin_commands;

// Структура, описывающая пакет данных
typedef struct packet_data_struct
{
    uint16_t header;     	// Заголовок
		uint8_t length;     	// Число байт кадра с командного байта
    dwin_commands cmd;  	// Команда
    uint8_t data[249];    // Данные
}__attribute__((packed)) packet_struct;

void dwin_write_half_word(uint16_t data, uint16_t addr);

void dwin_write_variable(char* data, uint16_t addr, uint8_t len);

void dwin_print_home_page();

void dwin_print_gasboiler_page();

void dwin_print_net_page();

void dwin_print_regs_page();

#endif /* __DWIN_H */