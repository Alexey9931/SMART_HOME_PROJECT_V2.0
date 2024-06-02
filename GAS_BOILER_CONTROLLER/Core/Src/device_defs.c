#include "device_defs.h"

// Сетевые настройки по умолчанию
uint8_t 	ip_addr_ini_1[4] = {192, 168, 1, 42};											//IP адрес 1-го порта по умолчанию
uint8_t 	ip_addr_ini_2[4] = {192, 168, 1, 43};											//IP адрес 2-го порта по умолчанию
uint8_t 	ip_gate_ini[4] = {192, 168, 1, 1};												//IP маршрутизатора по умолчанию
uint8_t 	ip_mask_ini[4] = {255, 255, 255, 0};											//Маскирование по умолчанию
uint8_t 	mac_addr_ini_1[6] = {0x00, 0x15, 0x42, 0xBF, 0xF0, 0x42};	//MAC адрес 1-го порта по умолчанию
uint8_t 	mac_addr_ini_2[6] = {0x00, 0x15, 0x42, 0xBF, 0xF0, 0x43};	//MAC адрес 2-го порта по умолчанию
uint32_t 	local_port_ini = 5151;																		//Порт соединения (сокета) по умолчанию

// Настройки устройства по умолчанию
float temp_setpoint = 20.0f;
float temp_range = 5.0f;
