/**
 * API для взаимодействия с контроллером Ethernet W5500
 */
#ifndef W5500_H_
#define W5500_H_

#include "device_defs.h"

#define BUF_LEN 1024

// Структура с настройками и данными микросхемы w5500
typedef struct w5500_struct
{
  SPI_HandleTypeDef spi_n;  						// Выбранный интерфейс SPI для микросхемы w5500
	GPIO_TypeDef*			cs_eth_gpio_port;		// Настройки порта линии cs
	uint16_t					cs_eth_pin;					// Номер вывода cs
	GPIO_TypeDef*			rst_eth_gpio_port;	// Настройки порта линии rst
	uint16_t					rst_eth_pin;				// Номер вывода rst
  uint8_t 					macaddr[6];       	// MAC адрес
  uint8_t 					ipaddr[4];        	// IP адрес
  uint8_t 					ipgate[4];        	// IP адрес маршрутизатора
  uint8_t 					ipmask[4];        	// Маска подсети
  uint16_t 					local_port;      		// Порт соединения
  uint8_t 					sock_num;         	// Номер сокета
	uint8_t 					is_soc_active;			// Статус сокета (активен/не активен)
	TIM_HandleTypeDef htim;								// Таймер для ведения таймаута
	uint8_t 					rx_buf[BUF_LEN];		// Буфер приемника
	uint8_t 					tx_buf[BUF_LEN];		// Буфер передатчика
	uint32_t 					rx_buf_len;					// Длина буфера приемника
	uint32_t 					tx_buf_len;					// Длина буфера передатчика
} w5500_data;

#define BSB_COMMON  0x00
#define BSB_S0      0x01
#define BSB_S0_TX   0x02
#define BSB_S0_RX   0x03

#define RWB_WRITE 1
#define RWB_READ  0

#define OM_FDM0 0x00  //режим передачи данных переменной длины
#define OM_FDM1 0x01  //режим передачи данных по одному байту
#define OM_FDM2 0x02  //режим передачи данных по два байта
#define OM_FDM3 0x03  //режим передачи данных по четыре байта

#define MR 0x0000 //Mode Register

#define SHAR0 0x0009  //Source Hardware Address Register MSB
#define SHAR1 0x000A
#define SHAR2 0x000B
#define SHAR3 0x000C
#define SHAR4 0x000D
#define SHAR5 0x000E  // LSB
#define GWR0  0x0001  //Gateway IP Address Register MSB
#define GWR1  0x0002
#define GWR2  0x0003
#define GWR3  0x0004  // LSB
#define SUBR0 0x0005  //Subnet Mask Register MSB
#define SUBR1 0x0006
#define SUBR2 0x0007
#define SUBR3 0x0008  // LSB
#define SIPR0 0x000F  //Source IP Address Register MSB
#define SIPR1 0x0010
#define SIPR2 0x0011
#define SIPR3 0x0012  // LSB

#define Sn_PORT0 0x0004 // Socket 0 Source Port Register MSB
#define Sn_PORT1 0x0005 // Socket 0 Source Port Register LSB

#define Sn_MR 0x0000 // Socket 0 Mode Register
#define Sn_CR 0x0001 // Socket 0 Command Register
#define Sn_SR 0x0003 // Socket 0 Status Register

//Socket mode
#define Mode_CLOSED 0x00
#define Mode_TCP    0x01
#define Mode_UDP    0x02
#define Mode_MACRAV 0x04

//Socket states
#define SOCK_CLOSED       0x00
#define SOCK_INIT         0x13
#define SOCK_LISTEN       0x14
#define SOCK_ESTABLISHED  0x17

#define Sn_MSSR0    0x0012
#define Sn_MSSR1    0x0013
#define Sn_TX_FSR0  0x0020
#define Sn_TX_FSR1  0x0021
#define Sn_TX_RD0   0x0022
#define Sn_TX_RD1   0x0023
#define Sn_TX_WR0   0x0024
#define Sn_TX_WR1   0x0025
#define Sn_RX_RSR0  0x0026
#define Sn_RX_RSR1  0x0027
#define Sn_RX_RD0   0x0028
#define Sn_RX_RD1   0x0029

#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))

typedef struct data_sect 
{
  volatile uint16_t addr;
  volatile uint8_t opcode;
  uint8_t data[];
}__attribute__((packed)) data_sect_ptr;

