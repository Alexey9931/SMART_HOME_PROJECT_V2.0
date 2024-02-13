/**
 * API для организации обмена данными по протоколу заданной структуры
 */
#ifndef MODBUS_H_
#define MODBUS_H_

#include "w5500.h"
#include "eeprom.h"

// Константные поля протокола
#define PACKET_HEADER 0x55
#define PACKET_TAIL 	0xAAAA

// Поддерживаемые протоколом команды
#define READ_CMD 		0x01
#define WRITE_CMD 	0x02
#define CONFIG_CMD 	0x03
#define RESET_CMD 	0x04
#define TYPE_CMD 		0x05

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

// Функция, реализующая обмен данными по принципу запрос-ответ
uint8_t request_reply_iteration(w5500_data* w5500_n, uint8_t sn);
// Функция получения пакета
uint8_t receive_packet(w5500_data* w5500_n, uint8_t sn);
// Функция отправки пакета
void transmit_packet(w5500_data* w5500_n, uint8_t sn);
// Функция выполнения команды
void do_cmd(void);
// Функция для заполнения таблицы CRC32
void fill_crc32_table(void);
// Функция вычисления контрольной суммы буфера по алгоритму CRC32
uint_least32_t crc32(unsigned char *buf, size_t len);

#endif /* MODBUS_H_ */