
#ifndef USART_LCD
#define USART_LCD

#include "KQM28xx.h"

#define SPEED_DISPLAY "SBC(CC);DCV32(30,20,'XXXkm/s', FF);\r\n"
#define TEMP_DISPLAY "SBC(CC);DCV24(10,75,'XXXX℃',FF);\r\n"
#define RH_DISPLAY	"SBC(CC);DCV24(10,100,'XX%',FF);\r\n"
#define TVOC_DISPLAY "SBC(CC);DCV16(10,55,'TVOC:XXXXXppm',FF);\r\n"
#define GPS_DISPLAY "SBC(CC);DCV16(10,0,'XX',FF);\r\n"
#define AIR_DISPLAY "SBC(CC);DCV16(95,85,'XXXXXX',FF);\r\n"
#define ERROR_DISPLAY "SBC(CC);DCV16(95,110,'ERROR:XX',FF)\r\n"

#define AIR_GOOD "良好  "
#define AIR_BAD "较差  "
#define AIR_SO_BAD "很差  "
#define AIR_SOO_BAD "非常差 "

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


#endif
