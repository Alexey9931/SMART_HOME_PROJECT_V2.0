#include "dwin.h"
#include "modbus.h"

extern UART_HandleTypeDef huart1;
extern ram_data_struct *ram_ptr;
extern network_map dev_net_map;

extern uint8_t is_time_to_update_rom; //Флаг того, что пора обновить данные eeprom
extern uint8_t is_time_to_update_systime; //Флаг того, что пора обновить системное время

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
	char tmp_str[16] = {0};
	//домашняя температура
	if (ram_ptr->uniq.control_panel.temperature < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.temperature);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.temperature);
	}
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_INDOOR_TEMP_INT_VP), 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.temperature*10.0f)%10);
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_INDOOR_TEMP_FRACT_VP), 1);
	ram_ptr->uniq.control_panel.temperature >= 0.0f ?
		dwin_write_variable("+", revert_word(HOME_PAGE_INDOOR_TEMP_SIGN_VP), 1):
		dwin_write_variable("-", revert_word(HOME_PAGE_INDOOR_TEMP_SIGN_VP), 1);
	//уличная температура
	if (ram_ptr->uniq.control_panel.str_weath_stat_data.temperature < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.str_weath_stat_data.temperature);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.str_weath_stat_data.temperature);
	}
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_OUTDOOR_TEMP_INT_VP), 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.str_weath_stat_data.temperature*10.0f)%10);
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_OUTDOOR_TEMP_FRACT_VP), 1);
	ram_ptr->uniq.control_panel.str_weath_stat_data.temperature >= 0.0f ?
		dwin_write_variable("+", revert_word(HOME_PAGE_OUTDOOR_TEMP_SIGN_VP), 1):
		dwin_write_variable("-", revert_word(HOME_PAGE_OUTDOOR_TEMP_SIGN_VP), 1);
	//домашняя влажность
	if (ram_ptr->uniq.control_panel.humidity < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.humidity);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.humidity);
	}
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_INDOOR_HUM_INT_VP), 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.humidity*10.0f)%10);
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_INDOOR_HUM_FRACT_VP), 1);
	//уличная влажность
	if (ram_ptr->uniq.control_panel.str_weath_stat_data.humidity < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.str_weath_stat_data.humidity);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.str_weath_stat_data.humidity);
	}
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_OUTDOOR_HUM_INT_VP), 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.str_weath_stat_data.humidity*10.0f)%10);
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_OUTDOOR_HUM_FRACT_VP), 1);
	//атм.давление
	if (ram_ptr->uniq.control_panel.pressure < 100.0f)
	{
		sprintf(tmp_str, "  %d", (int)ram_ptr->uniq.control_panel.pressure);
	}
	else
	{
		sprintf(tmp_str, "%d", (int)ram_ptr->uniq.control_panel.pressure);
	}
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_PRESSURE_VP), 3);
	//скорость ветра
	if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed < 10.0f)
	{
		sprintf(tmp_str, " %.1f", ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed);
	}
	else
	{
		sprintf(tmp_str, "%.1f", ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed);
	}
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_WIND_SPEED_VP), 4);
	//направление ветра
	if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.north) 
		dwin_write_variable(" N ", revert_word(HOME_PAGE_WIND_DIRECT_VP), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.northeast) 
		dwin_write_variable("N-E", revert_word(HOME_PAGE_WIND_DIRECT_VP), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.east) 
		dwin_write_variable(" E ", revert_word(HOME_PAGE_WIND_DIRECT_VP), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.southeast) 
		dwin_write_variable("S-E", revert_word(HOME_PAGE_WIND_DIRECT_VP), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.south) 
		dwin_write_variable(" S ", revert_word(HOME_PAGE_WIND_DIRECT_VP), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.southwest) 
		dwin_write_variable("S-W", revert_word(HOME_PAGE_WIND_DIRECT_VP), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.west) 
		dwin_write_variable(" W ", revert_word(HOME_PAGE_WIND_DIRECT_VP), 3);
	else if (ram_ptr->uniq.control_panel.str_weath_stat_data.wind_direct.northwest) 
		dwin_write_variable("N-W", revert_word(HOME_PAGE_WIND_DIRECT_VP), 3);
	else dwin_write_variable("   ", revert_word(HOME_PAGE_WIND_DIRECT_VP), 3);
	//время/дата
	sprintf(tmp_str, "%.2d:%.2d", ram_ptr->uniq.control_panel.sys_time.hour,
																ram_ptr->uniq.control_panel.sys_time.minutes);
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_HOURS_MINS_VP), 5);
	sprintf(tmp_str, "/%.2d", ram_ptr->uniq.control_panel.sys_time.dayofmonth);
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_MOUNTH_DAY_VP), 3);
	switch(ram_ptr->uniq.control_panel.sys_time.month)
	{
		case 1:
			dwin_write_variable("JAN", revert_word(HOME_PAGE_MOUNTH_VP), 3);
			break;
		case 2:
			dwin_write_variable("FEB", revert_word(HOME_PAGE_MOUNTH_VP), 3);
			break;
		case 3:
			dwin_write_variable("MAR", revert_word(HOME_PAGE_MOUNTH_VP), 3);
			break;
		case 4:
			dwin_write_variable("APR", revert_word(HOME_PAGE_MOUNTH_VP), 3);
			break;
		case 5:
			dwin_write_variable("MAY", revert_word(HOME_PAGE_MOUNTH_VP), 3);
			break;
		case 6:
			dwin_write_variable("JUN", revert_word(HOME_PAGE_MOUNTH_VP), 3);
			break;
		case 7:
			dwin_write_variable("JUL", revert_word(HOME_PAGE_MOUNTH_VP), 3);
			break;
		case 8:
			dwin_write_variable("AUG", revert_word(HOME_PAGE_MOUNTH_VP), 3);
			break;
		case 9:
			dwin_write_variable("SEP", revert_word(HOME_PAGE_MOUNTH_VP), 3);
			break;
		case 10:
			dwin_write_variable("OCT", revert_word(HOME_PAGE_MOUNTH_VP), 3);
			break;
		case 11:
			dwin_write_variable("NOV", revert_word(HOME_PAGE_MOUNTH_VP), 3);
			break;
		case 12:
			dwin_write_variable("DEC", revert_word(HOME_PAGE_MOUNTH_VP), 3);
			break;
	}
	switch(ram_ptr->uniq.control_panel.sys_time.dayofweek)
	{
		case 1:
			dwin_write_variable("  MONDAY ", revert_word(HOME_PAGE_WEAK_DAY_VP), 9);
			break;
		case 2:
			dwin_write_variable(" TUESDAY ", revert_word(HOME_PAGE_WEAK_DAY_VP), 9);
			break;
		case 3:
			dwin_write_variable("WEDNESDAY", revert_word(HOME_PAGE_WEAK_DAY_VP), 9);
			break;
		case 4:
			dwin_write_variable("THURSDAY ", revert_word(HOME_PAGE_WEAK_DAY_VP), 9);
			break;
		case 5:
			dwin_write_variable("  FRIDAY ", revert_word(HOME_PAGE_WEAK_DAY_VP), 9);
			break;
		case 6:
			dwin_write_variable("SATURDAY ", revert_word(HOME_PAGE_WEAK_DAY_VP), 9);
			break;
		case 7:
			dwin_write_variable("  SUNDAY ", revert_word(HOME_PAGE_WEAK_DAY_VP), 9);
			break;
	}
	//статус газового котла
	if (ram_ptr->uniq.control_panel.gas_boiler_uniq.rele_status == 1)
	{
		dwin_write_variable("ON", revert_word(HOME_PAGE_GASBOILER_STAT_VP), 3);
		dwin_write_half_word(revert_word(0x0400), revert_word(HOME_PAGE_GASBOILER_STAT_SP+3));
	}
	else
	{
		dwin_write_variable("OFF", revert_word(HOME_PAGE_GASBOILER_STAT_VP), 3);
		dwin_write_half_word(revert_word(0xF800), revert_word(HOME_PAGE_GASBOILER_STAT_SP+3));
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
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_GASBOILER_SET_INT_VP), 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint*10.0f)%10);
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_GASBOILER_SET_FRACT_VP), 1);
	//текущая температура
	if (ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature < 10.0f)
	{
		sprintf(tmp_str, " %d.", (int)ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature);
	}
	else
	{
		sprintf(tmp_str, "%d.", (int)ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature);
	}
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_GASBOILER_CUR_INT_VP), 3);
	sprintf(tmp_str, "%d", (int)(ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature*10.0f)%10);
	dwin_write_variable(tmp_str, revert_word(HOME_PAGE_GASBOILER_CUR_FRACT_VP), 1);
	
	for (uint8_t i = 0; i < (sizeof(dev_net_map.client_devs)/sizeof(dev_net_map.client_devs[0])); i++)
	{
		if (strstr((const char*)dev_net_map.client_devs[i].device_name, GAS_BOIL_NAME) != NULL) 
		{
			//статус сети газового котла
			if (dev_net_map.client_devs[i].is_inited == 1)
			{
				dwin_write_variable("LINKED", revert_word(HOME_PAGE_LINK_GASBOILER_VP), 6);
				dwin_write_half_word(revert_word(0x86B3), revert_word(HOME_PAGE_LINK_GASBOILER_SP+3));
			}
			else
			{
				dwin_write_variable("FAILED", revert_word(HOME_PAGE_LINK_GASBOILER_VP), 6);
				dwin_write_half_word(revert_word(0xF800), revert_word(HOME_PAGE_LINK_GASBOILER_SP+3));
			}
		}
		else if (strstr((const char*)dev_net_map.client_devs[i].device_name, STR_WEATH_NAME)!= NULL) 
		{
			//статус сети метеостанции
			if (dev_net_map.client_devs[i].is_inited == 1)
			{
				dwin_write_variable("LINKED", revert_word(HOME_PAGE_LINK_WEATHSTAT_VP), 6);
				dwin_write_half_word(revert_word(0x86B3), revert_word(HOME_PAGE_LINK_WEATHSTAT_SP+3));
			}
			else
			{
				dwin_write_variable("FAILED", revert_word(HOME_PAGE_LINK_WEATHSTAT_VP), 6);
				dwin_write_half_word(revert_word(0xF800), revert_word(HOME_PAGE_LINK_WEATHSTAT_SP+3));
			}
		}
		else
		{
			switch(i)
			{
				case 0:
					dwin_write_variable("FAILED", revert_word(HOME_PAGE_LINK_GASBOILER_VP), 6);
					dwin_write_half_word(revert_word(0xF800), revert_word(HOME_PAGE_LINK_GASBOILER_SP+3));
					break;
				case 1:
					dwin_write_variable("FAILED", revert_word(HOME_PAGE_LINK_WEATHSTAT_VP), 6);
					dwin_write_half_word(revert_word(0xF800), revert_word(HOME_PAGE_LINK_WEATHSTAT_SP+3));
					break;
			}
		}
	}
	//статус сети сервера
	if (dev_net_map.is_server_connected)
	{
		dwin_write_variable("LINKED", revert_word(HOME_PAGE_LINK_SERVER_VP), 6);
		dwin_write_half_word(revert_word(0x86B3), revert_word(HOME_PAGE_LINK_SERVER_SP+3));
	}
	else
	{
		dwin_write_variable("FAILED", revert_word(HOME_PAGE_LINK_SERVER_VP), 6);
		dwin_write_half_word(revert_word(0xF800), revert_word(HOME_PAGE_LINK_SERVER_SP+3));
	}
	//картинка текущей погоды
	if ((ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed == 0.0f) &&
			(ram_ptr->uniq.control_panel.str_weath_stat_data.rainfall <= 20.0f))
	{
		//sunny
		dwin_write_half_word(revert_word(HOME_PAGE_SUNNY_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+6));
		dwin_write_half_word(revert_word(HOME_PAGE_SUNNY_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+7));
		dwin_write_half_word(revert_word(HOME_PAGE_SUNNY_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+8));
	}
	else if (((ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed > 0.0f) &&
						(ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed < 7.0f)) &&
					  (ram_ptr->uniq.control_panel.str_weath_stat_data.rainfall <= 20.0f))
	{
		//sunny with clouds
		dwin_write_half_word(revert_word(HOME_PAGE_SUNNY_CLOUDS_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+6));
		dwin_write_half_word(revert_word(HOME_PAGE_SUNNY_CLOUDS_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+7));
		dwin_write_half_word(revert_word(HOME_PAGE_SUNNY_CLOUDS_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+8));
	}
	else if ((ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed >= 7.0f) &&
					 (ram_ptr->uniq.control_panel.str_weath_stat_data.rainfall <= 20.0f))
	{
		//only clouds
		dwin_write_half_word(revert_word(HOME_PAGE_CLOUDS_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+6));
		dwin_write_half_word(revert_word(HOME_PAGE_CLOUDS_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+7));
		dwin_write_half_word(revert_word(HOME_PAGE_CLOUDS_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+8));
	}
	else if (((ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed >= 0.0f) &&
						(ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed < 7.0f)) &&
					  ((ram_ptr->uniq.control_panel.str_weath_stat_data.rainfall > 20.0f)))
	{
		//small rain with sun
		dwin_write_half_word(revert_word(HOME_PAGE_SUNNY_RAIN_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+6));
		dwin_write_half_word(revert_word(HOME_PAGE_SUNNY_RAIN_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+7));
		dwin_write_half_word(revert_word(HOME_PAGE_SUNNY_RAIN_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+8));
	}
	else if ((ram_ptr->uniq.control_panel.str_weath_stat_data.wind_speed >= 7.0f) &&
					 (ram_ptr->uniq.control_panel.str_weath_stat_data.rainfall > 20.0f))
	{
		//hard rainy
		dwin_write_half_word(revert_word(HOME_PAGE_HARD_RAIN_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+6));
		dwin_write_half_word(revert_word(HOME_PAGE_HARD_RAIN_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+7));
		dwin_write_half_word(revert_word(HOME_PAGE_HARD_RAIN_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+8));
	}
	else
	{
		//sunny
		dwin_write_half_word(revert_word(HOME_PAGE_SUNNY_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+6));
		dwin_write_half_word(revert_word(HOME_PAGE_SUNNY_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+7));
		dwin_write_half_word(revert_word(HOME_PAGE_SUNNY_PICT_ID), revert_word(HOME_PAGE_WEATHER_PICTURE_VP+8));
	}
}

