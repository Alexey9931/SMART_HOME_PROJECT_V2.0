#include "thermostat_alg.h"
#include "w5500.h"

extern ram_data_struct ram_data;	//Пространство памяти ОЗУ (куда зеркализованы в т.ч. и данные из ПЗУ)
extern ram_data_struct *ram_ptr;	// Указатель на данные ОЗУ
extern w5500_data* w5500_2_ptr;

void thermostat_task(void)
{
	// выбираем датчик для измерения температуры
	if (!ram_ptr->common.mirrored_to_rom_regs.unig.gas_boiler.temp_source)
	{
		ram_ptr->uniq.gas_boiler.current_temp = ram_ptr->uniq.gas_boiler.gasboiler_temp;
	}
	else if ((ram_ptr->common.mirrored_to_rom_regs.unig.gas_boiler.temp_source) &&
					 (w5500_2_ptr->port_set[1].is_soc_active || (w5500_2_ptr->port_set[1].sock_recon_num == 1)))
	{
		ram_ptr->uniq.gas_boiler.current_temp = ram_ptr->uniq.gas_boiler.controlpanel_temp;
	}
	else
	{
		ram_ptr->uniq.gas_boiler.current_temp = ram_ptr->uniq.gas_boiler.gasboiler_temp;
		ram_ptr->common.mirrored_to_rom_regs.unig.gas_boiler.temp_source = 0;
	}

	// если температура уставка больше температуры на DELTA_TEMP градуса, то включаем котел
	if ((ram_ptr->uniq.gas_boiler.current_temp + ram_ptr->common.mirrored_to_rom_regs.unig.gas_boiler.temp_range) 
		<= ram_ptr->common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint)
	{
		ram_ptr->uniq.gas_boiler.rele_status = 1;
		enable_rele;
	}
	// если температура больше уставки, то выключаем реле
	else if (ram_ptr->uniq.gas_boiler.current_temp > ram_ptr->common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint)
	{
		ram_ptr->uniq.gas_boiler.rele_status = 0;
		disable_rele;
	}
}