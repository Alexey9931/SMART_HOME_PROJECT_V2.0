#include "device_defs.h"

// Сетевые настройки по умолчанию
uint8_t 	ip_addr_ini_1[4] = {192, 168, 1, 22};											//IP адрес 1-го порта по умолчанию
uint8_t 	ip_addr_ini_2[4] = {192, 168, 1, 23};											//IP адрес 2-го порта по умолчанию
uint8_t 	ip_gate_ini[4] = {192, 168, 1, 1};												//IP маршрутизатора по умолчанию
uint8_t 	ip_mask_ini[4] = {255, 255, 255, 0};											//Маскирование по умолчанию
uint8_t 	mac_addr_ini_1[6] = {0x00, 0x15, 0x42, 0xBF, 0xF0, 0x22};	//MAC адрес 1-го порта по умолчанию
uint8_t 	mac_addr_ini_2[6] = {0x00, 0x15, 0x42, 0xBF, 0xF0, 0x23};	//MAC адрес 2-го порта по умолчанию
uint32_t 	local_port_ini[2] = {5151, 5152};													//Порты соединения (сокета) по умолчанию

write_cmd_entry* get_free_write_entry(client_network_map* client_dev)
{
	for(uint8_t i = 0; i < (sizeof(client_dev->write_entries)/sizeof(client_dev->write_entries[0])); i++)
	{
		if (!client_dev->write_entries[i].status)
		{
			return &client_dev->write_entries[i];
		}
	}
	return NULL;
}

write_cmd_entry* get_loaded_write_entry(client_network_map* client_dev)
{
	for(uint8_t i = 0; i < (sizeof(client_dev->write_entries)/sizeof(client_dev->write_entries[0])); i++)
	{
		if (client_dev->write_entries[i].status)
		{
			return &client_dev->write_entries[i];
		}
	}
	return NULL;
}

void set_write_entry(write_cmd_entry* entry, uint16_t	reg_addr, void* value, uint16_t value_size)
{
	entry->reg_addr = reg_addr;
	entry->value = value;
	entry->value_size = value_size;
	entry->status = 1;
}

void free_write_entry(write_cmd_entry* entry)
{
	memset(entry, 0, sizeof(write_cmd_entry));
}