void dwin_print_gasboiler_page()
{
	char tmp_str[32] = {0};
	uint16_t tmp_var = 0;

	//текущая температура
	if (ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature < 10.0f)
	{
		sprintf(tmp_str, "0%.1f", ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature);
	}
	else
	{
		sprintf(tmp_str, "%.1f", ram_ptr->uniq.control_panel.gas_boiler_uniq.temperature);
	}
	dwin_write_variable(tmp_str, revert_word(GASBOILER_PAGE_CUR_TEMP_VP), 4);
	
	//уставка
	tmp_var = revert_word((uint16_t)(ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint*10));
	dwin_write_variable((char*)&tmp_var, revert_word(GASBOILER_PAGE_SET_TEMP_VP), sizeof(tmp_var));
	
	//минимальная
	tmp_var = revert_word((uint16_t)((ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint - 
			ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_range)*10));
	dwin_write_variable((char*)&tmp_var, revert_word(GASBOILER_PAGE_MIN_TEMP_VP), sizeof(tmp_var));
	
	//статус газового котла
	if (ram_ptr->uniq.control_panel.gas_boiler_uniq.rele_status == 1)
	{
		dwin_write_variable("ON", revert_word(GASBOILER_PAGE_STATUS_VP), 3);
		dwin_write_half_word(revert_word(0x0400), revert_word(GASBOILER_PAGE_STATUS_SP+3));
	}
	else
	{
		dwin_write_variable("OFF", revert_word(GASBOILER_PAGE_STATUS_VP), 3);
		dwin_write_half_word(revert_word(0xF800), revert_word(GASBOILER_PAGE_STATUS_SP+3));
	}
}

