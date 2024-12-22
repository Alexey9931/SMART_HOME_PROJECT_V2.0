#include "dwin.h"
#include "modbus.h"

extern UART_HandleTypeDef huart1;
extern ram_data_struct *ram_ptr;
extern network_map dev_net_map;

uint8_t dwin_rx_buf[DWIN_BUF_SIZE]; // Буффер приемника DWIN

uint16_t revert_word(uint16_t word)
{
	return (word >> 8)|(word << 8);
}

dwin_status dwin_write_half_word(uint16_t data, uint16_t addr)
{
	packet_struct tx_pack = {0};
	tx_pack.header = HEADER;
	tx_pack.length = sizeof(addr) + sizeof(data) + 1;
	tx_pack.cmd = write_variable;
	memcpy(tx_pack.data, &addr, sizeof(addr));
	memcpy(tx_pack.data + sizeof(addr), &data, sizeof(data));

  HAL_UART_Transmit(&DWIN_UART, (uint8_t*)&tx_pack, sizeof(tx_pack) -
		sizeof(tx_pack.data) + sizeof(data) + sizeof(addr), 0xFF);
	
	packet_struct rx_pack;
//	HAL_UART_Receive(&DWIN_UART, (uint8_t*)&rx_pack, 6, 0xFF);
	
	if ((rx_pack.header != HEADER) || (rx_pack.cmd != write_variable)
		|| (rx_pack.length != 3) || (rx_pack.data[0] != 0x4F)
		|| (rx_pack.data[1] != 0x4B))
		return DWIN_ERROR;
	else
		return DWIN_OK;
}

