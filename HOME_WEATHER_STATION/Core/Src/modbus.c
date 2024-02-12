#include "modbus.h"

extern eeprom_data rom_data;	//Пространство памяти ОЗУ (зеркализованное данными из ПЗУ)

// Таблица для вычисления контрольной суммы
uint_least32_t crc_table[256];	
// Принятый пакет
modbus_packet rx_packet;
// Отправленный пакет
modbus_packet tx_packet;
// Размер данных в отправляемом пакете
uint32_t tx_data_size;

// Функция, реализующая обмен данными по принципу запрос-ответ
uint8_t request_reply_iteration(w5500_data* w5500_n, uint8_t sn)
{
	if (receive_packet(w5500_n, sn) != 0) return 1;
	do_cmd();
	transmit_packet(w5500_n, sn);
	
	return 0;
}
// Функция получения пакета
uint8_t receive_packet(w5500_data* w5500_n, uint8_t sn)
{
	// Если статус текущего сокета "Соединено"
	if(get_socket_status(w5500_n, sn) == SOCK_ESTABLISHED)
	{
		w5500_n->rx_buf_len = get_size_rx(w5500_n, sn);
		
		//Если пришел пустой пакет, то уходим из функции
		if(w5500_n->rx_buf_len == 0)
		{
			return 1;
		}
		
		// Читаем входной буфер приемника
		set_read_pointer(w5500_n, sn, get_read_pointer(w5500_n, sn) + w5500_n->rx_buf_len);
		w5500_read_sock_buf(w5500_n, sn, get_read_pointer(w5500_n, sn), w5500_n->rx_buf, w5500_n->rx_buf_len);
		
		// Распознавание полей принятого пакета с проведением первичных проверок
		memcpy(&rx_packet.header_fields, w5500_n->rx_buf, sizeof(packet_header));
		// Проверка заголовка пакета
		if(rx_packet.header_fields.header != PACKET_HEADER) 
		{
			return 1;
		}
		// Прием данных
		memcpy(rx_packet.data, w5500_n->rx_buf + sizeof(packet_header), rx_packet.header_fields.length - sizeof(rx_packet.end_fields.crc)
			- sizeof(rx_packet.header_fields) + sizeof(rx_packet.header_fields.header));
		// Проверка хвоста пакета
		memcpy(&rx_packet.end_fields, w5500_n->rx_buf + sizeof(packet_header) + rx_packet.header_fields.length - 
			sizeof(rx_packet.end_fields.crc) - sizeof(rx_packet.header_fields) + sizeof(rx_packet.header_fields.header), 
				sizeof(rx_packet.end_fields));
		if(rx_packet.end_fields.end != PACKET_TAIL)
		{
			return 1;
		}
		// Проверка контрольной суммы
		uint32_t crc = crc32((uint8_t *)&rx_packet.header_fields.recv_addr, rx_packet.header_fields.length - 
			sizeof(rx_packet.end_fields.crc));
		if(rx_packet.end_fields.crc != crc)
		{
			return 1;
		}
		
		return 0;
	}
	
	return 1;
}
// Функция отправки пакета
void transmit_packet(w5500_data* w5500_n, uint8_t sn)
{
		uint8_t start_tx_buf_index = 3;
	
		tx_packet.header_fields.header = PACKET_HEADER;
		tx_packet.header_fields.recv_addr = rx_packet.header_fields.send_addr;
		tx_packet.header_fields.send_addr = rx_packet.header_fields.recv_addr;
		tx_packet.header_fields.cmd = rx_packet.header_fields.cmd;
		tx_packet.header_fields.length = tx_data_size + sizeof(packet_header) - sizeof(rx_packet.header_fields.header)
			+ sizeof(rx_packet.end_fields.crc);
	
		memcpy(w5500_n->tx_buf + start_tx_buf_index, &tx_packet.header_fields, sizeof(packet_header));
		memcpy(w5500_n->tx_buf + start_tx_buf_index + sizeof(packet_header), &tx_packet.data, tx_data_size);
	
		tx_packet.end_fields.crc = crc32(w5500_n->tx_buf + start_tx_buf_index + sizeof(tx_packet.header_fields.header), 
			tx_packet.header_fields.length - sizeof(tx_packet.end_fields.crc));
		tx_packet.end_fields.end = PACKET_TAIL;
		
		memcpy(w5500_n->tx_buf + start_tx_buf_index + sizeof(packet_header) + tx_data_size, &tx_packet.end_fields, sizeof(packet_end));
		
		w5500_n->tx_buf_len = sizeof(packet_header) + tx_data_size + sizeof(packet_end);
		
		set_write_pointer(w5500_n, sn, get_write_pointer(w5500_n, sn) + w5500_n->tx_buf_len);		
		
		w5500_write_sock_buf(w5500_n, sn, get_write_pointer(w5500_n, sn), w5500_n->tx_buf, 
			start_tx_buf_index + sizeof(packet_header) + tx_data_size + sizeof(packet_end));
			
		
		recv_socket(w5500_n, sn);
		send_socket(w5500_n, sn);
		
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_2);
}
// Функция выполнения команды
void do_cmd(void)
{
	uint16_t reg_addr;
	uint16_t reg_size;
	
	switch (rx_packet.header_fields.cmd)
	{
		case READ_CMD:
				reg_addr = *(uint16_t*)rx_packet.data;
				reg_size = *(uint16_t*)(rx_packet.data + sizeof(reg_addr));
				memcpy(&tx_packet.data, (uint8_t*)&rom_data + reg_addr, reg_size);
				tx_data_size = reg_size;
				break;
		case WRITE_CMD:
				reg_addr = *(uint16_t*)rx_packet.data;
				reg_size = *(uint16_t*)(rx_packet.data + sizeof(reg_addr));
				memcpy((uint8_t*)&rom_data + reg_addr, rx_packet.data + sizeof(reg_addr) + sizeof(reg_size), reg_size);
				memset(&tx_packet.data, WRITE_CMD, 1);
				tx_data_size = 1;
				break;
	}
}
// Функция вычисления контрольной суммы буфера по алгоритму CRC32
uint_least32_t crc32(unsigned char *buf, size_t len)
{
	uint_least32_t crc;

	crc = 0xFFFFFFFFUL;

	while (len--)
		crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

	return crc ^ 0xFFFFFFFFUL;
}
// Функция для заполнения таблицы CRC32
void fill_crc32_table(void)
{
	uint_least32_t crc; int i, j;

	for (i = 0; i < 256; i++)
	{
		crc = i;
		for (j = 0; j < 8; j++)
			crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

		crc_table[i] = crc;
	}
}