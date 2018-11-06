#include "AwesomeDataProcess.h"

/************  内存处理函数   *************/

/* 初始化内存空间 
*  形式参数：
*		char* src -> 待初始化的缓冲区
*		unsigned int len -> 缓冲区数据空间大小
*		char value -> 初始化值
*  返回参数： 数据有效返回true，无效返回false 
*/ 
BOOL memery_set(char* src, unsigned int len, char value)
{
	if( (void*)0==src || 0==len )	return BOOL_FALSE;
	
	while(len--)
	{
		*src++ = value;
	}
	return BOOL_TRUE;
}

/*************** 字符串处理函数 *************/


/*  字符串长度计算，以'\0'为结束符 
*   输入参数： 
*		const char* str -> 待需计算的字符串长度的原字符串 
*   返回值： 
*		字符串长度,不包括字符串结束符'\0' 
*/
unsigned int string_len(const char* str)
{
	unsigned int len = 0;
	
	if((void*)0 == str) return 0;
	
	while(*str)
	{
		++len;
		++str;
	}
	return len;
} 

/* 字符串复制
*  将原始字符串复制的另一个字符串中，结束符为'\0' 
*  需保正目标字符串空间足够
* 
*  形式参数：
*		char* dest -> 目标字符串
* 		const char* src -> 原始字符串
*  复制成功成功返回真， 否则返回假 
*
* */ 
BOOL string_copy(char* dest, const char* src)
{
	if( (void*)0==dest || (void*)0==src )
		return BOOL_FALSE;
	while(*src)
	{
		*dest++ = *src++;
	}
	*dest = '\0';
	return BOOL_TRUE;
}

/* 复制一定长度的字符串 
*  将原始字符串复制的另一个字符串中，结束符为'\0'或者复制长度足够 
*  需保正目标字符串空间足够
* 
*  形式参数：
*		char* dest -> 目标字符串
* 		const char* src -> 原始字符串
*		unsigend int len -> 复制字符串的长度 
*  复制成功成功返回真， 否则返回假 
*
* */ 
BOOL string_copy_len(char* dest, const char* src, unsigned int len)
{
	if( (void*)0==dest || (void*)0==src || 0==len )
		return BOOL_FALSE;
	while( *src && 0!=len )
	{
		*dest++ = *src++;
		--len;
	}
	*dest = '\0';
	return BOOL_TRUE;
}


/************* 数据转换函数 ****************/ 

/* 十六进制字符转换为数字 */
unsigned char data_hex_to_uint(char c)
{
	if(c>='0'&& c<='9')
	{
		return (unsigned char)(c-48);
	}else if(c>='A' && c<='F')
	{
		return (unsigned char)(c-55);
	}
	return 0;
}




/************  常用算法函数    **************/

/* nmea0183协议数据封包加帧头'$',结束符'*'和校验位
*  校验位的求法：$和*之间的字符异或，异或的结果再换算成16进制字符表示 
*  形式参数： char* dest -> 处理完成存储数据缓冲区
*			  char* src  -> 待封装数据 
*  返回参数： 数据无效返回false，成功返回true 
*/
BOOL nmea0183_crc_str(char* dest, const char* src)
{
	char crc = 0;
	
	if( (void*)0==dest || (void*)0==src ) return BOOL_FALSE;
	
	*dest++ = '$';
	
	while(*src)
	{
		*dest++ = *src;
		crc ^= *src;
		++src; 
	}
	*dest++ = '*';
	*dest++ = data_uint_to_hex(crc/16);
	*dest++ = data_uint_to_hex(crc%16);
	*dest++ = '\r';
	*dest++ = '\n'; 
	*dest = '\0';
	return BOOL_TRUE;
}

