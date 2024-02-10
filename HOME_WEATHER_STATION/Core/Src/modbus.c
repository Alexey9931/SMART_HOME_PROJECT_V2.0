#include "modbus.h"

// Таблица для вычисления контрольной суммы
uint_least32_t crc_table[256];	
// Принятый пакет
modbus_packet rx_packet;
// Отправленный пакет
modbus_packet tx_packet;

// Функция получения пакета
void receive_packet(w5500_data* w5500_n, uint8_t sn)
{
	// Если статус текущего сокета "Соединено"
	if(get_socket_status(w5500_n, sn) == SOCK_ESTABLISHED)
	{
		w5500_n->rx_buf_len = get_size_rx(w5500_n, sn);
		
		//Если пришел пустой пакет, то уходим из функции
		if(w5500_n->rx_buf_len == 0)	return;

		// Читаем входной буфер приемника
		w5500_read_sock_buf(w5500_n, sn, get_read_pointer(w5500_n, sn), w5500_n->rx_buf, w5500_n->rx_buf_len);
		
		// Распознавание полей принятого пакета с проведением первичных проверок
		memcpy(&rx_packet.header_fields, w5500_n->rx_buf, sizeof(packet_header));
		// Проверка заголовка пакета
		if(rx_packet.header_fields.header != PACKET_HEADER) return;
		// Прием данных
		memcpy(rx_packet.data, w5500_n->rx_buf + sizeof(packet_header), rx_packet.header_fields.length - sizeof(rx_packet.end_fields.crc)
			- sizeof(rx_packet.header_fields) + sizeof(rx_packet.header_fields.header));
		// Проверка хвоста пакета
		memcpy(&rx_packet.end_fields, w5500_n->rx_buf + sizeof(packet_header) + rx_packet.header_fields.length - 
			sizeof(rx_packet.end_fields.crc) - sizeof(rx_packet.header_fields) + sizeof(rx_packet.header_fields.header), 
				sizeof(rx_packet.end_fields));
		if(rx_packet.end_fields.end != PACKET_TAIL) return;
		// Проверка контрольной суммы
		if(rx_packet.end_fields.crc != crc32((uint8_t *)&rx_packet.header_fields.recv_addr, rx_packet.header_fields.length - 
			sizeof(rx_packet.end_fields.crc))) return;

	}
}
// Функция отправки пакета
void transmit_packet(w5500_data* w5500_n, uint8_t sn)
{
		memcpy(w5500_n->tx_buf + 3, w5500_n->rx_buf, w5500_n->rx_buf_len);
		w5500_write_sock_buf(w5500_n, sn, get_write_pointer(w5500_n, sn), w5500_n->tx_buf, w5500_n->rx_buf_len);
		recv_socket(w5500_n, sn);
		send_socket(w5500_n, sn);
			
		disconnect_socket(w5500_n, sn);
			
		open_socket(w5500_n, sn, Mode_TCP);
		socket_init_wait(w5500_n, sn);
			
		listen_socket(w5500_n, sn);
		socket_listen_wait(w5500_n, sn);
		
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_2);
}
// Функция выполнения команды
void do_cmd(void)
{

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