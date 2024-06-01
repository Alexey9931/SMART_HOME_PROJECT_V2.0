#include "ram.h"

ram_data_struct ram_data;	//Пространство памяти ОЗУ (куда зеркализованы в т.ч. и данные из ПЗУ)
ram_data_struct *ram_ptr = &ram_data;	// Указатель на данные ОЗУ
