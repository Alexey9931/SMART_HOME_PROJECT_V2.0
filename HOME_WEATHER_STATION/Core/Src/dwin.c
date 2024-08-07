#include "dwin.h"
#include "modbus.h"

extern UART_HandleTypeDef huart1;
extern ram_data_struct *ram_ptr;
extern network_map dev_net_map;

void dwin_write_half_word(uint16_t data, uint16_t addr)
{
	packet_struct tx_pack = {0};
	tx_pack.header = HEADER;
	tx_pack.length = sizeof(addr) + sizeof(data) + 1;
	tx_pack.cmd = write_variable;
	memcpy(tx_pack.data, &addr, sizeof(addr));
	memcpy(tx_pack.data + sizeof(addr), &data, sizeof(data));

  HAL_UART_Transmit(&DWIN_UART, (uint8_t*)&tx_pack, sizeof(tx_pack) -
		sizeof(tx_pack.data) + sizeof(data) + sizeof(addr), 0xFF); 
}

void dwin_write_variable(char* data, uint16_t addr, uint8_t len)
{
  packet_struct tx_pack;
	tx_pack.header = HEADER;
	tx_pack.length = sizeof(addr) + len + 1;
  tx_pack.cmd = write_variable;
  memcpy(tx_pack.data, &addr, sizeof(addr));
	memcpy(tx_pack.data + sizeof(addr), data, len);

  HAL_UART_Transmit(&DWIN_UART, (uint8_t*)&tx_pack, sizeof(tx_pack) -
		sizeof(tx_pack.data) + len + sizeof(addr), 0xFF);    
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
	dwin_write_variable(tmp_str, 0x0010, 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.temperature*10.0f)%10);
	dwin_write_variable(tmp_str, 0x6010, 1);
	ram_ptr->uniq.control_panel.temperature >= 0.0f ? dwin_write_variable("+", 0x4010, 1):
		dwin_write_variable("-", 0x4010, 1);
	//уличная температура
	if (ram_ptr->uniq.control_panel.str_weath_stat_data.temperature < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.str_weath_stat_data.temperature);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.str_weath_stat_data.temperature);
	}
	dwin_write_variable(tmp_str, 0x2010, 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.str_weath_stat_data.temperature*10.0f)%10);
	dwin_write_variable(tmp_str, 0x7010, 1);
	ram_ptr->uniq.control_panel.str_weath_stat_data.temperature >= 0.0f ? dwin_write_variable("+", 0x5010, 1):
		dwin_write_variable("-", 0x5010, 1);
	//домашняя влажность
	if (ram_ptr->uniq.control_panel.humidity < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.humidity);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.humidity);
	}
	dwin_write_variable(tmp_str, 0x1010, 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.humidity*10.0f)%10);
	dwin_write_variable(tmp_str, 0x8010, 1);
	//уличная влажность
	if (ram_ptr->uniq.control_panel.str_weath_stat_data.humidity < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.str_weath_stat_data.humidity);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.str_weath_stat_data.humidity);
	}
	dwin_write_variable(tmp_str, 0x3010, 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.str_weath_stat_data.humidity*10.0f)%10);
	dwin_write_variable(tmp_str, 0x9010, 1);
	//атм.давление
	if (ram_ptr->uniq.control_panel.pressure < 100.0f)
	{
		sprintf(tmp_str, "  %d", (int)ram_ptr->uniq.control_panel.pressure);
	}
	else
	{
		sprintf(tmp_str, "%d", (int)ram_ptr->uniq.control_panel.pressure);
	}
	dwin_write_variable(tmp_str, 0x0011, 3);
	//скорость ветра
	if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed < 10.0f)
	{
		sprintf(tmp_str, " %.1f", ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed);
	}
	else
	{
		sprintf(tmp_str, "%.1f", ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed);
	}
	dwin_write_variable(tmp_str, 0x1011, 4);
	//направление ветра
	if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.north) dwin_write_variable(" N ", 0x2011, 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.northeast) dwin_write_variable("N-E", 0x2011, 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.east) dwin_write_variable(" E ", 0x2011, 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.southeast) dwin_write_variable("S-E", 0x2011, 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.south) dwin_write_variable(" S ", 0x2011, 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.southwest) dwin_write_variable("S-W", 0x2011, 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.west) dwin_write_variable(" W ", 0x2011, 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.northwest) dwin_write_variable("N-W", 0x2011, 3);
	else dwin_write_variable("   ", 0x2011, 3);
	//время/дата
	sprintf(tmp_str, "%.2d:%.2d", ram_ptr->uniq.control_panel.sys_time.hour,
																ram_ptr->uniq.control_panel.sys_time.minutes);
	dwin_write_variable(tmp_str, 0x0012, 5);
	sprintf(tmp_str, "/%.2d", ram_ptr->uniq.control_panel.sys_time.dayofmonth);
	dwin_write_variable(tmp_str, 0x2012, 3);
	switch(ram_ptr->uniq.control_panel.sys_time.month)
	{
		case 1:
			dwin_write_variable("JAN", 0x1012, 3);
			break;
		case 2:
			dwin_write_variable("FEB", 0x1012, 3);
			break;
		case 3:
			dwin_write_variable("MAR", 0x1012, 3);
			break;
		case 4:
			dwin_write_variable("APR", 0x1012, 3);
			break;
		case 5:
			dwin_write_variable("MAY", 0x1012, 3);
			break;
		case 6:
			dwin_write_variable("JUN", 0x1012, 3);
			break;
		case 7:
			dwin_write_variable("JUL", 0x1012, 3);
			break;
		case 8:
			dwin_write_variable("AUG", 0x1012, 3);
			break;
		case 9:
			dwin_write_variable("SEP", 0x1012, 3);
			break;
		case 10:
			dwin_write_variable("OCT", 0x1012, 3);
			break;
		case 11:
			dwin_write_variable("NOV", 0x1012, 3);
			break;
		case 12:
			dwin_write_variable("DEC", 0x1012, 3);
			break;
	}
	switch(ram_ptr->uniq.control_panel.sys_time.dayofweek)
	{
		case 1:
			dwin_write_variable("  MONDAY ", 0x9011, 9);
			break;
		case 2:
			dwin_write_variable(" TUESDAY ", 0x9011, 9);
			break;
		case 3:
			dwin_write_variable("WEDNESDAY", 0x9011, 9);
			break;
		case 4:
			dwin_write_variable("THURSDAY ", 0x9011, 9);
			break;
		case 5:
			dwin_write_variable("  FRIDAY ", 0x9011, 9);
			break;
		case 6:
			dwin_write_variable("SATURDAY ", 0x9011, 9);
			break;
		case 7:
			dwin_write_variable("  SUNDAY ", 0x9011, 9);
			break;
	}
	//статус газового котла
	if (ram_ptr->uniq.control_panel.gas_boiler_uniq.rele_status == 1)
	{
		dwin_write_variable("ON", 0x6012, 3);
		dwin_write_half_word(0x0004, 0x0350);
	}
	else
	{
		dwin_write_variable("OFF", 0x6012, 3);
		dwin_write_half_word(0x00F8, 0x0350);
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
	dwin_write_variable(tmp_str, 0x4011, 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint*10.0f)%10);
	dwin_write_variable(tmp_str, 0x8011, 1);
	//текущая температура
	if (ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature);
	}
	dwin_write_variable(tmp_str, 0x3011, 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature*10.0f)%10);
	dwin_write_variable(tmp_str, 0x7011, 1);
	
	for (uint8_t i = 0; i < (sizeof(dev_net_map)/sizeof(dev_net_map[0])); i++)
	{
		if (strstr((const char*)dev_net_map[i].device_name, GAS_BOIL_NAME) != NULL) 
		{
			//статус сети газового котла
			if (dev_net_map[i].is_inited == 1)
			{
				dwin_write_variable("LINKED", 0x3012, 6);
				dwin_write_half_word(0xB386, 0x0351);
			}
			else
			{
				dwin_write_variable("FAILED", 0x3012, 6);
				dwin_write_half_word(0x00F8, 0x0351);
			}
		} 
		else if (strstr((const char*)dev_net_map[i].device_name, STR_WEATH_NAME)!= NULL) 
		{
			//статус сети метеостанции
			if (dev_net_map[i].is_inited == 1)
			{
				dwin_write_variable("LINKED", 0x5012, 6);
				dwin_write_half_word(0xB386, 0x0353);
			}
			else
			{
				dwin_write_variable("FAILED", 0x5012, 6);
				dwin_write_half_word(0x00F8, 0x0350);
			}
		}
	}
	//статус сети сервера
	dwin_write_variable("FAILED", 0x4012, 6);
	dwin_write_half_word(0x00F8, 0x0352);
	//картинка прогноза погоды
	dwin_write_half_word(0x0100, 0x0654);
	dwin_write_half_word(0x0100, 0x0754);
	dwin_write_half_word(0x0100, 0x0854);
}

