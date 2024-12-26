#include "modbus.h"
#include "ram.h"

extern ram_data_struct ram_data;	// Пространство памяти ОЗУ (куда зеркализованы в т.ч. и данные из ПЗУ)
extern ram_data_struct *ram_ptr;	// Указатель на данные ОЗУ

// Таблица для вычисления контрольной суммы
uint_least32_t crc_table[256];	
// Принятый пакет
modbus_packet rx_packet;
// Отправленный пакет
modbus_packet tx_packet;
// Размер данных в отправляемом пакете
uint32_t tx_data_size;

// Серверная функция, обеспечивающая обмен данными
uint8_t reply_iteration(w5500_data* w5500_n, uint8_t sn)
{
	if (receive_packet(w5500_n, sn) != 0) return 1;
	do_cmd();
	transmit_packet(w5500_n, sn);
	__HAL_TIM_SET_COUNTER(w5500_n->port_set[sn].htim, 0);
	w5500_n->port_set[sn].is_soc_active = 1;
	
	return 0;
}
// Клиентская функция, инициирующая обмен данными
uint8_t request_iteration(w5500_data* w5500_n, uint8_t sn, uint8_t *dev_name, uint8_t dev_addr, uint8_t cmd)
{
	uint16_t rw_size;
	void *val_ptr;
	if (strstr((const char*)dev_name, GAS_BOIL_NAME) != NULL) 
	{
		rw_size = GAS_BOILER_CONTROLLER_REGS_SIZE;
		val_ptr = (void*)&ram_ptr->uniq.control_panel.gas_boiler_common;
  } 
	else if (strstr((const char*)dev_name, STR_WEATH_NAME)!= NULL) 
	{
		rw_size = WEATH_STATION_REGS_SIZE;
		val_ptr = (void*)&ram_ptr->uniq.control_panel.str_weath_stat_common;
  }
	
	switch(cmd)
	{
		case type_cmd:
				if (do_type_cmd(w5500_n, dev_addr, sn) != 0)
				{
					w5500_n->port_set[sn].is_soc_active = 0;
					return 1;
				}
				break;
		case read_cmd:
				if (do_read_cmd(w5500_n, dev_addr, sn, 0, rw_size) != 0)
				{
					w5500_n->port_set[sn].is_soc_active = 0;
					return 1;
				}
				break;
		case write_cmd:
				if (do_write_cmd(w5500_n, dev_addr, sn, 0, val_ptr, rw_size) != 0)
				{
					w5500_n->port_set[sn].is_soc_active = 0;
					return 1;
				}
				break;
		case config_cmd:
				if (do_config_cmd(w5500_n, dev_addr, sn, 0, val_ptr, sizeof(eeprom_data)) != 0)
				{
					w5500_n->port_set[sn].is_soc_active = 0;
					return 1;
				}
				break;
		default:
				return 1;
	}
	__HAL_TIM_SET_COUNTER(w5500_n->port_set[sn].htim, 0);
	w5500_n->port_set[sn].is_soc_active = 1;
	while (1)
	{
		if (!receive_packet(w5500_n, sn)) 
		{
			__HAL_TIM_SET_COUNTER(w5500_n->port_set[sn].htim, 0);
			w5500_n->port_set[sn].is_soc_active = 1;
			return 0;
		}
		else if (!w5500_n->port_set[sn].is_soc_active) 
			return 1;
	}
}
// Функция получения пакета-запроса
uint8_t receive_packet(w5500_data* w5500_n, uint8_t sn)
{
	// Если статус текущего сокета "Соединено"
	if(get_socket_status(w5500_n, sn) == SOCK_ESTABLISHED)
	{
		w5500_n->rx_buf_len = get_size_rx(w5500_n, sn);
		if (w5500_n->rx_buf_len > DEF_SOC_BUF_SIZE)
		{
			set_read_pointer(w5500_n, sn, 0);
			return 1;
		}
		
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

		// Проверка адреса назначения
		if (rx_packet.header_fields.recv_addr != w5500_n->ipaddr[3])
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
		
		ram_ptr->common.num_rx_pack++;
		
		return 0;
	}
	
	return 1;
}
// Функция отправки пакета-ответа
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
		
		//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_2);
}
// Функция выполнения команды по запросу
void do_cmd(void)
{
	ram_ptr->common.num_tx_pack++;
	
	uint16_t reg_addr;
	uint16_t reg_size;
	
	switch (rx_packet.header_fields.cmd)
	{
		case read_cmd:
				reg_addr = *(uint16_t*)rx_packet.data;
				reg_size = *(uint16_t*)(rx_packet.data + sizeof(reg_addr));
				memcpy(&tx_packet.data, (uint8_t*)&ram_data + reg_addr, reg_size);
				tx_data_size = reg_size;
				break;
		case write_cmd:
				reg_addr = *(uint16_t*)rx_packet.data;
				reg_size = *(uint16_t*)(rx_packet.data + sizeof(reg_addr));
				memcpy((uint8_t*)&ram_data + reg_addr, rx_packet.data + sizeof(reg_addr) + sizeof(reg_size), reg_size);
				memset(&tx_packet.data, write_cmd, 1);
				tx_data_size = 1;
				break;
		case config_cmd:
				reg_addr = *(uint16_t*)rx_packet.data;
				reg_size = *(uint16_t*)(rx_packet.data + sizeof(reg_addr));
				memset(&tx_packet.data, config_cmd, 1);
				tx_data_size = 1;
				eeprom_write(&USED_I2C, reg_addr, rx_packet.data + sizeof(reg_addr) + sizeof(reg_size), reg_size);
				break;
		case reset_cmd:
				eeproms_first_ini(&USED_I2C);
				memset(&tx_packet.data, reset_cmd, 1);
				tx_data_size = 1;
				break;
		case type_cmd:
				memcpy((char*)&tx_packet.data, (char*)&ram_data.common.mirrored_to_rom_regs.common.device_name, 
					sizeof(ram_data.common.mirrored_to_rom_regs.common.device_name));
				tx_data_size = sizeof(ram_data.common.mirrored_to_rom_regs.common.device_name);
				break;
		case init_cmd:
				memcpy((char*)&tx_packet.data, (char*)&ram_data, sizeof(ram_data));
				tx_data_size = sizeof(ram_data);
				break;
		default:
				break;
	}
}
// Функция отправки команды read
uint8_t do_read_cmd(w5500_data* w5500_n, uint8_t dev_addr, uint8_t sn, uint16_t reg_addr, uint16_t value_size)
{
	uint8_t start_tx_buf_index = 3;
	// Если статус текущего сокета "Соединено"
	if(get_socket_status(w5500_n, sn) == SOCK_ESTABLISHED)
	{	
		set_write_pointer(w5500_n, sn, 0);
		
		tx_packet.header_fields.header = PACKET_HEADER;
		tx_packet.header_fields.recv_addr = dev_addr;
		tx_packet.header_fields.send_addr = w5500_n->ipaddr[3];
		tx_packet.header_fields.cmd = read_cmd;
		tx_data_size = sizeof(reg_addr) + sizeof(value_size);
		tx_packet.header_fields.length = tx_data_size + sizeof(packet_header) - sizeof(rx_packet.header_fields.header)
				+ sizeof(rx_packet.end_fields.crc);
		memcpy((char*)&tx_packet.data, &reg_addr, sizeof(reg_addr));
		memcpy((char*)&tx_packet.data + sizeof(reg_addr), &value_size, sizeof(value_size));
		
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
		
		ram_ptr->common.num_tx_pack++;
		
		return 0;
	}
	return 1;
}
// Функция отправки команды write
uint8_t do_write_cmd(w5500_data* w5500_n, uint8_t dev_addr, uint8_t sn, uint16_t reg_addr, void* value, uint16_t value_size)
{
	uint8_t start_tx_buf_index = 3;
	// Если статус текущего сокета "Соединено"
	if(get_socket_status(w5500_n, sn) == SOCK_ESTABLISHED)
	{	
		set_write_pointer(w5500_n, sn, 0);
		
		tx_packet.header_fields.header = PACKET_HEADER;
		tx_packet.header_fields.recv_addr = dev_addr;
		tx_packet.header_fields.send_addr = w5500_n->ipaddr[3];
		tx_packet.header_fields.cmd = write_cmd;
		tx_data_size = value_size + sizeof(reg_addr) + sizeof(value_size);
		tx_packet.header_fields.length = tx_data_size + sizeof(packet_header) - sizeof(rx_packet.header_fields.header)
				+ sizeof(rx_packet.end_fields.crc);
		memcpy((char*)&tx_packet.data, &reg_addr, sizeof(reg_addr));
		memcpy((char*)&tx_packet.data + sizeof(reg_addr), &value_size, sizeof(value_size));
		memcpy((char*)&tx_packet.data + sizeof(reg_addr) + sizeof(value_size), value, value_size);
		
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
		
		ram_ptr->common.num_tx_pack++;
		
		return 0;
	}
	return 1;
}
// Функция отправки команды type
uint8_t do_type_cmd(w5500_data* w5500_n, uint8_t dev_addr, uint8_t sn)
{
	uint8_t start_tx_buf_index = 3;
	// Если статус текущего сокета "Соединено"
	if(get_socket_status(w5500_n, sn) == SOCK_ESTABLISHED)
	{	
		set_write_pointer(w5500_n, sn, 0);
		
		tx_packet.header_fields.header = PACKET_HEADER;
		tx_packet.header_fields.recv_addr = dev_addr;
		tx_packet.header_fields.send_addr = w5500_n->ipaddr[3];
		tx_packet.header_fields.cmd = type_cmd;
		tx_data_size = 1;
		tx_packet.header_fields.length = tx_data_size + sizeof(packet_header) - sizeof(rx_packet.header_fields.header)
				+ sizeof(rx_packet.end_fields.crc);
		memset((char*)&tx_packet.data, type_cmd, 1);
		
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
		
		ram_ptr->common.num_tx_pack++;
		
		return 0;
	}
	return 1;
}
// Функция отправки команды config
uint8_t do_config_cmd(w5500_data* w5500_n, uint8_t dev_addr, uint8_t sn, uint16_t reg_addr, void* value, uint16_t value_size)
{
	uint8_t start_tx_buf_index = 3;
	// Если статус текущего сокета "Соединено"
	if(get_socket_status(w5500_n, sn) == SOCK_ESTABLISHED)
	{	
		set_write_pointer(w5500_n, sn, 0);
		
		tx_packet.header_fields.header = PACKET_HEADER;
		tx_packet.header_fields.recv_addr = dev_addr;
		tx_packet.header_fields.send_addr = w5500_n->ipaddr[3];
		tx_packet.header_fields.cmd = config_cmd;
		tx_data_size = value_size + sizeof(reg_addr) + sizeof(value_size);
		tx_packet.header_fields.length = tx_data_size + sizeof(packet_header) - sizeof(rx_packet.header_fields.header)
				+ sizeof(rx_packet.end_fields.crc);
		
		memcpy((char*)&tx_packet.data, &reg_addr, sizeof(reg_addr));
		memcpy((char*)&tx_packet.data + sizeof(reg_addr), &value_size, sizeof(value_size));
		memcpy((char*)&tx_packet.data + sizeof(reg_addr) + sizeof(value_size), value, value_size);
		
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
		
		ram_ptr->common.num_tx_pack++;
		
		return 0;
	}
	return 1;
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
