/*
**  			数据处理函数库 
**  一些常用算法和数据计算处理 
**
**	Write By Awesome    2018-10-22 
*/


#ifndef AWESOME_DATA_PROCESS
#define AWESOME_DATA_PROCESS

typedef unsigned char BOOL;

/* 宏定义处理 */ 
#define data_uint_to_hex(x) "0123456789ABCDEF"[x];   /* 将数字转换成十六进制字符 */ 
#define BOOL_TRUE 1
#define BOOL_FALSE 0

#define IS_DIGITAL(x) ( ((x)>='0'&&(x)<='9')||(x)=='-'||(x)=='.' )
#define CHAR_TO_DIGITAL(x) ((x)-'0')
#define DIGITAL_TO_CHAR(x) ( (x)+'0' )

/************  内存处理函数   *************/
BOOL memery_set(char* src, unsigned int len, char value); 

/*************** 字符串处理函数 *************/
unsigned int string_len(const char* str);
BOOL string_copy(char* dest, const char* src);
BOOL string_copy_len(char* dest, const char* src, unsigned int len);


/************* 数据转换函数 ****************/ 
unsigned char data_hex_to_uint(char c);
int sring_to_int(const char* pstr, unsigned char* b);
double string_to_float(const char* pstr, unsigned char* b);
unsigned char float_to_string(double value, char* pdest, unsigned int intgr, unsigned int dec);
unsigned char int_to_string(int value, char* pdest, unsigned int intgr);


/***********  常用算法函数    *************/
BOOL nmea0183_crc_str(char* dest, const char* src);


#endif


