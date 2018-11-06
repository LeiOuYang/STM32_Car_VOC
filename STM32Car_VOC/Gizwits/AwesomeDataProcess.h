/*
**  			���ݴ��������� 
**  һЩ�����㷨�����ݼ��㴦�� 
**
**	Write By Awesome    2018-10-22 
*/


#ifndef AWESOME_DATA_PROCESS
#define AWESOME_DATA_PROCESS

typedef unsigned char BOOL;

/* �궨�崦�� */ 
#define data_uint_to_hex(x) "0123456789ABCDEF"[x];   /* ������ת����ʮ�������ַ� */ 
#define BOOL_TRUE 1
#define BOOL_FALSE 0

/************  �ڴ洦������   *************/
BOOL memery_set(char* src, unsigned int len, char value); 

/*************** �ַ����������� *************/
unsigned int string_len(const char* str);
BOOL string_copy(char* dest, const char* src);
BOOL string_copy_len(char* dest, const char* src, unsigned int len);


/************* ����ת������ ****************/ 
unsigned char data_hex_to_uint(char c); 


/***********  �����㷨����    *************/
BOOL nmea0183_crc_str(char* dest, const char* src);


#endif

