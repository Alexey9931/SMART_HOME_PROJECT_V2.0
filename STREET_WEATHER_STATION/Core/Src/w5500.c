#include "w5500.h"

extern SPI_HandleTypeDef hspi1;

uint8_t macaddr[6] = MAC_ADDR;
extern uint8_t ipaddr[4];
extern uint8_t ipgate[4];
extern uint8_t ipmask[4];
extern uint16_t local_port;

// Функция записи байта в регистр
void w5500_write_reg(uint8_t op, uint16_t address, uint8_t data)
{
  uint8_t buf[] = {address >> 8, address, op|(RWB_WRITE<<2), data};
  SS_SELECT();
  HAL_SPI_Transmit(&hspi1, buf, 4, 0xFFFFFFFF);
  SS_DESELECT();
}
// Функция записи в буфер данных переменной длины
void w5500_write_buf(data_sect_ptr *datasect, uint16_t len)
{
  SS_SELECT();
  HAL_SPI_Transmit(&hspi1, (uint8_t*) datasect, len, 0xFFFFFFFF);
  SS_DESELECT();
}
// Функция записи в буфер данных переменной длины с привязкой к определенному сокету
void w5500_write_sock_buf(uint8_t sock_num, uint16_t point, uint8_t *buf, uint16_t len)
{
  data_sect_ptr *datasect = (void*)buf;
  datasect->opcode = (((sock_num<<2)|BSB_S0_TX)<<3)|(RWB_WRITE<<2)|OM_FDM0;
  datasect->addr = be16toword(point);
  w5500_write_buf(datasect,len+3);//3 служебных байта 
}
// Функция чтения байта из регистра
uint8_t w5500_read_reg(uint8_t op, uint16_t address)
{
  uint8_t data;
  uint8_t wbuf[] = {address >> 8, address, op, 0x0};
  uint8_t rbuf[4];
  SS_SELECT();
  HAL_SPI_TransmitReceive(&hspi1, wbuf, rbuf, 4, 0xFFFFFFFF);
  SS_DESELECT();
  data = rbuf[3];
  return data;
}
// Функция чтения буфера
void w5500_read_buf(data_sect_ptr *datasect, uint16_t len)
{
  SS_SELECT();
  HAL_SPI_Transmit(&hspi1, (uint8_t*) datasect, 3, 0xFFFFFFFF);
  HAL_SPI_Receive(&hspi1, (uint8_t*) datasect, len, 0xFFFFFFFF);
  SS_DESELECT();
}
// Функция чтения одного байта из буфера
uint8_t w5500_read_sock_buf_byte(uint8_t sock_num, uint16_t point)
{
  uint8_t opcode, bt;
  opcode = (((sock_num<<2)|BSB_S0_RX)<<3)|OM_FDM1;
  bt = w5500_read_reg(opcode, point);
  return bt;
}
// Функция чтения нескольких байт буфера 
void w5500_read_sock_buf(uint8_t sock_num, uint16_t point, uint8_t *buf, uint16_t len)
{
  data_sect_ptr *datasect = (void*)buf;
  datasect->opcode = (((sock_num<<2)|BSB_S0_RX)<<3)|OM_FDM0;
  datasect->addr = be16toword(point);
  w5500_read_buf(datasect,len);
}
// Функция инициализации порта в сокете
void set_sock_port(uint8_t sock_num, uint16_t port)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(opcode, Sn_PORT0,port>>8);
  w5500_write_reg(opcode, Sn_PORT1,port);
}
// Функция инициализации сокета
void open_socket(uint8_t sock_num, uint16_t mode)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(opcode, Sn_MR, mode);
  w5500_write_reg(opcode, Sn_CR, 0x01);
}
// Функция ожидания окончания инициализации сокета
void socket_init_wait(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_read_reg(opcode, Sn_SR)==SOCK_INIT)
    {
      break;
    }
  }
}
// Функция прослушивания сокета
void listen_socket(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(opcode, Sn_CR, 0x02); //LISTEN SOCKET
}
// Функция ожидания пакета по сокету
void socket_listen_wait(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_read_reg(opcode, Sn_SR)==SOCK_LISTEN)
    {
      break;
    }
  }
}
// Функция ожидания закрытия сокета
void socket_closed_wait(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_read_reg(opcode, Sn_SR)==SOCK_CLOSED)
    {
      break;
    }
  }
}
// Функция закрытия соединения
void disconnect_socket(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(opcode, Sn_CR, 0x08); //DISCON
}
// Функция определения текущего состояния сокета
uint8_t get_socket_status(uint8_t sock_num)
{
  uint8_t dt;
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  dt = w5500_read_reg(opcode, Sn_SR);
  return dt;
}
// Функция для подготовки отправки буфера сетевому устройству
void recv_socket(uint8_t sock_num)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(opcode, Sn_CR, 0x40); //RECV SOCKET
}
// Функция отправки буфера сетевому устройству
void send_socket(uint8_t sock_num)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(opcode, Sn_CR, 0x20); //SEND SOCKET
}
// Функция определения размера принятых данных
uint16_t get_size_rx(uint8_t sock_num)
{
  uint16_t len;
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  len = (w5500_read_reg(opcode,Sn_RX_RSR0)<<8|w5500_read_reg(opcode,Sn_RX_RSR1));
  return len;
}
// Функция определения адрес данных в приемном буфере
uint16_t get_read_pointer(uint8_t sock_num)
{
  uint16_t point;
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  point = (w5500_read_reg(opcode,Sn_RX_RD0)<<8|w5500_read_reg(opcode,Sn_RX_RD1));
  return point;
}
// Функция возвращает адрес начала данных для записи в буфер отправки
uint16_t get_write_pointer(uint8_t sock_num)
{
  uint16_t point;
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  point = (w5500_read_reg(opcode,Sn_TX_WR0)<<8|w5500_read_reg(opcode,Sn_TX_WR1));
  return point;
}
// Функция установки адреса начала данных для записи в буфер отправки
void set_write_pointer(uint8_t sock_num, uint16_t point)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(opcode, Sn_TX_WR0, point>>8);
  w5500_write_reg(opcode, Sn_TX_WR1, (uint8_t)point);
}
// Функция аппаратного сброса микросхемы
void w5500_hardware_rst(void)
{
	RST_SELECT();
  HAL_Delay(70);
  RST_DESELECT();
  HAL_Delay(70);
}
// Функция программного сброса микросхемы
void w5500_soft_rst(void)
{
	uint8_t opcode = 0;
	
	opcode = (BSB_COMMON<<3)|OM_FDM1;
	w5500_write_reg(opcode, MR, 0x80);
	HAL_Delay(100);
}
// Функция установки mac адреса микросхемы
void w5500_set_mac_addr(uint8_t macaddr[6])
{
  uint8_t opcode = 0;

  w5500_write_reg(opcode, SHAR0,macaddr[0]);
  w5500_write_reg(opcode, SHAR1,macaddr[1]);
  w5500_write_reg(opcode, SHAR2,macaddr[2]);
  w5500_write_reg(opcode, SHAR3,macaddr[3]);
  w5500_write_reg(opcode, SHAR4,macaddr[4]);
  w5500_write_reg(opcode, SHAR5,macaddr[5]);
}
// Функция установки ip адреса маршрутизатора
void w5500_set_ip_gate_addr(uint8_t ipgate[4])
{
  uint8_t opcode = 0;

  w5500_write_reg(opcode, GWR0,ipgate[0]);
  w5500_write_reg(opcode, GWR1,ipgate[1]);
  w5500_write_reg(opcode, GWR2,ipgate[2]);
  w5500_write_reg(opcode, GWR3,ipgate[3]);
}
// Функция установки маски подсети
void w5500_set_ipmask(uint8_t ipmask[4])
{
  uint8_t opcode = 0;
  
  w5500_write_reg(opcode, SUBR0,ipmask[0]);
  w5500_write_reg(opcode, SUBR1,ipmask[1]);
  w5500_write_reg(opcode, SUBR2,ipmask[2]);
  w5500_write_reg(opcode, SUBR3,ipmask[3]);
}
// Функция установки ip адреса микросхемы
void w5500_set_ipaddr(uint8_t ipaddr[4])
{
  uint8_t opcode = 0;

  w5500_write_reg(opcode, SIPR0,ipaddr[0]);
  w5500_write_reg(opcode, SIPR1,ipaddr[1]);
  w5500_write_reg(opcode, SIPR2,ipaddr[2]);
  w5500_write_reg(opcode, SIPR3,ipaddr[3]);
}
// Функция инициализации микросхемы
void w5500_ini(void)
{ 
  // Аппаратный сброс
  w5500_hardware_rst();

  // Программный сброс
  w5500_soft_rst();

  // Конфигурация сети
  w5500_set_mac_addr(macaddr);
  w5500_set_ip_gate_addr(ipgate);
  w5500_set_ipmask(ipmask);
  w5500_set_ipaddr(ipaddr);

  // Настраиваем сокеты
  set_sock_port(0, local_port);

  // Открываем сокет 0
  open_socket(0,Mode_TCP);
  socket_init_wait(0);

  // Начинаем слушать сокет 0
  listen_socket(0);
  socket_listen_wait(0);
  HAL_Delay(500);

  // Проверяем статусы
	get_socket_status(0);
}
// Функция приема пакета по сети
void w5500_packet_receive(uint8_t sn)
{
  uint16_t point;
  uint16_t len;
	uint8_t rx_buf[56];
	uint8_t tx_buf[56];

  // Если статус текущего сокета "Соединено"
	if(get_socket_status(sn) == SOCK_ESTABLISHED)
	{
    //listen_socket(sn);
    //socket_listen_wait(sn);
		len = get_size_rx(sn);
    //Если пришел пустой пакет, то уходим из функции
		if(len == 0) 
		{
			return;
		}
		else
		{
			w5500_read_sock_buf(sn, get_read_pointer(sn), rx_buf, len);
			memcpy(tx_buf + 3, rx_buf, len);
			w5500_write_sock_buf(sn, get_write_pointer(sn), tx_buf, len);
			recv_socket(sn);
			send_socket(sn);
			
			disconnect_socket(sn);
			
			open_socket(sn, Mode_TCP);
			socket_init_wait(sn);
			
			listen_socket(sn);
      socket_listen_wait(sn);
		}
	}
}
