#include "bmp180.h"

// Defines according to the datsheet
short AC1 = 0;
short AC2 = 0;
short AC3 = 0;
unsigned short AC4 = 0;
unsigned short AC5 = 0;
unsigned short AC6 = 0;
short B1 = 0;
short B2 = 0;
short MB = 0;
short MC = 0;
short MD = 0;

long UT = 0;
short oss = 0;
long UP = 0;
long X1 = 0;
long X2 = 0;
long X3 = 0;
long B3 = 0;
long B5 = 0;
unsigned long B4 = 0;
long B6 = 0;
unsigned long B7 = 0;

void read_calliberation_data(I2C_HandleTypeDef* hi2c)
{
	uint8_t Callib_Data[22] = {0};
	uint16_t Callib_Start = 0xAA;
	
	HAL_I2C_Mem_Read(hi2c, BMP180_ADDRESS, Callib_Start, 1, Callib_Data,22, HAL_MAX_DELAY);

	AC1 = ((Callib_Data[0] << 8) | Callib_Data[1]);
	AC2 = ((Callib_Data[2] << 8) | Callib_Data[3]);
	AC3 = ((Callib_Data[4] << 8) | Callib_Data[5]);
	AC4 = ((Callib_Data[6] << 8) | Callib_Data[7]);
	AC5 = ((Callib_Data[8] << 8) | Callib_Data[9]);
	AC6 = ((Callib_Data[10] << 8) | Callib_Data[11]);
	B1 = ((Callib_Data[12] << 8) | Callib_Data[13]);
	B2 = ((Callib_Data[14] << 8) | Callib_Data[15]);
	MB = ((Callib_Data[16] << 8) | Callib_Data[17]);
	MC = ((Callib_Data[18] << 8) | Callib_Data[19]);
	MD = ((Callib_Data[20] << 8) | Callib_Data[21]);
}

// Get uncompensated Temp
uint16_t get_utemp(I2C_HandleTypeDef* hi2c)
{
	uint8_t datatowrite = 0x2E;
	uint8_t Temp_RAW[2] = {0};
	
	HAL_I2C_Mem_Write(hi2c, BMP180_ADDRESS, 0xF4, 1, &datatowrite, 1, 1000);
	HAL_Delay(5);  // wait 4.5 ms
	HAL_I2C_Mem_Read(hi2c, BMP180_ADDRESS, 0xF6, 1, Temp_RAW, 2, 1000);
	
	return ((Temp_RAW[0]<<8) + Temp_RAW[1]);
}

float bmp180_get_temp(I2C_HandleTypeDef* hi2c)
{
	float temp = 0;
	
	UT = get_utemp(hi2c);
	X1 = ((UT-AC6)*(AC5/(pow(2,15))));
	X2 = ((MC*(pow(2,11)))/(X1+MD));
	B5 = X1+X2;
	temp = (B5+8)/(pow(2,4));
	
	return temp/10.0;
}

// Get uncompensated Pressure
uint32_t get_upress(I2C_HandleTypeDef* hi2c, int oss)   // oversampling setting 0,1,2,3
{
	uint8_t datatowrite = 0x34+(oss<<6);
	uint8_t Press_RAW[3] = {0};
	
	HAL_I2C_Mem_Write(hi2c, BMP180_ADDRESS, 0xF4, 1, &datatowrite, 1, 1000);
	
	switch(oss)
	{
		case 0:
			HAL_Delay(5);
			break;
		case 1:
			HAL_Delay(8);
			break;
		case 2:
			HAL_Delay(14);
			break;
		case 3:
			HAL_Delay(26);
			break;
	}
	HAL_I2C_Mem_Read(hi2c, BMP180_ADDRESS, 0xF6, 1, Press_RAW, 3, 1000);
	
	return (((Press_RAW[0]<<16)+(Press_RAW[1]<<8)+Press_RAW[2]) >> (8-oss));
}

float bmp180_get_press(I2C_HandleTypeDef* hi2c, int oss)
{
	float press = 0;
	
	UP = get_upress(hi2c, oss);
	if (UP == 0) return 0.0f;
	X1 = ((UT-AC6)*(AC5/(pow(2,15))));
	X2 = ((MC*(pow(2,11)))/(X1+MD));
	B5 = X1+X2;
	B6 = B5-4000;
	X1 = (B2 * (B6*B6/(pow(2,12))))/(pow(2,11));
	X2 = AC2*B6/(pow(2,11));
	X3 = X1+X2;
	B3 = (((AC1*4+X3)<<oss)+2)/4;
	X1 = AC3*B6/pow(2,13);
	X2 = (B1 * (B6*B6/(pow(2,12))))/(pow(2,16));
	X3 = ((X1+X2)+2)/pow(2,2);
	B4 = AC4*(unsigned long)(X3+32768)/(pow(2,15));
	B7 = ((unsigned long)UP-B3)*(50000>>oss);
	if (B7<0x80000000) press = (B7*2)/B4;
	else press = (B7/B4)*2;
	X1 = (press/(pow(2,8)))*(press/(pow(2,8)));
	X1 = (X1*3038)/(pow(2,16));
	X2 = (-7357*press)/(pow(2,16));
	press = press + (X1+X2+3791)/(pow(2,4));

	return press*0.0075f;
}

void bmp180_init(I2C_HandleTypeDef* hi2c)
{
	read_calliberation_data(hi2c);
}