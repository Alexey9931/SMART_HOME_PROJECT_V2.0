/**
 * API для организации обмена данными по протоколу заданной структуры
 */
#ifndef MODBUS_H_
#define MODBUS_H_

#include "w5500.h"
#include "eeprom.h"
#include "device_defs.h"

// Имена устройств
#define CONTR_PAN_NAME 	"Control Panel"
#define GAS_BOIL_NAME 	"Gas Boiler Controller"
#define STR_WEATH_NAME 	"Weather Station"

// Константные поля протокола
#define PACKET_HEADER 0x55
#define PACKET_TAIL 	0xAAAA

//Device's register space
#define CONTROL_PANEL_REGS_SIZE          123
#define GAS_BOILER_CONTROLLER_REGS_SIZE  128
#define WEATH_STATION_REGS_SIZE          128

// Поддерживаемые протоколом команды
typedef enum
{
	read_cmd = 0x01,
	write_cmd = 0x02,
	config_cmd = 0x03,
	reset_cmd = 0x04,
	type_cmd = 0x05,
	init_cmd = 0x06
} modbus_commands;

// Структура, описывающая заголовочные поля пакета 
typedef struct packet_header_struct
{
    uint8_t header;         // Заголовок
    uint16_t recv_addr;     // Адрес получателя
    uint16_t send_addr;     // Адрес отправителя
    uint16_t length;        // Длина пакета
    uint8_t cmd;            // Команда
}__attribute__((packed)) packet_header;
// Структура, описывающая поля хвоста пакета
typedef struct packet_end_struct
{
    uint32_t  crc;  // Контрольная сумма
    uint16_t  end;  // Конец пакета
}__attribute__((packed)) packet_end;
// Структура, описывающая структуру пакета заданного протокола
typedef struct modbus_struct
{
    packet_header   header_fields;  // Заголовочные поля
    uint8_t         data[BUF_LEN];  // Данные
    packet_end      end_fields;     // Поля конца пакета
}__attribute__((packed)) modbus_packet;

// Серверная функция, обеспечивающая обмен данными
uint8_t reply_iteration(w5500_data* w5500_n, uint8_t sn);
// Функция проверки соединения
void check_sock_connection(w5500_data* w5500_n, port_settings* port);
// Функция для заполнения таблицы CRC32
void fill_crc32_table(void);
// Функция вычисления контрольной суммы буфера по алгоритму CRC32
uint_least32_t crc32(unsigned char *buf, size_t len);

// Клиентское API (только для ControlPanel)
#ifdef _CONTR_PANEL_
// Клиентская функция для выполнения команды "TYPE"
uint8_t do_type_cmd(w5500_data* w5500_n, uint8_t sn, client_network_map* device);
// Клиентская функция для выполнения команды "WRITE"
uint8_t do_write_cmd(w5500_data* w5500_n, uint8_t sn, client_network_map* device, uint16_t reg_addr, void* value, uint16_t value_size);
// Клиентская функция для выполнения команды "CONFIG"
uint8_t do_config_cmd(w5500_data* w5500_n, uint8_t sn, client_network_map* device, uint16_t reg_addr, void* value, uint16_t value_size);
// Клиентская функция для выполнения команды "READ"
uint8_t do_read_cmd(w5500_data* w5500_n, uint8_t sn, client_network_map* device, uint16_t reg_addr, uint16_t value_size);
#endif

#endif /* MODBUS_H_ */
