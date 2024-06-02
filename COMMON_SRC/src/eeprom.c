#include "eeprom.h"

// Сетевые настройки по умолчанию
extern uint8_t 	ip_addr_ini_1[4];		//IP адрес 1-го порта по умолчанию
extern uint8_t 	ip_addr_ini_2[4];		//IP адрес 2-го порта по умолчанию
extern uint8_t 	ip_gate_ini[4];			//IP маршрутизатора по умолчанию
extern uint8_t 	ip_mask_ini[4];			//Маскирование по умолчанию
extern uint8_t 	mac_addr_ini_1[6];	//MAC адрес 1-го порта по умолчанию
extern uint8_t 	mac_addr_ini_2[6];	//MAC адрес 2-го порта по умолчанию
extern uint32_t local_port_ini;		  //Порт соединения (сокета) по умолчанию

// Настройки устройства по умолчанию
#ifdef _GAS_BOILER_CONTR_
extern float temp_setpoint;
extern float temp_range;	
#endif
#ifdef _CONTR_PANEL_
	
#endif
#ifdef _STREET_WEATH_ST_

#endif

void eeproms_first_ini(I2C_HandleTypeDef* hi2c)
{
	eeprom_data rom_struct;
	
	strncpy((char*)&rom_struct.common.device_name, DEVICE_NAME, sizeof(DEVICE_NAME));	
	memcpy(rom_struct.common.ip_addr_1, ip_addr_ini_1, sizeof(ip_addr_ini_1));
	memcpy(rom_struct.common.ip_addr_2, ip_addr_ini_2, sizeof(ip_addr_ini_2));
	memcpy(rom_struct.common.ip_gate, ip_gate_ini, sizeof(ip_gate_ini));
	memcpy(rom_struct.common.ip_mask, ip_mask_ini, sizeof(ip_mask_ini));
	rom_struct.common.local_port = local_port_ini;
	memcpy(rom_struct.common.mac_addr_1, mac_addr_ini_1, sizeof(mac_addr_ini_1));
	memcpy(rom_struct.common.mac_addr_2, mac_addr_ini_2, sizeof(mac_addr_ini_2));
	
#ifdef _GAS_BOILER_CONTR_
	rom_struct.unig.gas_boiler.temp_range = temp_range;
	rom_struct.unig.gas_boiler.temp_setpoint = temp_setpoint;
#endif
#ifdef _CONTR_PANEL_
	
#endif
#ifdef _STREET_WEATH_ST_

#endif
	
	for (int i = 0; i < (1+(sizeof(eeprom_data)/PAGE_SIZE)); i++)
	{
		eeprom_page_erase(&USED_I2C, 1, i);
	}
	
	eeprom_write(hi2c, 0, (uint8_t*)&rom_struct, sizeof(rom_struct));
}

uint16_t bytestowrite(uint16_t size, uint16_t offset)
{
	if ((size+offset) < PAGE_SIZE) return size;
	else return PAGE_SIZE-offset;
}

void eeprom_write(I2C_HandleTypeDef* hi2c, uint16_t addr, uint8_t *data, uint16_t size)
{
	uint16_t start_page = addr/PAGE_SIZE;
	uint16_t start_offset = addr-start_page*PAGE_SIZE;
	uint16_t data_offset;
	
	while(1)
	{
		if (size > (PAGE_SIZE - start_offset))
		{
			_eeprom_write(hi2c, 1, start_page, start_offset, data + data_offset, PAGE_SIZE - start_offset);
			size -= (PAGE_SIZE - start_offset);
			start_offset = 0;
			start_page++;
			data_offset += (PAGE_SIZE - start_offset);
		}
		else 
		{
			_eeprom_write(hi2c, 1, start_page, start_offset, data + data_offset, size);
			break;
		}
	}
}

void eeprom_read(I2C_HandleTypeDef* hi2c, uint16_t addr, uint8_t *data, uint16_t size)
{
	uint16_t start_page = addr/PAGE_SIZE;
	uint16_t start_offset = addr-start_page*PAGE_SIZE;
	uint16_t data_offset;
	
	while(1)
	{
		if (size > (PAGE_SIZE - start_offset))
		{
			_eeprom_read(hi2c, 1, start_page, start_offset, data + data_offset, PAGE_SIZE - start_offset);
			size -= (PAGE_SIZE - start_offset);
			start_offset = 0;
			start_page++;
			data_offset += (PAGE_SIZE - start_offset);
		}
		else 
		{
			_eeprom_read(hi2c, 1, start_page, start_offset, data + data_offset, size);
			break;
		}
	}
}

void _eeprom_write(I2C_HandleTypeDef* hi2c, uint8_t rom_num, uint16_t page, uint16_t offset, uint8_t *data, uint16_t size)
{
	uint8_t eeprom_addr;
	
	switch(rom_num)
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
	uint16_t endPage = page+((size+offset)/PAGE_SIZE);

	if ((size+offset)%PAGE_SIZE == 0) endPage--;
	
	uint16_t numofpages = (endPage-startPage) + 1;
	uint16_t pos = 0;

	for (int i = 0; i < numofpages; i++)
	{
		uint16_t MemAddress = startPage<<paddrposition | offset;
		uint16_t bytesremaining = bytestowrite(size, offset);

		HAL_I2C_Mem_Write(hi2c, eeprom_addr, MemAddress, 2, &data[pos], bytesremaining, 10);

		startPage++;
		offset = 0;
		size = size-bytesremaining;
		pos += bytesremaining;

		HAL_Delay(10);
	}
}

void _eeprom_read(I2C_HandleTypeDef* hi2c, uint8_t rom_num, uint16_t page, uint16_t offset, uint8_t *data, uint16_t size)
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

	if ((size+offset)%PAGE_SIZE == 0) endPage--;
	
	uint16_t numofpages = (endPage-startPage) + 1;
	uint16_t pos = 0;

	for (int i = 0; i < numofpages; i++)
	{
		uint16_t MemAddress = startPage<<paddrposition | offset;
		uint16_t bytesremaining = bytestowrite(size, offset);
		HAL_I2C_Mem_Read(hi2c, eeprom_addr, MemAddress, 2, &data[pos], bytesremaining, 10);
		startPage++;
		offset = 0;
		size = size-bytesremaining;
		pos += bytesremaining;
	}
}

void eeprom_page_erase(I2C_HandleTypeDef* hi2c, uint8_t rom_num, uint16_t page)
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
	uint16_t MemAddress = page<<paddrposition;

	uint8_t data[PAGE_SIZE];
	memset(data,0xff,PAGE_SIZE);

	HAL_I2C_Mem_Write(hi2c, eeprom_addr, MemAddress, 2, data, PAGE_SIZE, 10);

	HAL_Delay(10); 
}
