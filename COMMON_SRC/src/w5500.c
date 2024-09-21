#include "w5500.h"

w5500_data w5500_1; // Настройки первой микросхемы w5500
w5500_data* w5500_1_ptr = &w5500_1;
w5500_data w5500_2; // Настройки второй микросхемы w5500
w5500_data* w5500_2_ptr = &w5500_2;

// Функция записи байта в регистр
w5500_status w5500_write_reg(w5500_data* w5500_n, uint8_t op, uint16_t address, uint8_t data)
{
  uint8_t buf[] = {address >> 8, address, op|(RWB_WRITE<<2), data};
	ss_select(w5500_n);
  HAL_StatusTypeDef st = HAL_SPI_Transmit(w5500_n->spi_n, buf, 4, SPI_RX_TIMEOUT);
	ss_deselect(w5500_n);
	
	if (st != HAL_OK)
		return W5500_ERROR;
	else
		return W5500_OK;
}
// Функция записи в буфер данных переменной длины
w5500_status w5500_write_buf(w5500_data* w5500_n, data_sect_ptr *datasect, uint16_t len)
{
  ss_select(w5500_n);
  HAL_StatusTypeDef st = HAL_SPI_Transmit(w5500_n->spi_n, (uint8_t*) datasect, len, SPI_RX_TIMEOUT);
	ss_deselect(w5500_n);
	
	if (st != HAL_OK)
		return W5500_ERROR;
	else
		return W5500_OK;
}
// Функция записи в буфер данных переменной длины с привязкой к определенному сокету
w5500_status w5500_write_sock_buf(w5500_data* w5500_n, uint8_t sock_num, uint16_t point, uint8_t *buf, uint16_t len)
{
  data_sect_ptr *datasect = (void*)buf;
  datasect->opcode = (((sock_num<<2)|BSB_S0_TX)<<3)|(RWB_WRITE<<2)|OM_FDM0;
  datasect->addr = be16toword(point);
  
	if (w5500_write_buf(w5500_n, datasect,len+3) != W5500_OK)
		return W5500_ERROR;//3 служебных байта 
	else
		return W5500_OK;
}
// Функция чтения байта из регистра
w5500_status w5500_read_reg(w5500_data* w5500_n, uint8_t op, uint16_t address, uint8_t *data)
{
  uint8_t wbuf[] = {address >> 8, address, op, 0x0};
  uint8_t rbuf[4];
  
	ss_select(w5500_n);
  HAL_StatusTypeDef st = HAL_SPI_TransmitReceive(w5500_n->spi_n, wbuf, rbuf, 4, SPI_RX_TIMEOUT);
  ss_deselect(w5500_n);
  *data = rbuf[3];
  
	if (st != HAL_OK)
		return W5500_ERROR;
	else
		return W5500_OK;
}
// Функция чтения буфера
w5500_status w5500_read_buf(w5500_data* w5500_n, data_sect_ptr *datasect, uint16_t len)
{
  ss_select(w5500_n);
  HAL_StatusTypeDef st = HAL_SPI_Transmit(w5500_n->spi_n, (uint8_t*) datasect, 3, SPI_RX_TIMEOUT);
  st = HAL_SPI_Receive(w5500_n->spi_n, (uint8_t*) datasect, len, SPI_RX_TIMEOUT);
  ss_deselect(w5500_n);
	
	if (st != HAL_OK)
		return W5500_ERROR;
	else
		return W5500_OK;
}
// Функция чтения одного байта из буфера
w5500_status w5500_read_sock_buf_byte(w5500_data* w5500_n, uint8_t sock_num, uint16_t point, uint8_t *data)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0_RX)<<3)|OM_FDM1;
  
	if (w5500_read_reg(w5500_n, opcode, point, data) != W5500_OK)
		return W5500_ERROR;
	else
		return W5500_OK;
}
// Функция чтения нескольких байт буфера 
w5500_status w5500_read_sock_buf(w5500_data* w5500_n, uint8_t sock_num, uint16_t point, uint8_t *buf, uint16_t len)
{
  data_sect_ptr *datasect = (void*)buf;
  datasect->opcode = (((sock_num<<2)|BSB_S0_RX)<<3)|OM_FDM0;
  datasect->addr = be16toword(point);
  
	if (w5500_read_buf(w5500_n, datasect,len) != W5500_OK)
		return W5500_ERROR;
	else
		return W5500_OK;
}
// Функция инициализации порта в сокете
w5500_status set_sock_port(w5500_data* w5500_n, uint8_t sock_num, uint16_t port)
{
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_write_reg(w5500_n, opcode, Sn_PORT0,port>>8) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, Sn_PORT1,port) != W5500_OK)
		return W5500_ERROR;
	
	return W5500_OK;
}
// Функция инициализации сокета
w5500_status open_socket(w5500_data* w5500_n, uint8_t sock_num, uint16_t mode)
{
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_write_reg(w5500_n, opcode, Sn_MR, mode) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, Sn_CR, 0x01) != W5500_OK)
		return W5500_ERROR;
	
	return W5500_OK;
}
// Функция ожидания окончания инициализации сокета
void socket_init_wait(w5500_data* w5500_n, uint8_t sock_num)
{
  while(1)
  {
    if(get_socket_status(w5500_n, sock_num) == SOCK_INIT)
    {
      break;
    }
  }
}
// Функция прослушивания сокета
w5500_status listen_socket(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_write_reg(w5500_n, opcode, Sn_CR, 0x02) != W5500_OK)
		return W5500_ERROR; //LISTEN SOCKET
	else
		return W5500_OK;
}
// Функция ожидания пакета по сокету
void socket_listen_wait(w5500_data* w5500_n, uint8_t sock_num)
{
  while(1)
  {
    if(get_socket_status(w5500_n, sock_num) == SOCK_LISTEN)
    {
      break;
    }
  }
}
// Функция ожидания закрытия сокета
void socket_closed_wait(w5500_data* w5500_n, uint8_t sock_num)
{
  while(1)
  {
    if(get_socket_status(w5500_n, sock_num) == SOCK_CLOSED)
    {
      break;
    }
  }
}
// Функция открытия соединения
w5500_status connect_socket(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_write_reg(w5500_n, opcode, Sn_CR, 0x04) != W5500_OK)
		return W5500_ERROR; //CONNECT
	else
		return W5500_OK;
}
// Функция закрытия соединения
w5500_status disconnect_socket(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_write_reg(w5500_n, opcode, Sn_CR, 0x08) != W5500_OK)
		return W5500_ERROR; //DISCON
	else
		return W5500_OK;
}
// Функция определения текущего состояния сокета
uint8_t get_socket_status(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t soc_status;
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_read_reg(w5500_n, opcode, Sn_SR, &soc_status) != W5500_OK)
		return SOCK_UNKNOWN;
	else
		return soc_status;
}
// Функция для подготовки отправки буфера сетевому устройству
w5500_status recv_socket(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_write_reg(w5500_n, opcode, Sn_CR, 0x40) != W5500_OK)
		return W5500_ERROR; //RECV SOCKET
	else
		return W5500_OK;
}
// Функция отправки буфера сетевому устройству
w5500_status send_socket(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_write_reg(w5500_n, opcode, Sn_CR, 0x20) != W5500_OK)
		return W5500_ERROR; //SEND SOCKET
	else
		return W5500_OK;
}
// Функция определения размера принятых данных
uint16_t get_size_rx(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t len1, len2;
	uint16_t len;
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_read_reg(w5500_n, opcode, Sn_RX_RSR0, &len1) != W5500_OK)
		return 0;
	if (w5500_read_reg(w5500_n, opcode, Sn_RX_RSR1, &len2) != W5500_OK)
		return 0;
	len = (len1<<8)|len2;
	
	return len;
}
// Функция определения адрес данных в приемном буфере
uint16_t get_read_pointer(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t point1, point2;
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_read_reg(w5500_n, opcode, Sn_RX_RD0, &point1) != W5500_OK)
		return 0;
	if (w5500_read_reg(w5500_n, opcode, Sn_RX_RD1, &point2) != W5500_OK)
		return 0;
  
	return (point1<<8)|point2;
}
w5500_status set_read_pointer(w5500_data* w5500_n, uint8_t sock_num, uint16_t point)
{
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_write_reg(w5500_n, opcode, Sn_RX_RD0, point>>8) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, Sn_RX_RD1, (uint8_t)point) != W5500_OK)
		return W5500_ERROR;
	
	return W5500_OK;
}
// Функция возвращает адрес начала данных для записи в буфер отправки
uint16_t get_write_pointer(w5500_data* w5500_n, uint8_t sock_num)
{
  uint8_t point1, point2;
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_read_reg(w5500_n, opcode, Sn_TX_WR0, &point1) != W5500_OK)
		return 0;
	if (w5500_read_reg(w5500_n, opcode, Sn_TX_WR1, &point2) != W5500_OK)
		return 0;
  
	return (point1<<8)|point2;
}
// Функция установки адреса начала данных для записи в буфер отправки
w5500_status set_write_pointer(w5500_data* w5500_n, uint8_t sock_num, uint16_t point)
{
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_write_reg(w5500_n, opcode, Sn_TX_WR0, point>>8) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, Sn_TX_WR1, (uint8_t)point) != W5500_OK)
		return W5500_ERROR;
	
	return W5500_OK;
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
w5500_status w5500_soft_rst(w5500_data* w5500_n)
{
	uint8_t opcode = (BSB_COMMON<<3)|OM_FDM1;
	if (w5500_write_reg(w5500_n, opcode, MR, 0x80) != W5500_OK)
		return W5500_ERROR;
	HAL_Delay(100);
	
	return W5500_OK;
}
// Функция установки mac адреса микросхемы
w5500_status w5500_set_mac_addr(w5500_data* w5500_n, uint8_t macaddr[6])
{
  uint8_t opcode = 0;

  if (w5500_write_reg(w5500_n, opcode, SHAR0, macaddr[0]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, SHAR1, macaddr[1]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, SHAR2, macaddr[2]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, SHAR3, macaddr[3]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, SHAR4, macaddr[4]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, SHAR5, macaddr[5]) != W5500_OK)
		return W5500_ERROR;
	
	return W5500_OK;
}
// Функция установки ip адреса маршрутизатора
w5500_status w5500_set_ip_gate_addr(w5500_data* w5500_n, uint8_t ipgate[4])
{
  uint8_t opcode = 0;

  if (w5500_write_reg(w5500_n, opcode, GWR0, ipgate[0]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, GWR1, ipgate[1]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, GWR2, ipgate[2]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, GWR3, ipgate[3]) != W5500_OK)
		return W5500_ERROR;
	
	return W5500_OK;
}
// Функция установки маски подсети
w5500_status w5500_set_ipmask(w5500_data* w5500_n, uint8_t ipmask[4])
{
  uint8_t opcode = 0;
  
  if (w5500_write_reg(w5500_n, opcode, SUBR0, ipmask[0]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, SUBR1, ipmask[1]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, SUBR2, ipmask[2]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, SUBR3, ipmask[3]) != W5500_OK)
		return W5500_ERROR;
	
	return W5500_OK;
}
// Функция установки ip адреса микросхемы
w5500_status w5500_set_ipaddr(w5500_data* w5500_n, uint8_t ipaddr[4])
{
  uint8_t opcode = 0;

  if (w5500_write_reg(w5500_n, opcode, SIPR0, ipaddr[0]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, SIPR1, ipaddr[1]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, SIPR2, ipaddr[2]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, SIPR3, ipaddr[3]) != W5500_OK)
		return W5500_ERROR;
	
	return W5500_OK;
}
// Функция установки ip адреса назначения
w5500_status w5500_set_target_ipaddr(w5500_data* w5500_n,  uint8_t ipaddr[4], uint8_t sock_num)
{
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;

  if (w5500_write_reg(w5500_n, opcode, Sn_DIPR0, ipaddr[0]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, Sn_DIPR1, ipaddr[1]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, Sn_DIPR2, ipaddr[2]) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, Sn_DIPR3, ipaddr[3]) != W5500_OK)
		return W5500_ERROR;
	
	return W5500_OK;
}
// Функция установки порта назначения
w5500_status w5500_set_target_port(w5500_data* w5500_n, uint16_t port, uint8_t sock_num)
{
  uint8_t opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  if (w5500_write_reg(w5500_n, opcode, Sn_DPORTR0, port>>8) != W5500_OK)
		return W5500_ERROR;
  if (w5500_write_reg(w5500_n, opcode, Sn_DPORTR1, port) != W5500_OK)
		return W5500_ERROR;
	
	return W5500_OK;
}
// Функция инициализации микросхемы
w5500_status w5500_ini(w5500_data* w5500_n)
{ 
  // Аппаратный сброс
//  w5500_hardware_rst(w5500_n);

  // Программный сброс
  if (w5500_soft_rst(w5500_n))
		return W5500_ERROR;

  // Конфигурация сети
  if (w5500_set_mac_addr(w5500_n, w5500_n->macaddr) != W5500_OK)
		return W5500_ERROR;
  if (w5500_set_ip_gate_addr(w5500_n, w5500_n->ipgate) != W5500_OK)
		return W5500_ERROR;
  if (w5500_set_ipmask(w5500_n, w5500_n->ipmask) != W5500_OK)
		return W5500_ERROR;
  if (w5500_set_ipaddr(w5500_n, w5500_n->ipaddr) != W5500_OK)
		return W5500_ERROR;

	for (uint8_t i = 0; i < (sizeof(w5500_n->port_set)/sizeof(w5500_n->port_set[0])); i++)
	{
		// Настраиваем сокеты
		if (set_sock_port(w5500_n, w5500_n->port_set[i].sock_num, w5500_n->port_set[i].local_port) != W5500_OK)
			return W5500_ERROR;
		// Открываем сокеты
		if (open_socket(w5500_n, w5500_n->port_set[i].sock_num, Mode_TCP) != W5500_OK)
			return W5500_ERROR;
		socket_init_wait(w5500_n, w5500_n->port_set[i].sock_num);
		// Начинаем слушать сокеты - для сервера,
		// коннектимся для клиента
		if (w5500_n->port_set[i].is_client != 1)
		{
			if (w5500_set_target_port(w5500_n, w5500_n->port_set[i].local_port, w5500_n->port_set[i].sock_num) != W5500_OK)
				return W5500_ERROR;
			if (listen_socket(w5500_n, w5500_n->port_set[i].sock_num) != W5500_OK)
				return W5500_ERROR;
			socket_listen_wait(w5500_n, w5500_n->port_set[i].sock_num);
		}
		else
		{
			if (w5500_set_target_port(w5500_n, w5500_n->port_set[i].local_port, w5500_n->port_set[i].sock_num) != W5500_OK)
				return W5500_ERROR;
			if (w5500_set_target_ipaddr(w5500_n, w5500_n->port_set[i].target_ip_addr, w5500_n->port_set[i].sock_num) != W5500_OK)
				return W5500_ERROR;
			if (connect_socket(w5500_n, w5500_n->port_set[i].sock_num) != W5500_OK)
				return W5500_ERROR;
		}
	}
  HAL_Delay(100);
	
	return W5500_OK;
}
// Функция реинициализации сокета
w5500_status w5500_reini_sock(w5500_data* w5500_n, uint8_t sn)
{
	// Конфигурация сети
  if (w5500_set_mac_addr(w5500_n, w5500_n->macaddr) != W5500_OK)
		return W5500_ERROR;
  if (w5500_set_ip_gate_addr(w5500_n, w5500_n->ipgate) != W5500_OK)
		return W5500_ERROR;
  if (w5500_set_ipmask(w5500_n, w5500_n->ipmask) != W5500_OK)
		return W5500_ERROR;
  if (w5500_set_ipaddr(w5500_n, w5500_n->ipaddr) != W5500_OK)
		return W5500_ERROR;
	// Закрываем сокет
	if (disconnect_socket(w5500_n, w5500_n->port_set[sn].sock_num) != W5500_OK)
		return W5500_ERROR;
	//socket_closed_wait(w5500_n, w5500_n->port_set[sn].sock_num);
	// Настраиваем сокет
  if (set_sock_port(w5500_n, w5500_n->port_set[sn].sock_num, w5500_n->port_set[sn].local_port) != W5500_OK)
		return W5500_ERROR;
	// Открываем сокет
  if (open_socket(w5500_n, w5500_n->port_set[sn].sock_num, Mode_TCP) != W5500_OK)
		return W5500_ERROR;
  socket_init_wait(w5500_n, w5500_n->port_set[sn].sock_num);
  // Начинаем слушать сокеты - для сервера,
	// коннектимся для клиента
  if (w5500_n->port_set[sn].is_client != 1)
	{
		if (w5500_set_target_port(w5500_n, w5500_n->port_set[sn].local_port, w5500_n->port_set[sn].sock_num) != W5500_OK)
			return W5500_ERROR;
		if (listen_socket(w5500_n, w5500_n->port_set[sn].sock_num) != W5500_OK)
			return W5500_ERROR;
		socket_listen_wait(w5500_n, w5500_n->port_set[sn].sock_num);
	}
	else
	{
		if (w5500_set_target_port(w5500_n, w5500_n->port_set[sn].local_port, w5500_n->port_set[sn].sock_num) != W5500_OK)
			return W5500_ERROR;
		if (w5500_set_target_ipaddr(w5500_n, w5500_n->port_set[sn].target_ip_addr, w5500_n->port_set[sn].sock_num) != W5500_OK)
			return W5500_ERROR;
		if (connect_socket(w5500_n, w5500_n->port_set[sn].sock_num) != W5500_OK)
			return W5500_ERROR;
	}
  HAL_Delay(100);
	
	return W5500_OK;
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
