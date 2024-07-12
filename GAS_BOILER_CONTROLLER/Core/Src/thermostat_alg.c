#include "thermostat_alg.h"

extern ram_data_struct ram_data;	//Пространство памяти ОЗУ (куда зеркализованы в т.ч. и данные из ПЗУ)
extern ram_data_struct *ram_ptr;	// Указатель на данные ОЗУ

void thermostat_task(void)
{
	// если температура уставка больше температуры на DELTA_TEMP градуса, то включаем котел
	if ((ram_ptr->uniq.gas_boiler.temperature + ram_ptr->common.mirrored_to_rom_regs.unig.gas_boiler.temp_range) 
		<= ram_ptr->common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint)
	{
		ram_ptr->uniq.gas_boiler.rele_status = 1;
		enable_rele;
	}
	// если температура больше уставки, то выключаем реле
	else if (ram_ptr->uniq.gas_boiler.temperature > ram_ptr->common.mirrored_to_rom_regs.unig.gas_boiler.temp_setpoint)
	{
		ram_ptr->uniq.gas_boiler.rele_status = 0;
		disable_rele;
	}
}