void dwin_print_net_page()
{
	char tmp_str[32];
	for (uint8_t i = 0; i < (sizeof(dev_net_map)/sizeof(dev_net_map[0])); i++)
	{
		if (strstr((const char*)dev_net_map[i].device_name, GAS_BOIL_NAME) != NULL) 
		{
			//сетевые параметры газового котла
			sprintf(tmp_str, "192.168.1.%d", dev_net_map[i].dev_addr);
			dwin_write_variable(tmp_str, 0x6014, 13);
			
			if (dev_net_map[i].is_inited == 1)
			{
				dwin_write_variable("ENABLE", 0x8014, 6);
				dwin_write_half_word(0xB386, 0x0357);
			}
			else
			{
				dwin_write_variable("DISABLE", 0x8014, 7);
				dwin_write_half_word(0x00F8, 0x0357);
			}
		} 
		else if (strstr((const char*)dev_net_map[i].device_name, STR_WEATH_NAME)!= NULL) 
		{
			//сетевые параметры метеостанции
			sprintf(tmp_str, "192.168.1.%d", dev_net_map[i].dev_addr);
			dwin_write_variable(tmp_str, 0x9014, 13);
			
			if (dev_net_map[i].is_inited == 1)
			{
				dwin_write_variable("ENABLE", 0x1015, 6);
				dwin_write_half_word(0xB386, 0x0358);
			}
			else
			{
				dwin_write_variable("DISABLE", 0x1015, 7);
				dwin_write_half_word(0x00F8, 0x0358);
			}
		}
	}
	//сетевые параметры панели управления
	sprintf(tmp_str, "192.168.1.%d\n", ram_ptr->common.mirrored_to_rom_regs.common.ip_addr_1[3]);
	dwin_write_variable(tmp_str, 0x0014, 13);
	dwin_write_variable("ENABLE", 0x2014, 6);
	dwin_write_half_word(0xB386, 0x0355);
	//сетевые параметры роутера
	sprintf(tmp_str, "192.168.1.%d", ram_ptr->common.mirrored_to_rom_regs.common.ip_gate[3]);
	dwin_write_variable(tmp_str, 0x3015, 13);
	//сетевые параметры raspberry
	dwin_write_variable("DISABLE", 0x5014, 7);
	dwin_write_half_word(0x00F8, 0x0356);
}