dwin_status dwin_write_variable(char* data, uint16_t addr, uint8_t len)
{
  packet_struct tx_pack;
	tx_pack.header = HEADER;
	tx_pack.length = sizeof(addr) + len + 1;
  tx_pack.cmd = write_variable;
  memcpy(tx_pack.data, &addr, sizeof(addr));
	memcpy(tx_pack.data + sizeof(addr), data, len);

  HAL_UART_Transmit(&DWIN_UART, (uint8_t*)&tx_pack, sizeof(tx_pack) -
		sizeof(tx_pack.data) + len + sizeof(addr), 0xFF);   
	
	packet_struct rx_pack;
//	HAL_UART_Receive(&DWIN_UART, (uint8_t*)&rx_pack, 6, 0xFF);
	
	if ((rx_pack.header != HEADER) || (rx_pack.cmd != write_variable)
		|| (rx_pack.length != 3) || (rx_pack.data[0] != 0x4F)
		|| (rx_pack.data[1] != 0x4B))
		return DWIN_ERROR;
	else
		return DWIN_OK;
}
void dwin_switch_page(uint8_t id)
{
	char data[4] = {0x5A, 0x01, 0x00, id};
	dwin_write_variable(data, revert_word(0x0084), sizeof(data)/sizeof(data[0]));
}
void dwin_init()
{
	HAL_UART_Receive_IT(&DWIN_UART, dwin_rx_buf, 1);
	dwin_switch_page(1);
}
void dwin_print_home_page()
{
	char tmp_str[16];
	//домашняя температура
	if (ram_ptr->uniq.control_panel.temperature < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.temperature);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.temperature);
	}
	dwin_write_variable(tmp_str, revert_word(0x1000), 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.temperature*10.0f)%10);
	dwin_write_variable(tmp_str, revert_word(0x1060), 1);
	ram_ptr->uniq.control_panel.temperature >= 0.0f ? dwin_write_variable("+", revert_word(0x1040), 1):
		dwin_write_variable("-", revert_word(0x1040), 1);
	//уличная температура
	if (ram_ptr->uniq.control_panel.str_weath_stat_data.temperature < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.str_weath_stat_data.temperature);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.str_weath_stat_data.temperature);
	}
	dwin_write_variable(tmp_str, revert_word(0x1020), 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.str_weath_stat_data.temperature*10.0f)%10);
	dwin_write_variable(tmp_str, revert_word(0x1070), 1);
	ram_ptr->uniq.control_panel.str_weath_stat_data.temperature >= 0.0f ? dwin_write_variable("+", revert_word(0x1050), 1):
		dwin_write_variable("-", revert_word(0x1050), 1);
	//домашняя влажность
	if (ram_ptr->uniq.control_panel.humidity < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.humidity);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.humidity);
	}
	dwin_write_variable(tmp_str, revert_word(0x1010), 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.humidity*10.0f)%10);
	dwin_write_variable(tmp_str, revert_word(0x1080), 1);
	//уличная влажность
	if (ram_ptr->uniq.control_panel.str_weath_stat_data.humidity < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.str_weath_stat_data.humidity);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.str_weath_stat_data.humidity);
	}
	dwin_write_variable(tmp_str, revert_word(0x1030), 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.str_weath_stat_data.humidity*10.0f)%10);
	dwin_write_variable(tmp_str, revert_word(0x1090), 1);
	//атм.давление
	if (ram_ptr->uniq.control_panel.pressure < 100.0f)
	{
		sprintf(tmp_str, "  %d", (int)ram_ptr->uniq.control_panel.pressure);
	}
	else
	{
		sprintf(tmp_str, "%d", (int)ram_ptr->uniq.control_panel.pressure);
	}
	dwin_write_variable(tmp_str, revert_word(0x1100), 3);
	//скорость ветра
	if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed < 10.0f)
	{
		sprintf(tmp_str, " %.1f", ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed);
	}
	else
	{
		sprintf(tmp_str, "%.1f", ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed);
	}
	dwin_write_variable(tmp_str, revert_word(0x1110), 4);
	//направление ветра
	if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.north) 
		dwin_write_variable(" N ", revert_word(0x1120), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.northeast) 
		dwin_write_variable("N-E", revert_word(0x1120), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.east) 
		dwin_write_variable(" E ", revert_word(0x1120), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.southeast) 
		dwin_write_variable("S-E", revert_word(0x1120), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.south) 
		dwin_write_variable(" S ", revert_word(0x1120), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.southwest) 
		dwin_write_variable("S-W", revert_word(0x1120), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.west) 
		dwin_write_variable(" W ", revert_word(0x1120), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.northwest) 
		dwin_write_variable("N-W", revert_word(0x1120), 3);
	else dwin_write_variable("   ", revert_word(0x1120), 3);
	//время/дата
	sprintf(tmp_str, "%.2d:%.2d", ram_ptr->uniq.control_panel.sys_time.hour,
																ram_ptr->uniq.control_panel.sys_time.minutes);
	dwin_write_variable(tmp_str, revert_word(0x1200), 5);
	sprintf(tmp_str, "/%.2d", ram_ptr->uniq.control_panel.sys_time.dayofmonth);
	dwin_write_variable(tmp_str, revert_word(0x1220), 3);
	switch(ram_ptr->uniq.control_panel.sys_time.month)
	{
		case 1:
			dwin_write_variable("JAN", revert_word(0x1210), 3);
			break;
		case 2:
			dwin_write_variable("FEB", revert_word(0x1210), 3);
			break;
		case 3:
			dwin_write_variable("MAR", revert_word(0x1210), 3);
			break;
		case 4:
			dwin_write_variable("APR", revert_word(0x1210), 3);
			break;
		case 5:
			dwin_write_variable("MAY", revert_word(0x1210), 3);
			break;
		case 6:
			dwin_write_variable("JUN", revert_word(0x1210), 3);
			break;
		case 7:
			dwin_write_variable("JUL", revert_word(0x1210), 3);
			break;
		case 8:
			dwin_write_variable("AUG", revert_word(0x1210), 3);
			break;
		case 9:
			dwin_write_variable("SEP", revert_word(0x1210), 3);
			break;
		case 10:
			dwin_write_variable("OCT", revert_word(0x1210), 3);
			break;
		case 11:
			dwin_write_variable("NOV", revert_word(0x1210), 3);
			break;
		case 12:
			dwin_write_variable("DEC", revert_word(0x1210), 3);
			break;
	}
	switch(ram_ptr->uniq.control_panel.sys_time.dayofweek)
	{
		case 1:
			dwin_write_variable("  MONDAY ", revert_word(0x1190), 9);
			break;
		case 2:
			dwin_write_variable(" TUESDAY ", revert_word(0x1190), 9);
			break;
		case 3:
			dwin_write_variable("WEDNESDAY", revert_word(0x1190), 9);
			break;
		case 4:
			dwin_write_variable("THURSDAY ", revert_word(0x1190), 9);
			break;
		case 5:
			dwin_write_variable("  FRIDAY ", revert_word(0x1190), 9);
			break;
		case 6:
			dwin_write_variable("SATURDAY ", revert_word(0x1190), 9);
			break;
		case 7:
			dwin_write_variable("  SUNDAY ", revert_word(0x1190), 9);
			break;
	}
	//статус газового котла
	if (ram_ptr->uniq.control_panel.gas_boiler_uniq.rele_status == 1)
	{
		dwin_write_variable("ON", revert_word(0x1260), 3);
		dwin_write_half_word(0x0004, revert_word(0x5003));
	}
	else
	{
		dwin_write_variable("OFF", revert_word(0x1260), 3);
		dwin_write_half_word(0x00F8, revert_word(0x5003));
	}
	//уставка температуры
	if (ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint);
	}
	dwin_write_variable(tmp_str, revert_word(0x1140), 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint*10.0f)%10);
	dwin_write_variable(tmp_str, revert_word(0x1180), 1);
	//текущая температура
	if (ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature);
	}
	dwin_write_variable(tmp_str, revert_word(0x1130), 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature*10.0f)%10);
	dwin_write_variable(tmp_str, revert_word(0x1170), 1);
	
	for (uint8_t i = 0; i < (sizeof(dev_net_map.client_devs)/sizeof(dev_net_map.client_devs[0])); i++)
	{
		if (strstr((const char*)dev_net_map.client_devs[i].device_name, GAS_BOIL_NAME) != NULL) 
		{
			//статус сети газового котла
			if (dev_net_map.client_devs[i].is_inited == 1)
			{
				dwin_write_variable("LINKED", revert_word(0x1230), 6);
				dwin_write_half_word(0xB386, revert_word(0x5103));
			}
			else
			{
				dwin_write_variable("FAILED", revert_word(0x1230), 6);
				dwin_write_half_word(0x00F8, revert_word(0x5103));
			}
		}
		else if (strstr((const char*)dev_net_map.client_devs[i].device_name, STR_WEATH_NAME)!= NULL) 
		{
			//статус сети метеостанции
			if (dev_net_map.client_devs[i].is_inited == 1)
			{
				dwin_write_variable("LINKED", revert_word(0x1250), 6);
				dwin_write_half_word(0xB386, revert_word(0x5303));
			}
			else
			{
				dwin_write_variable("FAILED", revert_word(0x1250), 6);
				dwin_write_half_word(0x00F8, revert_word(0x5303));
			}
		}
		else
		{
			switch(i)
			{
				case 0:
					dwin_write_variable("FAILED", revert_word(0x1230), 6);
					dwin_write_half_word(0x00F8, revert_word(0x5103));
					break;
				case 1:
					dwin_write_variable("FAILED", revert_word(0x1250), 6);
					dwin_write_half_word(0x00F8, revert_word(0x5303));
					break;
			}
		}
	}
	//статус сети сервера
	if (dev_net_map.is_server_connected)
	{
		dwin_write_variable("LINKED", revert_word(0x1240), 6);
		dwin_write_half_word(0xB386, revert_word(0x5203));
	}
	else
	{
		dwin_write_variable("FAILED", revert_word(0x1240), 6);
		dwin_write_half_word(0x00F8, revert_word(0x5203));
	}
	//картинка прогноза погоды
	dwin_write_half_word(0x0D00, revert_word(0x5406));
	dwin_write_half_word(0x0D00, revert_word(0x5407));
	dwin_write_half_word(0x0D00, revert_word(0x5408));
}

