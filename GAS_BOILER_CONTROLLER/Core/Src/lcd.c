#include "lcd.h"

extern SPI_HandleTypeDef hspi3;

void send_max7219(char rg, char dt)
{
	SS_LCD_SELECT;
	HAL_SPI_Transmit(&hspi3, (uint8_t*)&rg, sizeof(rg), 0xFFFFFFFF);
	HAL_SPI_Transmit(&hspi3, (uint8_t*)&dt, sizeof(dt), 0xFFFFFFFF);
	SS_LCD_DESELECT;
}

void max7219_init()
{
	max7219_clear();
	send_max7219(0x0F, 0x00); //Тест индикатора выключен
	HAL_Delay(10);
	send_max7219(0x0B, 7); //сколько разрядов используем
	HAL_Delay(10);
	send_max7219(0x0A, 0x04); //яркость
	HAL_Delay(10);
	send_max7219(0x0C, 1); //включим индикатор
	HAL_Delay(10);
	print_temp_max7219(0, 0, 1);
}

void max7219_clear()
{
	send_max7219(0x0F, 0x00); //Тест индикатора выключен
	send_max7219(0x0B, 7); //сколько разрядов используем
	send_max7219(0x09, 0xFF); //включим режим декодирования для всех разрядов
	send_max7219(0x0C, 1); //включим индикатор
	uint8_t i=8;
	do
	{
		send_max7219(i,0xF);//символ пустоты
	} while (--i);
}

void number_max7219(volatile long n)
                                                                                                                 {
	char ng = 0; //переменная для минуса
	if (n < 0)
	{
		ng = 1;
		n *= -1;
	}
	max7219_clear();
	if (n == 0) 
	{
		send_max7219(0x01, 0); //в первый разряд напишем 0
		return;
	}
	char i = 0;
	do {
		send_max7219(++i, n%10);
		n /= 10;
	} while (n);
	if (ng) {
		send_max7219(i+1, 0xA); //символ —
	}
}

uint8_t print_temp_max7219(int current_temp, int setpoint_temp, uint8_t is_lcd_on)
{
	if ((current_temp > 999) || (setpoint_temp > 999) || (current_temp < 0) || (setpoint_temp < 0))
	{
		return 1;
	}
		uint8_t start_pos;
	start_pos = 2;

	send_max7219(0x09, 0xEE); //включим режим декодирования для всех разрядов кроме позиций 2 и 6
	//_delay_ms(10);
	send_max7219(1, 0x4E);//вывод букв С
	send_max7219(5, 0x4E);//вывод букв С
	if (current_temp == 0)
	{
		send_max7219(start_pos, 0);
		send_max7219(start_pos+1, 0x80);
	}
	else
	{
		while (current_temp != 0)
		{
			if((start_pos == 3)||(start_pos == 7))
			{
				send_max7219(start_pos, (current_temp%10)|0x80);//вывод с точкой
			}
			else
			{
				send_max7219(start_pos, current_temp%10);
			}
			start_pos++;
			current_temp /= 10;
		}
	}
	start_pos = 6;
	if (is_lcd_on)
	{
		if (setpoint_temp == 0)
		{
			send_max7219(start_pos, 0);
			send_max7219(start_pos+1, 0x80);
		}
		else
		{
			if ((setpoint_temp < 100) && (setpoint_temp <= 10))
			{
				send_max7219(start_pos+2, 0xF);
			}
			else if (setpoint_temp < 10)
			{
				send_max7219(start_pos+1, 0x80);
				send_max7219(start_pos+2, 0xF);
			}
			while (setpoint_temp != 0)
			{
				if((start_pos == 3)||(start_pos == 7))
				{
					send_max7219(start_pos, (setpoint_temp%10)|0x80);//вывод с точкой
				}
				else
				{
					send_max7219(start_pos, setpoint_temp%10);
				}
				start_pos++;
				setpoint_temp /= 10;
			}
		}
	}
	else
	{
		send_max7219(start_pos++, 0xF);
		send_max7219(start_pos++, 0xF);
		send_max7219(start_pos, 0xF);
	}
	return 0;
}