void dwin_print_net_page()
{
	char tmp_str[32] = {0};
	for (uint8_t i = 0; i < (sizeof(dev_net_map.client_devs)/sizeof(dev_net_map.client_devs[0])); i++)
	{
		if (strstr((const char*)dev_net_map.client_devs[i].device_name, GAS_BOIL_NAME) != NULL) 
		{
			//статус сети газового котла			
			if (dev_net_map.client_devs[i].is_inited == 1)
			{
				dwin_write_variable("ENABLE", revert_word(NET_PAGE_GASBOILER_STAT_VP), 7);
				dwin_write_half_word(revert_word(0x86B3), revert_word(NET_PAGE_GASBOILER_STAT_SP+3));
			}
			else
			{
				dwin_write_variable("DISABLE", revert_word(NET_PAGE_GASBOILER_STAT_VP), 7);
				dwin_write_half_word(revert_word(0xF800), revert_word(NET_PAGE_GASBOILER_STAT_SP+3));
			}
		} 
		else if (strstr((const char*)dev_net_map.client_devs[i].device_name, STR_WEATH_NAME)!= NULL) 
		{
			//статус сети метеостанции
			if (dev_net_map.client_devs[i].is_inited == 1)
			{
				dwin_write_variable("ENABLE", revert_word(NET_PAGE_WEATHSTAT_STAT_VP), 7);
				dwin_write_half_word(revert_word(0x86B3), revert_word(NET_PAGE_WEATHSTAT_STAT_SP+3));
			}
			else
			{
				dwin_write_variable("DISABLE", revert_word(NET_PAGE_WEATHSTAT_STAT_VP), 7);
				dwin_write_half_word(revert_word(0xF800), revert_word(NET_PAGE_WEATHSTAT_STAT_SP+3));
			}
		}
		else
		{
			switch(i)
			{
				case 0:
					dwin_write_variable("DISABLE", revert_word(NET_PAGE_GASBOILER_STAT_VP), 7);
					dwin_write_half_word(revert_word(0xF800), revert_word(NET_PAGE_GASBOILER_STAT_SP+3));
					break;
				case 1:
					dwin_write_variable("DISABLE", revert_word(NET_PAGE_WEATHSTAT_STAT_VP), 7);
					dwin_write_half_word(revert_word(0xF800), revert_word(NET_PAGE_WEATHSTAT_STAT_SP+3));
					break;
			}
		}
	}
	//сетевые параметры газового котла
	sprintf(tmp_str, "192.168.1.%d", dev_net_map.client_devs[0].dev_addr);
	dwin_write_variable(tmp_str, revert_word(NET_PAGE_GASBOILER_IP_VP), 13);
	//сетевые параметры метеостанции
	sprintf(tmp_str, "192.168.1.%d", dev_net_map.client_devs[1].dev_addr);
	dwin_write_variable(tmp_str, revert_word(NET_PAGE_WEATHSTAT_IP_VP), 13);
	//сетевые параметры панели управления
	sprintf(tmp_str, "192.168.1.%d", ram_ptr->common.mirrored_to_rom_regs.common.ip_addr_1[3]);
	dwin_write_variable(tmp_str, revert_word(NET_PAGE_CONTROLPANEL_IP_VP), 13);
	dwin_write_variable("ENABLE", revert_word(NET_PAGE_CONTROLPANEL_STAT_VP), 7);
	dwin_write_half_word(revert_word(0x86B3), revert_word(NET_PAGE_CONTROLPANEL_STAT_SP+3));
	//сетевые параметры роутера
	sprintf(tmp_str, "192.168.1.%d", ram_ptr->common.mirrored_to_rom_regs.common.ip_gate[3]);
	dwin_write_variable(tmp_str, revert_word(NET_PAGE_ROUTER_IP_VP), 13);
	//сетевые параметры raspberry
	sprintf(tmp_str, "192.168.1.%d", dev_net_map.serv_addr);
	dwin_write_variable(tmp_str, revert_word(NET_PAGE_SERVER_IP_VP), 13);
	if (dev_net_map.is_server_connected)
	{
		dwin_write_variable("ENABLE", revert_word(NET_PAGE_SERVER_STAT_VP), 7);
		dwin_write_half_word(revert_word(0x86B3), revert_word(NET_PAGE_SERVER_STAT_SP+3));
	}
	else
	{
		dwin_write_variable("DISABLE", revert_word(NET_PAGE_SERVER_STAT_VP), 7);
		dwin_write_half_word(revert_word(0xF800), revert_word(NET_PAGE_SERVER_STAT_SP+3));
	}
}

