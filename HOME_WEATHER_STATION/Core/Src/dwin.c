#include "dwin.h"

extern UART_HandleTypeDef huart1;

void dwin_write_half_word(uint16_t addr, uint16_t data)
{
	packet_struct tx_pack;
	uint8_t tx_buf[9];
	
	tx_pack.header = HEADER;
	tx_pack.length = sizeof(addr) + sizeof(data) + 1;
	tx_pack.cmd = write_variable;
	memcpy(tx_pack.data, &addr, sizeof(addr));
	memcpy(tx_pack.data + sizeof(addr), &data, sizeof(data));

  HAL_UART_Transmit(&DWIN_UART, (uint8_t*)&tx_pack, 8, 0xFF); 
}