void dwin_print_gasboiler_page()
{
	char tmp_str[32];
	uint16_t tmp_var;

	//текущая температура
	if (ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature < 10.0f)
	{
		sprintf(tmp_str, "0%.1f", ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature);
	}
	else
	{
		sprintf(tmp_str, "%.1f", ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature);
	}
	dwin_write_variable(tmp_str, revert_word(0x3040), 4);
	
	//уставка
	tmp_var = revert_word((uint16_t)(ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint*10));
	dwin_write_variable((char*)&tmp_var, revert_word(0x3000), sizeof(tmp_var));
	
	//минимальная
	tmp_var = revert_word((uint16_t)((ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint - 
			ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_range)*10));
	dwin_write_variable((char*)&tmp_var, revert_word(0x3020), sizeof(tmp_var));
	
	//статус газового котла
	if (ram_ptr->uniq.control_panel.gas_boiler_uniq.rele_status == 1)
	{
		dwin_write_variable("ON", revert_word(0x3080), 3);
		dwin_write_half_word(0x0004, revert_word(0x6003));
	}
	else
	{
		dwin_write_variable("OFF", revert_word(0x3080), 3);
		dwin_write_half_word(0x00F8, revert_word(0x6003));
	}
}

void dwin_print_net_page()
{
	char tmp_str[32];
	for (uint8_t i = 0; i < (sizeof(dev_net_map.client_devs)/sizeof(dev_net_map.client_devs[0])); i++)
	{
		if (strstr((const char*)dev_net_map.client_devs[i].device_name, GAS_BOIL_NAME) != NULL) 
		{
			//статус сети газового котла			
			if (dev_net_map.client_devs[i].is_inited == 1)
			{
				dwin_write_variable("ENABLE", revert_word(0x1480), 7);
				dwin_write_half_word(0xB386, revert_word(0x5703));
			}
			else
			{
				dwin_write_variable("DISABLE", revert_word(0x1480), 7);
				dwin_write_half_word(0x00F8, revert_word(0x5703));
			}
		} 
		else if (strstr((const char*)dev_net_map.client_devs[i].device_name, STR_WEATH_NAME)!= NULL) 
		{
			//статус сети метеостанции
			if (dev_net_map.client_devs[i].is_inited == 1)
			{
				dwin_write_variable("ENABLE", revert_word(0x1510), 7);
				dwin_write_half_word(0xB386, revert_word(0x5803));
			}
			else
			{
				dwin_write_variable("DISABLE", revert_word(0x1510), 7);
				dwin_write_half_word(0x00F8, revert_word(0x5803));
			}
		}
		else
		{
			switch(i)
			{
				case 0:
					dwin_write_variable("DISABLE", revert_word(0x1480), 7);
					dwin_write_half_word(0x00F8, revert_word(0x5703));
					break;
				case 1:
					dwin_write_variable("DISABLE", revert_word(0x1510), 7);
					dwin_write_half_word(0x00F8, revert_word(0x5803));
					break;
			}
		}
	}
	//сетевые параметры газового котла
	sprintf(tmp_str, "192.168.1.%d", dev_net_map.client_devs[0].dev_addr);
	dwin_write_variable(tmp_str, revert_word(0x1460), 13);
	//сетевые параметры метеостанции
	sprintf(tmp_str, "192.168.1.%d", dev_net_map.client_devs[1].dev_addr);
	dwin_write_variable(tmp_str, revert_word(0x1490), 13);
	//сетевые параметры панели управления
	sprintf(tmp_str, "192.168.1.%d", ram_ptr->common.mirrored_to_rom_regs.common.ip_addr_1[3]);
	dwin_write_variable(tmp_str, revert_word(0x1400), 13);
	dwin_write_variable("ENABLE", revert_word(0x1420), 7);
	dwin_write_half_word(0xB386, revert_word(0x5503));
	//сетевые параметры роутера
	sprintf(tmp_str, "192.168.1.%d", ram_ptr->common.mirrored_to_rom_regs.common.ip_gate[3]);
	dwin_write_variable(tmp_str, revert_word(0x1520), 13);
	//сетевые параметры raspberry
	sprintf(tmp_str, "192.168.1.%d", dev_net_map.serv_addr);
	dwin_write_variable(tmp_str, revert_word(0x1430), 13);
	if (dev_net_map.is_server_connected)
	{
		dwin_write_variable("ENABLE", revert_word(0x1450), 7);
		dwin_write_half_word(0xB386, revert_word(0x5603));
	}
	else
	{
		dwin_write_variable("DISABLE", revert_word(0x1450), 7);
		dwin_write_half_word(0x00F8, revert_word(0x5603));
	}
}

