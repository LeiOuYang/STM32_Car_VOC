
#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

	#include "oled.h"
	
	#define num_to_char(x) "0123456789."[x]
	
	/* 128*64 dot */
	#define MAX_ROW   128		
	#define MAX_LINE	64
	
	/* font dot 8*16 */
	#define FONT_X_8  8
	#define FONT_Y_16 16
	
	/* 显示标题区域 */
	#define TITLE_FONT_NUM	6
	#define TITLE_X 0
	#define TITLE_Y 0
	#define TITLE "Maverick"
	
	/* 时间显示区域 */
	#define RUNTIME_FONT_NUM	8  //字符个数为8
	#define RUNTIME_X (MAX_ROW-RUNTIME_FONT_NUM*FONT_X_8)
	#define RUNTIME_Y	0
		
	void display_title(void); //显示标题
	void display_string_Font8_16(unsigned char x, unsigned char y, char* pc);
	void display_fuhao_Font8_16(unsigned char x, unsigned char y, unsigned char index);
	void display_string_Font16_16(unsigned char x, unsigned char y, char* pc);
	void display_num_font16_16_char(unsigned char x,unsigned char y,unsigned char chr);
	void display_chinese_font16_16(unsigned char x,unsigned char y,unsigned char* pdata);
	void display_fuhao_Font6_8(unsigned char x, unsigned char y, unsigned char index);
	void display_north(unsigned char x, unsigned char y);
	void display_logo(void);

	void copy_string(char* dest,char *src,unsigned int num);
	unsigned char int_to_string(int value, char* buff, unsigned int num);
	unsigned char float_to_string(double value, char* buff, unsigned int num, unsigned int dotNum);
	static int pow_int(int num, int i);
	
	extern const unsigned char chinese16_16[][32];
	
	
#endif
