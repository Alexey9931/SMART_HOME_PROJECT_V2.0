#ifndef __DWIN_H
#define __DWIN_H

#include "stm32f4xx_hal.h"
#include "ram.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define DWIN_UART huart1
#define DWIN_BUF_SIZE 128

#define HEADER 0xA55A

// Описание ошибок DWIN
typedef enum
{
	DWIN_OK,
	DWIN_ERROR
} dwin_status;

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
    uint16_t header;							// Заголовок
		uint8_t length;     					// Число байт кадра с командного байта
    dwin_commands cmd;  					// Команда
    uint8_t data[249];	// Данные
}__attribute__((packed)) packet_struct;

uint16_t revert_word(uint16_t word);

dwin_status dwin_write_half_word(uint16_t data, uint16_t addr);

dwin_status dwin_write_variable(char* data, uint16_t addr, uint8_t len);

void dwin_init();

void dwin_print_home_page();

void dwin_print_gasboiler_page();

void dwin_print_net_page();

void dwin_print_regs_page();

#endif /* __DWIN_H */