void dwin_print_regs_page()
{
	char tmp_str[250];
	// регистры ControlPanel
	memset(tmp_str, 0, sizeof(tmp_str));
	strcpy(tmp_str, "DeviceName:\r\n"
									"IPAddr1:\r\n"
									"IPAddr2:\r\n"
									"IPGate:\r\n"
									"IPMask:\r\n"
									"MACAddr1:\r\n"
									"MACAddr2:\r\n"
									"LocalPort1:\r\n"
									"LocalPort2:\r\n"
									"NumRxPacks:\r\n"
									"NumTxPacks:\r\n"
									"WorkTime:");
	dwin_write_variable(tmp_str, revert_word(0x1600), sizeof(tmp_str));
	
	memset(tmp_str, 0, sizeof(tmp_str));
	sprintf(tmp_str, 	"%s\r\n"
										"%d.%d.%d.%d\r\n"
									  "%d.%d.%d.%d\r\n"
										"%d.%d.%d.%d\r\n"
										"%d.%d.%d.%d\r\n"
										"%X:%X:%X:%X:%X:%X\r\n"
										"%X:%X:%X:%X:%X:%X\r\n"
										"%d\r\n"
										"%d\r\n"
										"%d\r\n"
										"%d\r\n"
										"%d",
	ram_ptr->common.mirrored_to_rom_regs.common.device_name,
	ram_ptr->common.mirrored_to_rom_regs.common.ip_addr_1[0],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_addr_1[1],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_addr_1[2],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_addr_1[3],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_addr_2[0],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_addr_2[1],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_addr_2[2],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_addr_2[3],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_gate[0],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_gate[1],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_gate[2],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_gate[3],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_mask[0],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_mask[1],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_mask[2],
	ram_ptr->common.mirrored_to_rom_regs.common.ip_mask[3],
	ram_ptr->common.mirrored_to_rom_regs.common.mac_addr_1[0],
	ram_ptr->common.mirrored_to_rom_regs.common.mac_addr_1[1],
	ram_ptr->common.mirrored_to_rom_regs.common.mac_addr_1[2],
	ram_ptr->common.mirrored_to_rom_regs.common.mac_addr_1[3],
	ram_ptr->common.mirrored_to_rom_regs.common.mac_addr_1[4],
	ram_ptr->common.mirrored_to_rom_regs.common.mac_addr_1[5],
	ram_ptr->common.mirrored_to_rom_regs.common.mac_addr_2[0],
	ram_ptr->common.mirrored_to_rom_regs.common.mac_addr_2[1],
	ram_ptr->common.mirrored_to_rom_regs.common.mac_addr_2[2],
	ram_ptr->common.mirrored_to_rom_regs.common.mac_addr_2[3],
	ram_ptr->common.mirrored_to_rom_regs.common.mac_addr_2[4],
	ram_ptr->common.mirrored_to_rom_regs.common.mac_addr_2[5],
	ram_ptr->common.mirrored_to_rom_regs.common.local_port[0],
	ram_ptr->common.mirrored_to_rom_regs.common.local_port[1],
	ram_ptr->common.num_rx_pack,
	ram_ptr->common.num_tx_pack,
	ram_ptr->common.work_time);
	dwin_write_variable(tmp_str, revert_word(0x1680), sizeof(tmp_str));
	
	memset(tmp_str, 0, sizeof(tmp_str));
	strcpy(tmp_str, "StartTime:\r\n"
									"Time:\r\n"
									"Temperature:\r\n"
									"Temp correction:\r\n"
									"Humidity:\r\n"
									"Hum correction:\r\n"
									"Pressure:");
	dwin_write_variable(tmp_str, revert_word(0x1700), sizeof(tmp_str));
	
	memset(tmp_str, 0, sizeof(tmp_str));
	sprintf(tmp_str,  "%.2d:%.2d:%.2d %.2d/%.2d/20%.2d\r\n"
										"%.2d:%.2d:%.2d %.2d/%.2d/20%.2d\r\n"
										"%.2f\r\n"
										"%.2f\r\n"
										"%.2f\r\n"
										"%.2f\r\n"
										"%.2f",
	ram_ptr->uniq.control_panel.start_time.hour,
	ram_ptr->uniq.control_panel.start_time.minutes,
	ram_ptr->uniq.control_panel.start_time.seconds,
	ram_ptr->uniq.control_panel.start_time.dayofmonth,
	ram_ptr->uniq.control_panel.start_time.month,
	ram_ptr->uniq.control_panel.start_time.year,
	ram_ptr->uniq.control_panel.sys_time.hour,
	ram_ptr->uniq.control_panel.sys_time.minutes,
	ram_ptr->uniq.control_panel.sys_time.seconds,
	ram_ptr->uniq.control_panel.sys_time.dayofmonth,
	ram_ptr->uniq.control_panel.sys_time.month,
	ram_ptr->uniq.control_panel.sys_time.year,
	ram_ptr->uniq.control_panel.temperature,
	ram_ptr->common.mirrored_to_rom_regs.common.temp_correction,
	ram_ptr->uniq.control_panel.humidity,
	ram_ptr->common.mirrored_to_rom_regs.common.hum_correction,
	ram_ptr->uniq.control_panel.pressure);
	dwin_write_variable(tmp_str, revert_word(0x1780), sizeof(tmp_str));
	
	//регистры WeatherStation
	memset(tmp_str, 0, sizeof(tmp_str));
	strcpy(tmp_str, "DeviceName:\r\n"
									"IPAddr1:\r\n"
									"IPAddr2:\r\n"
									"IPGate:\r\n"
									"IPMask:\r\n"
									"MACAddr1:\r\n"
									"MACAddr2:\r\n"
									"LocalPort1:\r\n"
									"LocalPort2:\r\n"
									"NumRxPacks:\r\n"
									"NumTxPacks:\r\n"
									"WorkTime:");
	dwin_write_variable(tmp_str, revert_word(0x2000), sizeof(tmp_str));
	
	memset(tmp_str, 0, sizeof(tmp_str));
	sprintf(tmp_str, 	"%s\r\n"
										"%d.%d.%d.%d\r\n"
									  "%d.%d.%d.%d\r\n"
										"%d.%d.%d.%d\r\n"
										"%d.%d.%d.%d\r\n"
										"%X:%X:%X:%X:%X:%X\r\n"
										"%X:%X:%X:%X:%X:%X\r\n"
										"%d\r\n"
										"%d\r\n"
										"%d\r\n"
										"%d\r\n"
										"%d",
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.device_name,
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_addr_1[0],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_addr_1[1],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_addr_1[2],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_addr_1[3],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_addr_2[0],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_addr_2[1],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_addr_2[2],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_addr_2[3],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_gate[0],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_gate[1],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_gate[2],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_gate[3],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_mask[0],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_mask[1],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_mask[2],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.ip_mask[3],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.mac_addr_1[0],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.mac_addr_1[1],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.mac_addr_1[2],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.mac_addr_1[3],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.mac_addr_1[4],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.mac_addr_1[5],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.mac_addr_2[0],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.mac_addr_2[1],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.mac_addr_2[2],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.mac_addr_2[3],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.mac_addr_2[4],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.mac_addr_2[5],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.local_port[0],
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.local_port[1],
	ram_ptr->uniq.control_panel.str_weath_stat_common.num_rx_pack,
	ram_ptr->uniq.control_panel.str_weath_stat_common.num_tx_pack,
	ram_ptr->uniq.control_panel.str_weath_stat_common.work_time);
	dwin_write_variable(tmp_str, revert_word(0x2080), sizeof(tmp_str));
	
	memset(tmp_str, 0, sizeof(tmp_str));
	strcpy(tmp_str, "Temperature:\r\n"
									"Temp correction:\r\n"
									"Humidity:\r\n"
									"Hum correction:\r\n"
									"Rainfall:\r\n"
									"WindSpeed:\r\n"
									"WindDirection:");
	dwin_write_variable(tmp_str, revert_word(0x2100), sizeof(tmp_str));
	
	memset(tmp_str, 0, sizeof(tmp_str));
	sprintf(tmp_str,  "%.2f\r\n"
										"%.2f\r\n"
										"%.2f\r\n"
										"%.2f\r\n"
										"%.2f\r\n"
										"%.2f\r\n"
										"%d",
	ram_ptr->uniq.control_panel.str_weath_stat_data.temperature,
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.temp_correction,
	ram_ptr->uniq.control_panel.str_weath_stat_data.humidity,
	ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.hum_correction,
	ram_ptr->uniq.control_panel.str_weath_stat_data.rainfall,
	ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed,
	ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct);
	dwin_write_variable(tmp_str, revert_word(0x2180), sizeof(tmp_str));
	
	// регистры GasBoilerController
	memset(tmp_str, 0, sizeof(tmp_str));
	strcpy(tmp_str, "DeviceName:\r\n"
									"IPAddr1:\r\n"
									"IPAddr2:\r\n"
									"IPGate:\r\n"
									"IPMask:\r\n"
									"MACAddr1:\r\n"
									"MACAddr2:\r\n"
									"LocalPort1:\r\n"
									"LocalPort2:\r\n"
									"NumRxPacks:\r\n"
									"NumTxPacks:\r\n"
									"WorkTime:");
	dwin_write_variable(tmp_str, revert_word(0x1800), sizeof(tmp_str));
	
	memset(tmp_str, 0, sizeof(tmp_str));
	sprintf(tmp_str, 	"%s\r\n"
										"%d.%d.%d.%d\r\n"
									  "%d.%d.%d.%d\r\n"
										"%d.%d.%d.%d\r\n"
										"%d.%d.%d.%d\r\n"
										"%X:%X:%X:%X:%X:%X\r\n"
										"%X:%X:%X:%X:%X:%X\r\n"
										"%d\r\n"
										"%d\r\n"
										"%d\r\n"
										"%d\r\n"
										"%d",
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.device_name,
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_addr_1[0],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_addr_1[1],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_addr_1[2],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_addr_1[3],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_addr_2[0],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_addr_2[1],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_addr_2[2],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_addr_2[3],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_gate[0],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_gate[1],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_gate[2],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_gate[3],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_mask[0],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_mask[1],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_mask[2],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.ip_mask[3],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.mac_addr_1[0],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.mac_addr_1[1],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.mac_addr_1[2],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.mac_addr_1[3],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.mac_addr_1[4],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.mac_addr_1[5],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.mac_addr_2[0],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.mac_addr_2[1],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.mac_addr_2[2],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.mac_addr_2[3],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.mac_addr_2[4],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.mac_addr_2[5],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.local_port[0],
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.local_port[1],
	ram_ptr->uniq.control_panel.gas_boiler_common.num_rx_pack,
	ram_ptr->uniq.control_panel.gas_boiler_common.num_tx_pack,
	ram_ptr->uniq.control_panel.gas_boiler_common.work_time);
	dwin_write_variable(tmp_str, revert_word(0x1880), sizeof(tmp_str));
	
	memset(tmp_str, 0, sizeof(tmp_str));
	strcpy(tmp_str, "TempSetpoint:\r\n"
									"TempRange:\r\n"
									"StartTime:\r\n"
									"Time:\r\n"
									"Temperature:\r\n"
									"Temp correction:\r\n"
									"Humidity:\r\n"
									"Hum correction:\r\n"
									"ReleStatus:");
	dwin_write_variable(tmp_str, revert_word(0x1900), sizeof(tmp_str));
	
	memset(tmp_str, 0, sizeof(tmp_str));
	sprintf(tmp_str,  "%.2f\r\n"
										"%.2f\r\n"
										"%.2d:%.2d:%.2d %.2d/%.2d/20%.2d\r\n"
										"%.2d:%.2d:%.2d %.2d/%.2d/20%.2d\r\n"
										"%.2f\r\n"
										"%.2f\r\n"
										"%.2f\r\n"
										"%.2f\r\n"
										"%d",
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint,
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_range,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.start_time.hour,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.start_time.minutes,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.start_time.seconds,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.start_time.dayofmonth,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.start_time.month,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.start_time.year,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.sys_time.hour,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.sys_time.minutes,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.sys_time.seconds,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.sys_time.dayofmonth,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.sys_time.month,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.sys_time.year,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature,
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.temp_correction,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.humidity,
	ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.hum_correction,
	ram_ptr->uniq.control_panel.gas_boiler_uniq.rele_status);
	dwin_write_variable(tmp_str, revert_word(0x1980), sizeof(tmp_str));
}

