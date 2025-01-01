/**
 * API для взаимодействия с dwin дисплеем
 */
#ifndef __DWIN_H
#define __DWIN_H

#include "stm32f4xx_hal.h"
#include "ram.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define DWIN_UART huart1
#define DWIN_BUF_SIZE 128

#define HEADER 0xA55A

// Описание ошибок DWIN
typedef enum
{
	DWIN_OK,
	DWIN_ERROR
} dwin_status;

// Поддерживаемые протоколом команды
typedef enum
{
	none = 0x00,
	write_reg = 0x80,
	read_reg = 0x81,
	write_variable = 0x82,
	read_variable = 0x83
} dwin_commands;

// Структура, описывающая пакет данных
typedef struct packet_data_struct
{
	uint16_t header;		// Заголовок
	uint8_t length;     // Число байт кадра с командного байта
  dwin_commands cmd;  // Команда
  uint8_t data[249];	// Данные
}__attribute__((packed)) packet_struct;

// Функция инициализации дисплея
void dwin_init();

// Функция отображения домашней страницы
void dwin_print_home_page();

// Функция отображения страницы с настройками GasBoilerController
void dwin_print_gasboiler_page();

// Функция отображения страницы с сетевым статусом
void dwin_print_net_page();

// Функция отображения страницы с регистрами устройств
void dwin_print_regs_page();

// Функция отображения страницы с общими настройками
void dwin_print_settings_page();

#define HOME_PAGE_INDOOR_TEMP_INT_VP				0x1000
#define HOME_PAGE_INDOOR_TEMP_FRACT_VP			0x1060
#define HOME_PAGE_INDOOR_TEMP_SIGN_VP				0x1040
#define HOME_PAGE_INDOOR_HUM_INT_VP					0x1010
#define HOME_PAGE_INDOOR_HUM_FRACT_VP				0x1080

#define HOME_PAGE_OUTDOOR_TEMP_INT_VP				0x1020
#define HOME_PAGE_OUTDOOR_TEMP_FRACT_VP			0x1070
#define HOME_PAGE_OUTDOOR_TEMP_SIGN_VP			0x1050
#define HOME_PAGE_OUTDOOR_HUM_INT_VP				0x1030
#define HOME_PAGE_OUTDOOR_HUM_FRACT_VP			0x1090

#define HOME_PAGE_PRESSURE_VP								0x1100
#define HOME_PAGE_WIND_SPEED_VP							0x1110
#define HOME_PAGE_WIND_DIRECT_VP						0x1120

#define HOME_PAGE_HOURS_MINS_VP							0x1200
#define HOME_PAGE_WEAK_DAY_VP								0x1190
#define HOME_PAGE_MOUNTH_DAY_VP							0x1220
#define HOME_PAGE_MOUNTH_VP									0x1210

#define HOME_PAGE_GASBOILER_STAT_VP					0x1260
#define HOME_PAGE_GASBOILER_STAT_SP					0x5000
#define HOME_PAGE_GASBOILER_SET_INT_VP			0x1140
#define HOME_PAGE_GASBOILER_SET_FRACT_VP		0x1180
#define HOME_PAGE_GASBOILER_CUR_INT_VP			0x1130
#define HOME_PAGE_GASBOILER_CUR_FRACT_VP		0x1170

#define HOME_PAGE_LINK_GASBOILER_VP					0x1230
#define HOME_PAGE_LINK_GASBOILER_SP					0x5100
#define HOME_PAGE_LINK_SERVER_VP						0x1240
#define HOME_PAGE_LINK_SERVER_SP						0x5200
#define HOME_PAGE_LINK_WEATHSTAT_VP					0x1250
#define HOME_PAGE_LINK_WEATHSTAT_SP					0x5300

#define HOME_PAGE_WEATHER_PICTURE_VP				0x5400
#define HOME_PAGE_SUNNY_PICT_ID							0x000C
#define HOME_PAGE_SUNNY_CLOUDS_PICT_ID			0x000D
#define HOME_PAGE_CLOUDS_PICT_ID						0x0011
#define HOME_PAGE_SUNNY_RAIN_PICT_ID				0x000B
#define HOME_PAGE_HARD_RAIN_PICT_ID					0x000E

#define GASBOILER_PAGE_CUR_TEMP_VP					0x3040
#define GASBOILER_PAGE_SET_TEMP_VP					0x3000
#define GASBOILER_PAGE_MIN_TEMP_VP					0x3020
#define GASBOILER_PAGE_STATUS_VP						0x3080
#define GASBOILER_PAGE_STATUS_SP						0x6000

#define NET_PAGE_GASBOILER_STAT_VP					0x1480
#define NET_PAGE_GASBOILER_STAT_SP					0x5700
#define NET_PAGE_WEATHSTAT_STAT_VP					0x1510
#define NET_PAGE_WEATHSTAT_STAT_SP					0x5800
#define NET_PAGE_CONTROLPANEL_STAT_VP				0x1420
#define NET_PAGE_CONTROLPANEL_STAT_SP				0x5500
#define NET_PAGE_SERVER_STAT_VP							0x1450
#define NET_PAGE_SERVER_STAT_SP							0x5600
#define NET_PAGE_GASBOILER_IP_VP						0x1460
#define NET_PAGE_WEATHSTAT_IP_VP						0x1490
#define NET_PAGE_CONTROLPANEL_IP_VP					0x1400
#define NET_PAGE_ROUTER_IP_VP								0x1520
#define NET_PAGE_SERVER_IP_VP								0x1430

#define REGS_PAGE_CONTRPANEL1_VP						0x1600
#define REGS_PAGE_CONTRPANEL2_VP						0x1680
#define REGS_PAGE_CONTRPANEL3_VP						0x1700
#define REGS_PAGE_CONTRPANEL4_VP						0x1780

#define REGS_PAGE_WEATHSTAT1_VP							0x2000
#define REGS_PAGE_WEATHSTAT2_VP							0x2080
#define REGS_PAGE_WEATHSTAT3_VP							0x2100
#define REGS_PAGE_WEATHSTAT4_VP							0x2180

#define REGS_PAGE_GASBOILER1_VP							0x1800
#define REGS_PAGE_GASBOILER2_VP							0x1880
#define REGS_PAGE_GASBOILER3_VP							0x1900
#define REGS_PAGE_GASBOILER4_VP							0x1980

#define SETTING_PAGE_CONTRPANEL_TEMP_VP			0x3300
#define SETTING_PAGE_CONTRPANEL_HUM_VP			0x3330
#define SETTING_PAGE_GASBOILER_TEMP_VP			0x3310
#define SETTING_PAGE_GASBOILER_HUM_VP				0x3340
#define SETTING_PAGE_WEATHSTAT_TEMP_VP			0x3320
#define SETTING_PAGE_WEATHSTAT_HUM_VP				0x3350
#define SETTING_PAGE_HOURS_VP								0x3360
#define SETTING_PAGE_MINS_VP								0x3370
#define SETTING_PAGE_MOUNTHDAY_VP						0x3380
#define SETTING_PAGE_MOUNTH_VP							0x3390
#define SETTING_PAGE_YEAR_VP								0x3400

#endif /* __DWIN_H */
