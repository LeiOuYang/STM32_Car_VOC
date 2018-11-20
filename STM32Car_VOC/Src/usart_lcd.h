
#ifndef USART_LCD
#define USART_LCD

#include "KQM28xx.h"
#include "nmea0183.h"

#define LCD_240_320_ENABLE 1

#define LCD_120_160_ENABLE 0

#if (!LCD_240_320_ENABLE && !LCD_120_160_ENABLE)
	#define LCD_120_160_EXIST 1

#elif LCD_120_160_ENABLE
	#define LCD_120_160_EXIST 1
	
#else
 	#define LCD_240_320_EXIST 1
	
#endif

#if LCD_120_160_EXIST

	#define SPEED_DISPLAY "SBC(CC);DCV32(30,25,'XXXXX', FF);\r\n"
	#define TEMP_DISPLAY "SBC(CC);DCV24(10,77,'XXXX��',FF);\r\n"
	#define RH_DISPLAY	"SBC(CC);DCV24(10,100,'XX%',FF);\r\n"
	#define TVOC_DISPLAY "SBC(CC);DCV16(10,60,'TVOC:XXXXXppm',FF);\r\n"
	#define GPS_DISPLAY "SBC(CC);DCV16(10,20,'XX',FF);\r\n"
	#define AIR_DISPLAY "SBC(CC);DCV16(95,85,'XXXXXX',FF);\r\n"
	#define ERROR_DISPLAY "SBC(CC);DCV16(95,110,'ERROR:XX',FF);\r\n"
	#define TIME_DISPLAY	"SBC(CC);DCV16(14,0,'XX:XX   XXXX-XX-XX',FF);\r\n"

#elif  LCD_240_320_EXIST

	#define SPEED_DISPLAY "SBC(CC);DC48(80,50,'XXXXX', FF,1);\r\n"
	#define TEMP_DISPLAY "SBC(CC);DCV24(87,160,'XXXX��',FF);\r\n"
	#define RH_DISPLAY	"SBC(CC);DCV24(87,200,'XX% RH',FF);\r\n"
	#define TVOC_DISPLAY "SBC(CC);DCV24(87,120,'XXXXXppm',FF);\r\n"
	#define GPS_DISPLAY "SBC(CC);DCV16(32,40,'XX',FF));\r\n"
	#define AIR_DISPLAY "SBC(CC);DCV32(220,160,'XXXXXX',FF);\r\n"
	#define ERROR_DISPLAY "SBC(CC);DCV16(95,110,'ERROR:XX',FF);\r\n"
	#define TIME_DISPLAY	"SBC(CC);DCV24(15,5,' XX:XX       XXXX-XX-XX',FF);\r\n"	

#endif

#define AIR_GOOD "����  "
#define AIR_BAD "�ϲ�  "
#define AIR_SO_BAD "�ܲ�  "
#define AIR_SOO_BAD "�ǳ��� "

#define BLUE_COLOR 3  			/* ��ɫ */
#define BLACK_COLOR 0 			/* ��ɫ */
#define RED_COLOR 30				/* ��ɫ */
#define YELLOW_COLOR 28   	/* ��ɫ */
#define GREEN_COLOR 25			/* ��ɫ */
#define LIME_COLOR 26				/* ������ */
#define LIME_GREEN_COLOR 2 	/* �������� */
#define ROYAL_BLUE_COLOR 61 /* ����ɫ */
#define CYAN_COLOR 	33			/* ��ɫ */
#define ORANGE_COLOR 53     /* ��ɫ */
#define CHOCOLATE_COLOR 56  /* �ɿ���ɫ */
#define GRAY_COLOR 19				/* ��ɫ */
#define WHITE_COLOR 18      /* ��ɫ */

typedef struct LCD
{
	unsigned char speed_bcolor;
	char display_buff[64];
}LCD;

void usart_lcd_display(void);
char* get_lcd_str(void);
char* usart_lcd_display_speed(float speed, LCD *plcd, unsigned char bcolor, unsigned char fcolor);
char* usart_lcd_display_value(float value, LCD *plcd, const char* send, unsigned char bcolor);
char* usart_lcd_display_temp(float temp, LCD *plcd, unsigned char bcolor, unsigned char fcolor);
char* usart_lcd_display_RH(float rh, LCD *plcd, unsigned char bcolor, unsigned char fcolor);
char* usart_lcd_display_TVOC(float rh, LCD *plcd, unsigned char bcolor, unsigned char fcolor);
char* usart_lcd_display_GPS(unsigned char num, LCD *plcd, unsigned char bcolor, unsigned char fcolor);
char* usart_lcd_display_airq(TVOC_PPM_STATUS status, LCD *plcd, unsigned char bcolor, unsigned char fcolor);
char* usart_lcd_display_error(unsigned char num, LCD *plcd, unsigned char bcolor, unsigned char fcolor);
char* utc_time_display(LCD *plcd, DateTime* dateTime, unsigned char bcolor, unsigned char fcolor);


#endif