// Функция записи байта в регистр
void w5500_write_reg(w5500_data* w5500_n, uint8_t op, uint16_t address, uint8_t data);
// Функция записи в буфер данных переменной длины
void w5500_write_buf(w5500_data* w5500_n, data_sect_ptr *datasect, uint16_t len);
// Функция записи в буфер данных переменной длины с привязкой к определенному сокету
void w5500_write_sock_buf(w5500_data* w5500_n, uint8_t sock_num, uint16_t point, uint8_t *buf, uint16_t len);
// Функция чтения байта из регистра
uint8_t w5500_read_reg(w5500_data* w5500_n, uint8_t op, uint16_t address);
// Функция чтения буфера
void w5500_read_buf(w5500_data* w5500_n, data_sect_ptr *datasect, uint16_t len);
// Функция чтения одного байта из буфера
uint8_t w5500_read_sock_buf_byte(w5500_data* w5500_n, uint8_t sock_num, uint16_t point);
// Функция чтения нескольких байт буфера 
void w5500_read_sock_buf(w5500_data* w5500_n, uint8_t sock_num, uint16_t point, uint8_t *buf, uint16_t len);
// Функция инициализации порта в сокете
void set_sock_port(w5500_data* w5500_n, uint8_t sock_num, uint16_t port);
// Функция инициализации сокета
void open_socket(w5500_data* w5500_n, uint8_t sock_num, uint16_t mode);
// Функция ожидания окончания инициализации сокета
void socket_init_wait(w5500_data* w5500_n, uint8_t sock_num);
// Функция прослушивания сокета
void listen_socket(w5500_data* w5500_n, uint8_t sock_num);
// Функция ожидания пакета по сокету
void socket_listen_wait(w5500_data* w5500_n, uint8_t sock_num);
// Функция ожидания закрытия сокета
void socket_closed_wait(w5500_data* w5500_n, uint8_t sock_num);
// Функция закрытия соединения
void disconnect_socket(w5500_data* w5500_n, uint8_t sock_num);
// Функция определения текущего состояния сокета
uint8_t get_socket_status(w5500_data* w5500_n, uint8_t sock_num);
// Функция для подготовки отправки буфера сетевому устройству
void recv_socket(w5500_data* w5500_n, uint8_t sock_num);
// Функция отправки буфера сетевому устройству
void send_socket(w5500_data* w5500_n, uint8_t sock_num);
// Функция определения размера принятых данных
uint16_t get_size_rx(w5500_data* w5500_n, uint8_t sock_num);
// Функция определения адрес данных в приемном буфере
uint16_t get_read_pointer(w5500_data* w5500_n, uint8_t sock_num);
// Функция установки адреса начала данных для чтения из буфера приемника
void set_read_pointer(w5500_data* w5500_n, uint8_t sock_num, uint16_t point);
// Функция возвращает адрес начала данных для записи в буфер отправки
uint16_t get_write_pointer(w5500_data* w5500_n, uint8_t sock_num);
// Функция установки адреса начала данных для записи в буфер отправки
void set_write_pointer(w5500_data* w5500_n, uint8_t sock_num, uint16_t point);
// Функция аппаратного сброса микросхемы
void w5500_hardware_rst(w5500_data* w5500_n);
// Функция программного сброса микросхемы
void w5500_soft_rst(w5500_data* w5500_n);
// Функция установки mac адреса микросхемы
void w5500_set_mac_addr(w5500_data* w5500_n, uint8_t macaddr[6]);
// Функция установки ip адреса маршрутизатора
void w5500_set_ip_gate_addr(w5500_data* w5500_n, uint8_t ipgate[4]);
// Функция установки маски подсети
void w5500_set_ipmask(w5500_data* w5500_n, uint8_t ipmask[4]);
// Функция установки ip адреса микросхемы
void w5500_set_ipaddr(w5500_data* w5500_n, uint8_t ipaddr[4]);
// Функция инициализации микросхемы
void w5500_ini(w5500_data* w5500_n);
// Функция установки ss
void ss_select(w5500_data* w5500_n);
// Функция сброса ss
void ss_deselect(w5500_data* w5500_n);
// Функция установки rst
void rst_select(w5500_data* w5500_n);
// Функция сброса rst
void rst_deselect(w5500_data* w5500_n);

#endif /* W5500_H_ */
