#include "w5500.h"

w5500_data w5500_1; // Настройки первой микросхемы w5500
w5500_data* w5500_1_ptr = &w5500_1;
w5500_data w5500_2; // Настройки второй микросхемы w5500
w5500_data* w5500_2_ptr = &w5500_2;

// Функция записи байта в регистр
void w5500_write_reg(w5500_data* w5500_n, uint8_t op, uint16_t address, uint8_t data)
{
  uint8_t buf[] = {address >> 8, address, op|(RWB_WRITE<<2), data};
	ss_select(w5500_n);
  HAL_SPI_Transmit(&w5500_n->spi_n, buf, 4, 0xFFFFFFFF);
	ss_deselect(w5500_n);
}
// Функция записи в буфер данных переменной длины
void w5500_write_buf(w5500_data* w5500_n, data_sect_ptr *datasect, uint16_t len)
{
  ss_select(w5500_n);
  HAL_SPI_Transmit(&w5500_n->spi_n, (uint8_t*) datasect, len, 0xFFFFFFFF);
	ss_deselect(w5500_n);
}
// Функция записи в буфер данных переменной длины с привязкой к определенному сокету
void w5500_write_sock_buf(w5500_data* w5500_n, uint8_t sock_num, uint16_t point, uint8_t *buf, uint16_t len)
{
  data_sect_ptr *datasect = (void*)buf;
  datasect->opcode = (((sock_num<<2)|BSB_S0_TX)<<3)|(RWB_WRITE<<2)|OM_FDM0;
  datasect->addr = be16toword(point);
  w5500_write_buf(w5500_n, datasect,len+3);//3 служебных байта 
}
// Функция чтения байта из регистра
uint8_t w5500_read_reg(w5500_data* w5500_n, uint8_t op, uint16_t address)
{
  uint8_t data;
  uint8_t wbuf[] = {address >> 8, address, op, 0x0};
  uint8_t rbuf[4];
  
	ss_select(w5500_n);
  HAL_SPI_TransmitReceive(&w5500_n->spi_n, wbuf, rbuf, 4, 0xFFFFFFFF);
  ss_deselect(w5500_n);
  data = rbuf[3];
  return data;
}
// Функция чтения буфера
void w5500_read_buf(w5500_data* w5500_n, data_sect_ptr *datasect, uint16_t len)
{
  ss_select(w5500_n);
  HAL_SPI_Transmit(&w5500_n->spi_n, (uint8_t*) datasect, 3, 0xFFFFFFFF);
  HAL_SPI_Receive(&w5500_n->spi_n, (uint8_t*) datasect, len, 0xFFFFFFFF);
  ss_deselect(w5500_n);
}
// Функция чтения одного байта из буфера
uint8_t w5500_read_sock_buf_byte(w5500_data* w5500_n, uint8_t sock_num, uint16_t point)
{
  uint8_t opcode, bt;
  opcode = (((sock_num<<2)|BSB_S0_RX)<<3)|OM_FDM1;
  bt = w5500_read_reg(w5500_n, opcode, point);
  return bt;
}
// Функция чтения нескольких байт буфера 
void w5500_read_sock_buf(w5500_data* w5500_n, uint8_t sock_num, uint16_t point, uint8_t *buf, uint16_t len)
{
  data_sect_ptr *datasect = (void*)buf;
  datasect->opcode = (((sock_num<<2)|BSB_S0_RX)<<3)|OM_FDM0;
  datasect->addr = be16toword(point);
  w5500_read_buf(w5500_n, datasect,len);
}
// Функция инициализации порта в сокете
void set_sock_port(w5500_data* w5500_n, uint8_t sock_num, uint16_t port)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(w5500_n, opcode, Sn_PORT0,port>>8);
  w5500_write_reg(w5500_n, opcode, Sn_PORT1,port);
}
// Функция инициализации сокета
void open_socket(w5500_data* w5500_n, uint8_t sock_num, uint16_t mode)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(w5500_n, opcode, Sn_MR, mode);
  w5500_write_reg(w5500_n, opcode, Sn_CR, 0x01);
}
// Функция ожидания окончания инициализации сокета
void socket_init_wait(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_read_reg(w5500_n, opcode, Sn_SR)==SOCK_INIT)
    {
      break;
    }
  }
}
// Функция прослушивания сокета
void listen_socket(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(w5500_n, opcode, Sn_CR, 0x02); //LISTEN SOCKET
}
// Функция ожидания пакета по сокету
void socket_listen_wait(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_read_reg(w5500_n, opcode, Sn_SR)==SOCK_LISTEN)
    {
      break;
    }
  }
}
// Функция ожидания закрытия сокета
void socket_closed_wait(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_read_reg(w5500_n, opcode, Sn_SR)==SOCK_CLOSED)
    {
      break;
    }
  }
}
// Функция закрытия соединения
void disconnect_socket(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(w5500_n, opcode, Sn_CR, 0x08); //DISCON
}
// Функция определения текущего состояния сокета
uint8_t get_socket_status(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t dt;
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  dt = w5500_read_reg(w5500_n, opcode, Sn_SR);
  return dt;
}
// Функция для подготовки отправки буфера сетевому устройству
void recv_socket(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(w5500_n, opcode, Sn_CR, 0x40); //RECV SOCKET
}
// Функция отправки буфера сетевому устройству
void send_socket(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(w5500_n, opcode, Sn_CR, 0x20); //SEND SOCKET
}
// Функция определения размера принятых данных
uint16_t get_size_rx(w5500_data* w5500_n, uint8_t sock_num)
{
  uint16_t len;
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  len = (w5500_read_reg(w5500_n, opcode,Sn_RX_RSR0)<<8|w5500_read_reg(w5500_n, opcode,Sn_RX_RSR1));
  return len;
}
// Функция определения адрес данных в приемном буфере
uint16_t get_read_pointer(w5500_data* w5500_n, uint8_t sock_num)
{
  uint16_t point;
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  point = (w5500_read_reg(w5500_n, opcode,Sn_RX_RD0)<<8|w5500_read_reg(w5500_n, opcode,Sn_RX_RD1));
  return point;
}
void set_read_pointer(w5500_data* w5500_n, uint8_t sock_num, uint16_t point)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(w5500_n, opcode, Sn_RX_RD0, point>>8);
  w5500_write_reg(w5500_n, opcode, Sn_RX_RD1, (uint8_t)point);
}
// Функция возвращает адрес начала данных для записи в буфер отправки
uint16_t get_write_pointer(w5500_data* w5500_n, uint8_t sock_num)
{
  uint16_t point;
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  point = (w5500_read_reg(w5500_n, opcode, Sn_TX_WR0)<<8|w5500_read_reg(w5500_n, opcode, Sn_TX_WR1));
  return point;
}
// Функция установки адреса начала данных для записи в буфер отправки
void set_write_pointer(w5500_data* w5500_n, uint8_t sock_num, uint16_t point)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_write_reg(w5500_n, opcode, Sn_TX_WR0, point>>8);
  w5500_write_reg(w5500_n, opcode, Sn_TX_WR1, (uint8_t)point);
}
// Функция аппаратного сброса микросхемы
void w5500_hardware_rst(w5500_data* w5500_n)
{
	rst_select(w5500_n);
  HAL_Delay(70);
  rst_deselect(w5500_n);
  HAL_Delay(70);
}
// Функция программного сброса микросхемы
void w5500_soft_rst(w5500_data* w5500_n)
{
	uint8_t opcode = 0;
	
	opcode = (BSB_COMMON<<3)|OM_FDM1;
	w5500_write_reg(w5500_n, opcode, MR, 0x80);
	HAL_Delay(100);
}
// Функция установки mac адреса микросхемы
void w5500_set_mac_addr(w5500_data* w5500_n, uint8_t macaddr[6])
{
  uint8_t opcode = 0;

  w5500_write_reg(w5500_n, opcode, SHAR0, macaddr[0]);
  w5500_write_reg(w5500_n, opcode, SHAR1, macaddr[1]);
  w5500_write_reg(w5500_n, opcode, SHAR2, macaddr[2]);
  w5500_write_reg(w5500_n, opcode, SHAR3, macaddr[3]);
  w5500_write_reg(w5500_n, opcode, SHAR4, macaddr[4]);
  w5500_write_reg(w5500_n, opcode, SHAR5, macaddr[5]);
}
// Функция установки ip адреса маршрутизатора
void w5500_set_ip_gate_addr(w5500_data* w5500_n, uint8_t ipgate[4])
{
  uint8_t opcode = 0;

  w5500_write_reg(w5500_n, opcode, GWR0, ipgate[0]);
  w5500_write_reg(w5500_n, opcode, GWR1, ipgate[1]);
  w5500_write_reg(w5500_n, opcode, GWR2, ipgate[2]);
  w5500_write_reg(w5500_n, opcode, GWR3, ipgate[3]);
}
// Функция установки маски подсети
void w5500_set_ipmask(w5500_data* w5500_n, uint8_t ipmask[4])
{
  uint8_t opcode = 0;
  
  w5500_write_reg(w5500_n, opcode, SUBR0, ipmask[0]);
  w5500_write_reg(w5500_n, opcode, SUBR1, ipmask[1]);
  w5500_write_reg(w5500_n, opcode, SUBR2, ipmask[2]);
  w5500_write_reg(w5500_n, opcode, SUBR3, ipmask[3]);
}
// Функция установки ip адреса микросхемы
void w5500_set_ipaddr(w5500_data* w5500_n, uint8_t ipaddr[4])
{
  uint8_t opcode = 0;

  w5500_write_reg(w5500_n, opcode, SIPR0, ipaddr[0]);
  w5500_write_reg(w5500_n, opcode, SIPR1, ipaddr[1]);
  w5500_write_reg(w5500_n, opcode, SIPR2, ipaddr[2]);
  w5500_write_reg(w5500_n, opcode, SIPR3, ipaddr[3]);
}
// Функция инициализации микросхемы
void w5500_ini(w5500_data* w5500_n)
{ 
  // Аппаратный сброс
  //w5500_hardware_rst(w5500_n);

  // Программный сброс
  w5500_soft_rst(w5500_n);

  // Конфигурация сети
  w5500_set_mac_addr(w5500_n, w5500_n->macaddr);
  w5500_set_ip_gate_addr(w5500_n, w5500_n->ipgate);
  w5500_set_ipmask(w5500_n, w5500_n->ipmask);
  w5500_set_ipaddr(w5500_n, w5500_n->ipaddr);

  // Настраиваем сокеты
  set_sock_port(w5500_n, w5500_n->sock_num, w5500_n->local_port);

  // Открываем сокет
  open_socket(w5500_n, w5500_n->sock_num, Mode_TCP);
  socket_init_wait(w5500_n, w5500_n->sock_num);

  // Начинаем слушать сокет
  listen_socket(w5500_n, w5500_n->sock_num);
  socket_listen_wait(w5500_n, w5500_n->sock_num);
  HAL_Delay(500);

  // Проверяем статусы
	get_socket_status(w5500_n, w5500_n->sock_num);
}
// Функция установки ss
void ss_select(w5500_data* w5500_n)
{
	HAL_GPIO_WritePin(w5500_n->cs_eth_gpio_port, w5500_n->cs_eth_pin, GPIO_PIN_RESET);
}
// Функция сброса ss
void ss_deselect(w5500_data* w5500_n)
{
	HAL_GPIO_WritePin(w5500_n->cs_eth_gpio_port, w5500_n->cs_eth_pin, GPIO_PIN_SET);
}
// Функция установки rst
void rst_select(w5500_data* w5500_n)
{
	HAL_GPIO_WritePin(w5500_n->rst_eth_gpio_port, w5500_n->rst_eth_pin, GPIO_PIN_RESET);
}
// Функция сброса rst
void rst_deselect(w5500_data* w5500_n)
{
	HAL_GPIO_WritePin(w5500_n->rst_eth_gpio_port,  w5500_n->rst_eth_pin, GPIO_PIN_SET);
}
