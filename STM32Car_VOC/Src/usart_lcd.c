
#include "usart_lcd.h"
#include "usartdriver.h"
#include "oled_display.h"

char lcd_str[] = "CLR(60);BL(7);DIR(1);CIRF(5,25,3,53);DC16(10,20,'00',3);DC16(14,0,'00:00   2000-00-00',3);DC32(30,25,'000', 22);DC16(120,41,'km/h', 22);DC24(10,77,'22.1��',14);DC24(10,100,'22%',14);DC16(50,107,\'RH\',14);DC16(95,85,'����',14);DC16(10,60,'TVOC:100.0ppm',14);\r\n";

void usart_lcd_display(void)
{
	write(USART2_ID, &lcd_str[0], sizeof(lcd_str)-1);
}

char* get_lcd_str(void)
{
	return lcd_str;
}

/* �����ٶ����� */
char* usart_lcd_display_value(float value, LCD *plcd, const char* send, unsigned char bcolor)
{
	char buff[10];
	unsigned int i = 0;
	char *psrc;
	
	if(0==plcd) return 0;
	
	psrc = plcd->display_buff;
			
	copy_string(psrc, (char *)send, strlen((char*)send));
		
	for(; *psrc; ++psrc)
	{
		if(*psrc=='C' && *(psrc+1)=='C')
		{
			int i = 0;
			int_to_string(bcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			continue;
		}
		
		
		
		if(*psrc=='X' && *(psrc+1)=='X' && *(psrc+2)=='X')
		{
			int i = 0;
			float_to_string(value, buff, 3, 0);
			i = strlen(buff);
			if(3==i)
			{
				*psrc = buff[0];
				++psrc;
				*psrc = buff[1];
				++psrc;
				*psrc = buff[2];
			}else if(2==i)
			{
				*psrc = ' ';
				++psrc;
				*psrc = buff[0];
				++psrc;
				*psrc = buff[1];
			}else
			{
				*psrc = ' ';
				++psrc;
				*psrc = ' ';
				++psrc;
				*psrc = buff[0];
			}
			break;
		}
	}
	return plcd->display_buff;
}


/* �����ٶ����� */
char* usart_lcd_display_speed(float speed, LCD *plcd, unsigned char bcolor, unsigned char fcolor)
{
	char buff[10];
	unsigned int i = 0;
	char *psrc;
	
	if(0==plcd) return 0;
	
	psrc = plcd->display_buff;
			
	copy_string(plcd->display_buff, SPEED_DISPLAY, sizeof(SPEED_DISPLAY));
		
	for(; *psrc; ++psrc)
	{
		if(*psrc=='C' && *(psrc+1)=='C')
		{
			int i = 0;
			int_to_string(bcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			continue;
		}
		
		if(*psrc=='X' && *(psrc+1)=='X' && *(psrc+2)=='X')
		{
			int i = 0;
			float_to_string(speed, buff, 3, 1);
			i = strlen(buff);
			if(5==i)
			{
				*psrc = buff[0];
				++psrc;
				*psrc = buff[1];
				++psrc;
				*psrc = buff[2];
				++psrc;
				*psrc = buff[3];
				++psrc;
				*psrc = buff[4];
			}else if(4==i)
			{
				*psrc = ' ';
				++psrc;
				*psrc = buff[0];
				++psrc;
				*psrc = buff[1];
				++psrc;
				*psrc = buff[2];
				++psrc;
				*psrc = buff[3];
				
			}else if(3==i)
			{
				*psrc = ' ';
				++psrc;
				*psrc = ' ';
				++psrc;
				*psrc = buff[0];
				++psrc;
				*psrc = buff[1];
				++psrc;
				*psrc = buff[2];
			}else 
			{
				*psrc = ' ';
				++psrc;
				*psrc = ' ';
				++psrc;
				*psrc = '0';
				++psrc;
				*psrc = '.';
				++psrc;
				*psrc = '0';
			}
			continue;
		}
		
		if(*psrc=='F' && *(psrc+1)=='F')
		{
			int i = 0;
			int_to_string(fcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			break;
		}		
	}
	return plcd->display_buff;
}

/* �����¶����� */
char* usart_lcd_display_temp(float temp, LCD *plcd, unsigned char bcolor, unsigned char fcolor)
{
	char buff[10];
	unsigned int i = 0;
	char *psrc;
	
	if(0==plcd) return 0;
	
	psrc = plcd->display_buff;
			
	copy_string(psrc, (char*)TEMP_DISPLAY, sizeof(TEMP_DISPLAY));
		
	for(; *psrc; ++psrc)
	{
		if(*psrc=='C' && *(psrc+1)=='C')
		{
			int i = 0;
			int_to_string(bcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			continue;
		}
		
		if(*psrc=='X' && *(psrc+1)=='X' && *(psrc+2)=='X' && *(psrc+3)=='X')
		{
			int i = 0;
			float_to_string(temp, buff, 2, 1);
			i = strlen(buff);
			if(4==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
				*(psrc+2) = buff[2];
				*(psrc+3) = buff[3];
			}else if(3==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
				*(psrc+2) = buff[1];
				*(psrc+3) = buff[2];
			}else
			{
				*psrc = ' 0';
				*(psrc+1) ='0';
				*(psrc+2) = '.';
				*(psrc+3) = '0';
			}
			continue;
		}
		
		if(*psrc=='F' && *(psrc+1)=='F')
		{
			int i = 0;
			int_to_string(fcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			break;
		}
	}
	return plcd->display_buff;
}

/* ����ʪ���� */
char* usart_lcd_display_RH(float rh, LCD *plcd, unsigned char bcolor, unsigned char fcolor)
{
	char buff[10];
	unsigned int i = 0;
	char *psrc;
	
	if(0==plcd) return 0;
	
	psrc = plcd->display_buff;
			
	copy_string(psrc, (char *)RH_DISPLAY, sizeof(RH_DISPLAY));
		
	for(; *psrc; ++psrc)
	{
		if(*psrc=='C' && *(psrc+1)=='C')
		{
			int i = 0;
			int_to_string(bcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			continue;
		}
		
		if(*psrc=='X' && *(psrc+1)=='X')
		{
			int i = 0;
			float_to_string(rh, buff, 2, 0);
			i = strlen(buff);
			if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}else if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else
			{
				*psrc = ' 0';
				*(psrc+1) ='0';
			}
			continue;
		}
		
		if(*psrc=='F' && *(psrc+1)=='F')
		{
			int i = 0;
			int_to_string(fcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			break;
		}	
	}
	return plcd->display_buff;
}

/* ����TVOC��ʾ���� */
char* usart_lcd_display_TVOC(float rh, LCD *plcd, unsigned char bcolor, unsigned char fcolor)
{
	char buff[10];
	unsigned int i = 0;
	char *psrc;
	
	if(0==plcd) return 0;
	
	psrc = plcd->display_buff;
			
	copy_string(psrc, (char *)TVOC_DISPLAY, sizeof(TVOC_DISPLAY));
		
	for(; *psrc; ++psrc)
	{
		if(*psrc=='C' && *(psrc+1)=='C')
		{
			int i = 0;
			int_to_string(bcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			continue;
		}
		
		if(*psrc=='X' && *(psrc+1)=='X' && *(psrc+2)=='X' && *(psrc+3)=='X' && *(psrc+4)=='X')
		{
			int i = 0;
			float_to_string(rh, buff, 3, 1);
			i = strlen(buff);
			if(5==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
				*(psrc+2) = buff[2];
				*(psrc+3) = buff[3];
				*(psrc+4) = buff[4];
			}else if(4==i)
			{
				*psrc = ' ';
				*(psrc+1) = buff[0];
				*(psrc+2) = buff[1];
				*(psrc+3) = buff[2];
				*(psrc+4) = buff[3];
			}else if(3==i)
			{
				*psrc = ' ';
				*(psrc+1) = ' ';
				*(psrc+2) = buff[0];
				*(psrc+3) = buff[1];
				*(psrc+4) = buff[2];
			}else
			{
				*psrc = '0';
				*(psrc+1) = '0';
				*(psrc+2) = '0';
				*(psrc+3) = '.';
				*(psrc+4) = '0';
			}
			continue;
		}
		
		if(*psrc=='F' && *(psrc+1)=='F')
		{
			int i = 0;
			int_to_string(fcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			break;
		}	
	}
	return plcd->display_buff;
}

/* ������ʾ���ǿ������� */
char* usart_lcd_display_GPS(unsigned char num, LCD *plcd, unsigned char bcolor, unsigned char fcolor)
{
	char buff[10];
	unsigned int i = 0;
	char *psrc;
	
	if(0==plcd) return 0;
	
	psrc = plcd->display_buff;
			
	copy_string(psrc, (char *)GPS_DISPLAY, sizeof(GPS_DISPLAY));
		
	for(; *psrc; ++psrc)
	{
		if(*psrc=='C' && *(psrc+1)=='C')
		{
			int i = 0;
			int_to_string(bcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			continue;
		}
		
		if(*psrc=='X' && *(psrc+1)=='X')
		{
			int i = 0;
			int_to_string(num, buff , 2);
			i = strlen(buff);
			if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}else if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else
			{
				*psrc = '0';
				*(psrc+1) = '0';
			}
			continue;
		}
		
		if(*psrc=='F' && *(psrc+1)=='F')
		{
			int i = 0;
			int_to_string(fcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			break;
		}	
	}
	return plcd->display_buff;
}

/* ���¿���������ʾ���� */
char* usart_lcd_display_airq(TVOC_PPM_STATUS status, LCD *plcd, unsigned char bcolor, unsigned char fcolor)
{
	char buff[10];
	unsigned int i = 0;
	char *psrc;
	
	if(0==plcd) return 0;
	
	psrc = plcd->display_buff;
			
	copy_string(psrc, (char *)AIR_DISPLAY, sizeof(AIR_DISPLAY));
		
	for(; *psrc; ++psrc)
	{
		if(*psrc=='C' && *(psrc+1)=='C')
		{
			int i = 0;
			int_to_string(bcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			continue;
		}
		
		if(*psrc=='X' && *(psrc+1)=='X' && *(psrc+2)=='X' && *(psrc+3)=='X' && *(psrc+4)=='X')
		{
			if(TVOC_PPM_00==status)
			{
				copy_string(psrc, (char *)AIR_GOOD, sizeof(AIR_GOOD)-1);
			}else if(TVOC_PPM_01==status)
			{
				copy_string(psrc, (char *)AIR_BAD, sizeof(AIR_BAD)-1);
			}else if(TVOC_PPM_02==status)
			{
				copy_string(psrc, (char *)AIR_SO_BAD, sizeof(AIR_SO_BAD)-1);
			}else if(status>=TVOC_PPM_03)
			{
				copy_string(psrc, (char *)AIR_SOO_BAD, sizeof(AIR_SOO_BAD)-1);
			}else
			{
				
			}
			continue;
		}
		
		if(*psrc=='F' && *(psrc+1)=='F')
		{
			int i = 0;
			int_to_string(fcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			break;
		}	
	}
	return plcd->display_buff;
}

/* ������ʾ�������� */
char* usart_lcd_display_error(unsigned char num, LCD *plcd, unsigned char bcolor, unsigned char fcolor)
{
	char buff[10];
	unsigned int i = 0;
	char *psrc;
	
	if(0==plcd) return 0;
	
	psrc = plcd->display_buff;
			
	copy_string(psrc, (char *)ERROR_DISPLAY, sizeof(ERROR_DISPLAY));
		
	for(; *psrc; ++psrc)
	{
		if(*psrc=='C' && *(psrc+1)=='C')
		{
			int i = 0;
			int_to_string(bcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			continue;
		}
		
		if(*psrc=='X' && *(psrc+1)=='X')
		{
			int i = 0;

			int_to_string(num, buff , 2);
			i = strlen(buff);
			if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}else if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else
			{
				*psrc = '0';
				*(psrc+1) = '0';
			}
			continue;
		}
		
		if(*psrc=='F' && *(psrc+1)=='F')
		{
			int i = 0;
			int_to_string(fcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			break;
		}	
	}
	return plcd->display_buff;
}

//void time_convert(charchar* gpstime, char* gpsdate)

char* utc_time_display(LCD *plcd, char* gpstime, char* gpsdate, unsigned char bcolor, unsigned char fcolor)
{
	char buff[10];
	unsigned int i = 0;
	char *psrc;
	
	if(0==gpstime || 0==gpsdate || 0==plcd) return (void*)0;
	
	psrc = plcd->display_buff;	
	copy_string(psrc, (char *)TIME_DISPLAY, sizeof(TIME_DISPLAY));
	
	for(; *psrc; ++psrc)
	{
		if(*psrc=='C' && *(psrc+1)=='C')
		{
			int i = 0;
			int_to_string(bcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			continue;
		}
		
		if(*psrc=='X' && *(psrc+1)=='X' && *(psrc+2)==':')
		{
			*psrc++ = gpstime[0];
			*psrc++ = gpstime[1];
			*psrc++ = ':';
			*psrc++ = gpstime[2];
			*psrc++ = gpstime[3];
			continue;
		}
		
		if(*psrc=='X' && *(psrc+1)=='X' && *(psrc+2)=='X')
		{
			*psrc++ = '2';
			*psrc++ = '0';
			*psrc++ = gpsdate[4];
			*psrc++ = gpsdate[5];
			*psrc++ = '-';
			*psrc++ = gpsdate[2];
			*psrc++ = gpsdate[3];
			*psrc++ = '-';
			*psrc++ = gpsdate[0];
			*psrc++ = gpsdate[1];
			continue;
		}	
		
		if(*psrc=='F' && *(psrc+1)=='F')
		{
			int i = 0;
			int_to_string(fcolor, buff , 2);
			
			i = strlen(buff);
			
			if(1==i)
			{
				*psrc = '0';
				*(psrc+1) = buff[0];
			}else if(2==i)
			{
				*psrc = buff[0];
				*(psrc+1) = buff[1];
			}
			break;
		}	
	}
	return plcd->display_buff;
}



