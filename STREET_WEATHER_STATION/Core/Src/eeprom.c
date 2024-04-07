#include "eeprom.h"

extern I2C_HandleTypeDef hi2c1;

ram_data_struct ram_data;	//Пространство памяти ОЗУ (куда зеркализованы в т.ч. и данные из ПЗУ)
ram_data_struct *ram_ptr = &ram_data;	// Указатель на данные ОЗУ

// Настройки по умолчанию
uint8_t 	ip_addr_ini[4] = {192, 168, 1, 33};		//IP адрес порта по умолчанию
uint8_t 	ip_gate_ini[4] = {192, 168, 1, 1};		//IP маршрутизатора по умолчанию
uint8_t 	ip_mask_ini[4] = {255, 255, 255, 0};		//Маскирование по умолчанию
uint8_t 	mac_addr_ini[6] = {0x00, 0x15, 0x42, 0xBF, 0xF0, 0x33};	//MAC адрес порта по умолчанию
uint32_t 	local_port_ini = 5151;		//Порт соединения (сокета) по умолчанию

void eeproms_first_ini()
{
	eeprom_data rom_struct;
	
	strncpy((char*)&rom_struct.device_name, DEVICE_NAME, sizeof(DEVICE_NAME));	
	memcpy(rom_struct.ip_addr, ip_addr_ini, sizeof(ip_addr_ini));
	memcpy(rom_struct.ip_gate, ip_gate_ini, sizeof(ip_gate_ini));
	memcpy(rom_struct.ip_mask, ip_mask_ini, sizeof(ip_mask_ini));
	rom_struct.local_port = local_port_ini;
	memcpy(rom_struct.mac_addr, mac_addr_ini, sizeof(mac_addr_ini));
	
	eeprom_page_erase(1, 0);
	eeprom_write(0, (uint8_t*)&rom_struct, sizeof(rom_struct));
}

uint16_t bytestowrite (uint16_t size, uint16_t offset)
{
	if ((size+offset)<PAGE_SIZE) return size;
	else return PAGE_SIZE-offset;
}

void eeprom_write(uint16_t addr, uint8_t *data, uint16_t size)
{
	uint16_t start_page = addr/PAGE_SIZE;
	uint16_t start_offset = addr - start_page*PAGE_SIZE;
	uint16_t data_offset;
	
	while(1)
	{
		if (size > (PAGE_SIZE - start_offset))
		{
			_eeprom_write(1, start_page, start_offset, data + data_offset, PAGE_SIZE - start_offset);
			size -= (PAGE_SIZE - start_offset);
			start_offset = 0;
			start_page++;
			data_offset += (PAGE_SIZE - start_offset);
		}
		else 
		{
			_eeprom_write(1, start_page, start_offset, data + data_offset, size);
			break;
		}
	}
}

void eeprom_read(uint16_t addr, uint8_t *data, uint16_t size)
{
	uint16_t start_page = addr/PAGE_SIZE;
	uint16_t start_offset = addr - start_page*PAGE_SIZE;
	uint16_t data_offset;
	
	while(1)
	{
		if (size > (PAGE_SIZE - start_offset))
		{
			_eeprom_read(1, start_page, start_offset, data + data_offset, PAGE_SIZE - start_offset);
			size -= (PAGE_SIZE - start_offset);
			start_offset = 0;
			start_page++;
			data_offset += (PAGE_SIZE - start_offset);
		}
		else 
		{
			_eeprom_read(1, start_page, start_offset, data + data_offset, size);
			break;
		}
	}
}

void _eeprom_write(uint8_t rom_num, uint16_t page, uint16_t offset, uint8_t *data, uint16_t size)
{
	uint8_t eeprom_addr;
	
	switch (rom_num)
	{
		case 1:
					eeprom_addr = EEPROM_ADDR_1;
					break;
		case 2:
					eeprom_addr = EEPROM_ADDR_2;
					break;
		default:
					return;
	}
	
	// Find out the number of bit, where the page addressing starts
	int paddrposition = log(PAGE_SIZE)/log(2);

	// calculate the start page and the end page
	uint16_t startPage = page;
	uint16_t endPage = page + ((size+offset)/PAGE_SIZE);

	// number of pages to be written
	uint16_t numofpages = (endPage-startPage) + 1;
	uint16_t pos=0;

	// write the data
	for (int i=0; i<numofpages; i++)
	{
		/* calculate the address of the memory location
		 * Here we add the page address with the byte address
		 */
		uint16_t MemAddress = startPage<<paddrposition | offset;
		uint16_t bytesremaining = bytestowrite(size, offset);  // calculate the remaining bytes to be written

		HAL_I2C_Mem_Write(&hi2c1, eeprom_addr, MemAddress, 2, &data[pos], bytesremaining, 1000);  // write the data to the EEPROM

		startPage += 1;  // increment the page, so that a new page address can be selected for further write
		offset=0;   // since we will be writing to a new page, so offset will be 0
		size = size-bytesremaining;  // reduce the size of the bytes
		pos += bytesremaining;  // update the position for the data buffer

		HAL_Delay (5);  // Write cycle delay (5ms)
	}
}

void _eeprom_read(uint8_t rom_num, uint16_t page, uint16_t offset, uint8_t *data, uint16_t size)
{
	uint8_t eeprom_addr;
	
	switch (rom_num)
	{
		case 1:
					eeprom_addr = EEPROM_ADDR_1;
					break;
		case 2:
					eeprom_addr = EEPROM_ADDR_2;
					break;
		default:
					return;
	}
	
	int paddrposition = log(PAGE_SIZE)/log(2);

	uint16_t startPage = page;
	uint16_t endPage = page + ((size+offset)/PAGE_SIZE);

	uint16_t numofpages = (endPage-startPage) + 1;
	uint16_t pos=0;

	for (int i=0; i<numofpages; i++)
	{
		uint16_t MemAddress = startPage<<paddrposition | offset;
		uint16_t bytesremaining = bytestowrite(size, offset);
		HAL_I2C_Mem_Read(&hi2c1, eeprom_addr, MemAddress, 2, &data[pos], bytesremaining, 1000);
		startPage += 1;
		offset=0;
		size = size-bytesremaining;
		pos += bytesremaining;
	}
}

void eeprom_page_erase(uint8_t rom_num, uint16_t page)
{
	uint8_t eeprom_addr;
	
	switch (rom_num)
	{
		case 1:
					eeprom_addr = EEPROM_ADDR_1;
					break;
		case 2:
					eeprom_addr = EEPROM_ADDR_2;
					break;
		default:
					return;
	}
	
	// calculate the memory address based on the page number
	int paddrposition = log(PAGE_SIZE)/log(2);
	uint16_t MemAddress = page<<paddrposition;

	// create a buffer to store the reset values
	uint8_t data[PAGE_SIZE];
	memset(data,0xff,PAGE_SIZE);

	// write the data to the EEPROM
	HAL_I2C_Mem_Write(&hi2c1, eeprom_addr, MemAddress, 2, data, PAGE_SIZE, 1000);

	HAL_Delay (5);  // write cycle delay 
}