void dwin_print_regs_page()
{
	char tmp_str[250] = {0};
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
	dwin_write_variable(tmp_str, revert_word(REGS_PAGE_CONTRPANEL1_VP), sizeof(tmp_str));
	
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
	dwin_write_variable(tmp_str, revert_word(REGS_PAGE_CONTRPANEL2_VP), sizeof(tmp_str));
	
	memset(tmp_str, 0, sizeof(tmp_str));
	strcpy(tmp_str, "StartTime:\r\n"
									"Time:\r\n"
									"Temperature:\r\n"
									"Temp correction:\r\n"
									"Humidity:\r\n"
									"Hum correction:\r\n"
									"Pressure:");
	dwin_write_variable(tmp_str, revert_word(REGS_PAGE_CONTRPANEL3_VP), sizeof(tmp_str));
	
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
	dwin_write_variable(tmp_str, revert_word(REGS_PAGE_CONTRPANEL4_VP), sizeof(tmp_str));
	
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
	dwin_write_variable(tmp_str, revert_word(REGS_PAGE_WEATHSTAT1_VP), sizeof(tmp_str));
	
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
	dwin_write_variable(tmp_str, revert_word(REGS_PAGE_WEATHSTAT2_VP), sizeof(tmp_str));
	
	memset(tmp_str, 0, sizeof(tmp_str));
	strcpy(tmp_str, "Temperature:\r\n"
									"Temp correction:\r\n"
									"Humidity:\r\n"
									"Hum correction:\r\n"
									"Rainfall:\r\n"
									"WindSpeed:\r\n"
									"WindDirection:");
	dwin_write_variable(tmp_str, revert_word(REGS_PAGE_WEATHSTAT3_VP), sizeof(tmp_str));
	
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
	dwin_write_variable(tmp_str, revert_word(REGS_PAGE_WEATHSTAT4_VP), sizeof(tmp_str));
	
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
	dwin_write_variable(tmp_str, revert_word(REGS_PAGE_GASBOILER1_VP), sizeof(tmp_str));
	
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
	dwin_write_variable(tmp_str, revert_word(REGS_PAGE_GASBOILER2_VP), sizeof(tmp_str));
	
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
	dwin_write_variable(tmp_str, revert_word(REGS_PAGE_GASBOILER3_VP), sizeof(tmp_str));
	
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
	dwin_write_variable(tmp_str, revert_word(REGS_PAGE_GASBOILER4_VP), sizeof(tmp_str));
}