void dwin_print_settings_page()
{
	uint16_t tmp_var;

	//Поправка для датчика температуры ControlPanel
	tmp_var = revert_word((int16_t)(ram_ptr->common.mirrored_to_rom_regs.common.temp_correction*10));
	dwin_write_variable((char*)&tmp_var, revert_word(0x3300), sizeof(tmp_var));

	//Поправка для датчика влажности ControlPanel
	tmp_var = revert_word((int16_t)(ram_ptr->common.mirrored_to_rom_regs.common.hum_correction*10));
	dwin_write_variable((char*)&tmp_var, revert_word(0x3330), sizeof(tmp_var));

	//Поправка для датчика температуры GasboilerController
	tmp_var = revert_word((int16_t)(ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.temp_correction*10));
	dwin_write_variable((char*)&tmp_var, revert_word(0x3310), sizeof(tmp_var));

	//Поправка для датчика влажности GasboilerController
	tmp_var = revert_word((int16_t)(ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.hum_correction*10));
	dwin_write_variable((char*)&tmp_var, revert_word(0x3340), sizeof(tmp_var));

	//Поправка для датчика температуры StreetWeatherStation
	tmp_var = revert_word((int16_t)(ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.temp_correction*10));
	dwin_write_variable((char*)&tmp_var, revert_word(0x3320), sizeof(tmp_var));

	//Поправка для датчика влажности StreetWeatherStation
	tmp_var = revert_word((int16_t)(ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.hum_correction*10));
	dwin_write_variable((char*)&tmp_var, revert_word(0x3350), sizeof(tmp_var));
	
	//Дата/время
	tmp_var = revert_word(ram_ptr->uniq.control_panel.sys_time.hour);
	dwin_write_variable((char*)&tmp_var, revert_word(0x3360), sizeof(tmp_var));
	
	tmp_var = revert_word(ram_ptr->uniq.control_panel.sys_time.minutes);
	dwin_write_variable((char*)&tmp_var, revert_word(0x3370), sizeof(tmp_var));
	
	tmp_var = revert_word(ram_ptr->uniq.control_panel.sys_time.dayofmonth);
	dwin_write_variable((char*)&tmp_var, revert_word(0x3380), sizeof(tmp_var));
	
	tmp_var = revert_word(ram_ptr->uniq.control_panel.sys_time.month);
	dwin_write_variable((char*)&tmp_var, revert_word(0x3390), sizeof(tmp_var));
	
	tmp_var = revert_word(ram_ptr->uniq.control_panel.sys_time.year + 2000);
	dwin_write_variable((char*)&tmp_var, revert_word(0x3400), sizeof(tmp_var));
}