void dwin_print_regs_page()
{
	char tmp_str[250];
	
	sprintf(tmp_str, 	"DeviceName: %s\r\n"
										"IPAddr1: 	 %d.%d.%d.%d\r\n"
									  "IPAddr2: 	 %d.%d.%d.%d\r\n"
										"IPGate: 	 	 %d.%d.%d.%d\r\n"
										"IPMask: 	 	 %d.%d.%d.%d\r\n"
										"MACAddr1: 	 %X:%X:%X:%X:%X:%X\r\n"
										"MACAddr2: 	 %X:%X:%X:%X:%X:%X\r\n"
										"LocalPort1: %d\r\n"
										"LocalPort2: %d", 
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
	ram_ptr->common.mirrored_to_rom_regs.common.local_port[1]);
	dwin_write_variable(tmp_str, 0x0016, 250);
	
	sprintf(tmp_str,  "NumRxPacks: %d\r\n"
										"NumTxPacks: %d\r\n"
										"WorkTime: 	 %d\r\n"
										"StartTime:  %d:%d:%d,%d/%d/%d\r\n"
										"Time: 	 	   %d:%d:%d,%d/%d/%d\r\n"
										"Temperature:%.2f\r\n"
										"Humidity: 	 %.2f\r\n"
										"Pressure:   %.2f",
	ram_ptr->common.num_rx_pack,
	ram_ptr->common.num_tx_pack,
	ram_ptr->common.work_time,
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
	ram_ptr->uniq.control_panel.humidity,
	ram_ptr->uniq.control_panel.pressure);
}