void dwin_print_settings_page()
{
	uint16_t tmp_var = 0;

	//Поправка для датчика температуры ControlPanel
	tmp_var = revert_word((int16_t)(ram_ptr->common.mirrored_to_rom_regs.common.temp_correction*10));
	dwin_write_variable((char*)&tmp_var, revert_word(SETTING_PAGE_CONTRPANEL_TEMP_VP), sizeof(tmp_var));

	//Поправка для датчика влажности ControlPanel
	tmp_var = revert_word((int16_t)(ram_ptr->common.mirrored_to_rom_regs.common.hum_correction*10));
	dwin_write_variable((char*)&tmp_var, revert_word(SETTING_PAGE_CONTRPANEL_HUM_VP), sizeof(tmp_var));

	//Поправка для датчика температуры GasboilerController
	tmp_var = revert_word((int16_t)(ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.temp_correction*10));
	dwin_write_variable((char*)&tmp_var, revert_word(SETTING_PAGE_GASBOILER_TEMP_VP), sizeof(tmp_var));

	//Поправка для датчика влажности GasboilerController
	tmp_var = revert_word((int16_t)(ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.hum_correction*10));
	dwin_write_variable((char*)&tmp_var, revert_word(SETTING_PAGE_GASBOILER_HUM_VP), sizeof(tmp_var));

	//Поправка для датчика температуры StreetWeatherStation
	tmp_var = revert_word((int16_t)(ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.temp_correction*10));
	dwin_write_variable((char*)&tmp_var, revert_word(SETTING_PAGE_WEATHSTAT_TEMP_VP), sizeof(tmp_var));

	//Поправка для датчика влажности StreetWeatherStation
	tmp_var = revert_word((int16_t)(ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.hum_correction*10));
	dwin_write_variable((char*)&tmp_var, revert_word(SETTING_PAGE_WEATHSTAT_HUM_VP), sizeof(tmp_var));
	
	//Дата/время
	tmp_var = revert_word(ram_ptr->uniq.control_panel.sys_time.hour);
	dwin_write_variable((char*)&tmp_var, revert_word(SETTING_PAGE_HOURS_VP), sizeof(tmp_var));
	
	tmp_var = revert_word(ram_ptr->uniq.control_panel.sys_time.minutes);
	dwin_write_variable((char*)&tmp_var, revert_word(SETTING_PAGE_MINS_VP), sizeof(tmp_var));
	
	tmp_var = revert_word(ram_ptr->uniq.control_panel.sys_time.dayofmonth);
	dwin_write_variable((char*)&tmp_var, revert_word(SETTING_PAGE_MOUNTHDAY_VP), sizeof(tmp_var));
	
	tmp_var = revert_word(ram_ptr->uniq.control_panel.sys_time.month);
	dwin_write_variable((char*)&tmp_var, revert_word(SETTING_PAGE_MOUNTH_VP), sizeof(tmp_var));
	
	tmp_var = revert_word(ram_ptr->uniq.control_panel.sys_time.year + 2000);
	dwin_write_variable((char*)&tmp_var, revert_word(SETTING_PAGE_YEAR_VP), sizeof(tmp_var));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint16_t dwin_buf_cnt = 1;
	if (huart == &huart1)
	{
		// Сейчас принимаем только пакеты с командой на чтения,
		// нужных нам регистров
		packet_struct rx_pack = {0};
		uint16_t addr = 0;
		int16_t reg = 0;
		memcpy(&rx_pack, dwin_rx_buf, dwin_buf_cnt);

		if ((rx_pack.length == 0) || (dwin_buf_cnt < (rx_pack.length + 3)))
			goto continue_exit;
		if ((rx_pack.header != HEADER))
			goto error_exit;

		float delta_t;
		switch((uint8_t)rx_pack.cmd)
		{
			case read_variable:
				addr = revert_word(*(uint16_t*)rx_pack.data);
				reg = revert_word(*(uint16_t*)(rx_pack.data + 3));
				switch(addr)
				{
					//Уставка температуры для GasBoilerController
					case GASBOILER_PAGE_SET_TEMP_VP:
						if (((float)reg / 10.0) < (ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint -
							ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_range))
						{
							goto error_exit;
						}
						delta_t = ((float)reg / 10.0) -
							ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint;
						ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint +=
							delta_t;
						ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_range +=
							delta_t;

						for (uint8_t i = 0; i < (sizeof(dev_net_map.client_devs)/sizeof(dev_net_map.client_devs[0])); i++)
						{
							if (strstr((const char*)dev_net_map.client_devs[i].device_name, GAS_BOIL_NAME) != NULL)
							{
								dev_net_map.client_devs[i].write_flag = 1;
								break;
							}
						}
						break;
					//Минимальная температура для GasBoilerController
					case GASBOILER_PAGE_MIN_TEMP_VP:
						if (((float)reg / 10.0) > ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint)
						{
							goto error_exit;
						}
						delta_t = ((float)reg / 10.0) -
							fabs(ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint -
								ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_range);
						ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.unig.gas_boiler.temp_range -=
							delta_t;

						for (uint8_t i = 0; i < (sizeof(dev_net_map.client_devs)/sizeof(dev_net_map.client_devs[0])); i++)
						{
							if (strstr((const char*)dev_net_map.client_devs[i].device_name, GAS_BOIL_NAME) != NULL)
							{
								dev_net_map.client_devs[i].write_flag = 1;
								break;
							}
						}
						break;
					//Поправка для датчика температуры ControlPanel
					case SETTING_PAGE_CONTRPANEL_TEMP_VP:
						ram_ptr->common.mirrored_to_rom_regs.common.temp_correction = (float)reg / 10.0;
						is_time_to_update_rom = 1;
						break;
					//Поправка для датчика влажности ControlPanel
					case SETTING_PAGE_CONTRPANEL_HUM_VP:
						ram_ptr->common.mirrored_to_rom_regs.common.hum_correction = (float)reg / 10.0;
						is_time_to_update_rom = 1;
						break;
					//Поправка для датчика температуры GasboilerController
					case SETTING_PAGE_GASBOILER_TEMP_VP:
						ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.temp_correction = (float)reg / 10.0;

						for (uint8_t i = 0; i < (sizeof(dev_net_map.client_devs)/sizeof(dev_net_map.client_devs[0])); i++)
						{
							if (strstr((const char*)dev_net_map.client_devs[i].device_name, GAS_BOIL_NAME) != NULL)
							{
								dev_net_map.client_devs[i].write_flag = 1;
								break;
							}
						}
						break;
					//Поправка для датчика влажности GasboilerController
					case SETTING_PAGE_GASBOILER_HUM_VP:
						ram_ptr->uniq.control_panel.gas_boiler_common.mirrored_to_rom_regs.common.hum_correction = (float)reg / 10.0;

						for (uint8_t i = 0; i < (sizeof(dev_net_map.client_devs)/sizeof(dev_net_map.client_devs[0])); i++)
						{
							if (strstr((const char*)dev_net_map.client_devs[i].device_name, GAS_BOIL_NAME) != NULL)
							{
								dev_net_map.client_devs[i].write_flag = 1;
								break;
							}
						}
						break;
					//Поправка для датчика температуры StreetWeatherStation
					case SETTING_PAGE_WEATHSTAT_TEMP_VP:
						ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.temp_correction = (float)reg / 10.0;

						for (uint8_t i = 0; i < (sizeof(dev_net_map.client_devs)/sizeof(dev_net_map.client_devs[0])); i++)
						{
							if (strstr((const char*)dev_net_map.client_devs[i].device_name, STR_WEATH_NAME) != NULL)
							{
								dev_net_map.client_devs[i].write_flag = 1;
								break;
							}
						}
						break;
					//Поправка для датчика влажности StreetWeatherStation
					case SETTING_PAGE_WEATHSTAT_HUM_VP:
						ram_ptr->uniq.control_panel.str_weath_stat_common.mirrored_to_rom_regs.common.hum_correction = (float)reg / 10.0;
						for (uint8_t i = 0; i < (sizeof(dev_net_map.client_devs)/sizeof(dev_net_map.client_devs[0])); i++)
						{
							if (strstr((const char*)dev_net_map.client_devs[i].device_name, STR_WEATH_NAME) != NULL)
							{
								dev_net_map.client_devs[i].write_flag = 1;
								break;
							}
						}
						break;
					//Настройки даты/времени - часы
					case SETTING_PAGE_HOURS_VP:
						ram_ptr->uniq.control_panel.sys_time.hour = reg;
						is_time_to_update_systime = 1;
						break;
					//Настройки даты/времени - минуты
					case SETTING_PAGE_MINS_VP:
						ram_ptr->uniq.control_panel.sys_time.minutes = reg;
						is_time_to_update_systime = 1;
						break;
					//Настройки даты/времени - число
					case SETTING_PAGE_MOUNTHDAY_VP:
						switch (ram_ptr->uniq.control_panel.sys_time.month)
						{
							case 2:
								if ( !(((ram_ptr->uniq.control_panel.sys_time.year % 4 == 0) && (reg > 29)) ||
										 ((ram_ptr->uniq.control_panel.sys_time.year % 4 != 0) && (reg > 28))))
								{
									ram_ptr->uniq.control_panel.sys_time.dayofmonth = reg;
									is_time_to_update_systime = 1;
								}
								break;
							case 4: case 6: case 9: case 11:
								if (reg <= 30)
								{
									ram_ptr->uniq.control_panel.sys_time.dayofmonth = reg;
									is_time_to_update_systime = 1;
								}
								break;
							default:
								ram_ptr->uniq.control_panel.sys_time.dayofmonth = reg;
								is_time_to_update_systime = 1;
								break;
						}
						break;
					//Настройки даты/времени - месяц
					case SETTING_PAGE_MOUNTH_VP:
						ram_ptr->uniq.control_panel.sys_time.month = reg;
						is_time_to_update_systime = 1;
						break;
					//Настройки даты/времени - год
					case SETTING_PAGE_YEAR_VP:
						ram_ptr->uniq.control_panel.sys_time.year = reg%100;
						is_time_to_update_systime = 1;
						break;
					default:
						goto error_exit;
				}
				break;
			default:
				goto error_exit;
		}

		error_exit:
		{
			dwin_buf_cnt = 0;
			HAL_UART_Receive_IT(&DWIN_UART, dwin_rx_buf + dwin_buf_cnt, 1);
			dwin_buf_cnt++;
			return;
		}
		continue_exit:
		{
			HAL_UART_Receive_IT(&DWIN_UART, dwin_rx_buf + dwin_buf_cnt, 1);
			dwin_buf_cnt++;
			return;
		}
	}
}
