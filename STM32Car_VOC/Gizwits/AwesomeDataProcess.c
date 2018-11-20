#include "AwesomeDataProcess.h"

/* 辅助处理静态函数 */

static int int_pow(int value, unsigned int count);
static double double_pow(double value, unsigned int count);
static unsigned char string_check_digital(const char* pc);

static int int_pow(int value, unsigned int count)
{
	int v = 1;
	
	while(count--)
		v = v*value;
	return v;
} 

static double double_pow(double value, unsigned int count)
{
	double v = 1;
	
	while(count--)
		v = v*value;
	return v;
}

/* 检查字符串是否为数字字符串，是返回1，否则返回0 */ 
static unsigned char string_check_digital(const char* pc)
{
	const char* p;
	if((void*)0==pc || '\0'==*pc) return 0;	
	
	p = pc;
	while('\0'!=*p)
	{
		if(!IS_DIGITAL(*p)) return 0;
		++p;
	}
	return 1;	
}

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
unsigned char data_hex_to_uint(char a)
{
	if (a >= 'A' && a <= 'F')
			return a - 'A' + 10;
	else if (a >= 'a' && a <= 'f')
			return a - 'a' + 10;
	else
			return a - '0';
}

/* 数字字符串处理函数,识别符号并且不检查溢出
*  如果是小数，则只返回整数部分,如果转换出错，返回0 
*  传入参数： 原始字符串， 转换成功标志 0-错误  1-正确 
*/
int sring_to_int(const char* pstr, unsigned char* b)
{
	char nega = 1;    /* 负数为1， 正数为1 */ 
	const char* psrc = (void*)0;
	int value = 0;
	
	if( (void*)0==pstr ) 
	{
		*b = 0; 
		return 0;
	}
	
	if( !string_check_digital(pstr) ) 
	{
		*b = 0; 
		return 0;
	}
	
	psrc = pstr;
	if('-'==*psrc)  /* 符号位 */ 
	{
		nega = -1;
		++psrc;
	}	
	
	while('\0'!=*psrc)
	{
		if('.'==*psrc || '-'==*psrc ) return value;
		value = 10*value + CHAR_TO_DIGITAL(*psrc);	
		++psrc;
	} 
	if( -1==nega )
		value = -value;
		
	return value;
} 

/* 字符串转换成小数 */
double string_to_float(const char* pstr, unsigned char* b) 
{
	char nega = 1;    /* 负数为1， 正数为1 */ 
	const char* psrc = (void*)0;
	double value = 0.0;
	double lvalue = 0.0; /* 小数点后面数字 */ 
	unsigned char dotflag = 0;  /* 小数点标记  1为之前出现过小数点 */ 
	unsigned char j = 0;
	
	if( (void*)0==pstr ) 
	{
		*b = 0; 
		return 0;
	}
	
	psrc = pstr;
	if( !string_check_digital(psrc) ) 
	{
		*b = 0; 
		return 0;
	}
	
	if('-'==*psrc)  /* 符号位 */ 
	{
		nega = -1;
		++psrc;
	}	
	
	while('\0'!=*psrc)
	{
		if('-'==*psrc ) return value;
		if('.'==*psrc ) 
		{
			dotflag = 1;
			++psrc;
			continue;
		}
		
		if(!dotflag) 
		{
			value = 10*value + CHAR_TO_DIGITAL(*psrc);	
		}else
		{
			++j;
			lvalue = lvalue + CHAR_TO_DIGITAL(*psrc)*double_pow(0.1, j);
		}	
		++psrc;
	} 
	if( -1==nega )
		value = -(value+lvalue);
	else value = value+lvalue;
		
	return value;
}

/* 浮点数转换为字符串，包括整数转换为字符串 
*  intgr指定整数位个数，dec指定小数位个数 
*  自动去除前面的0，小数点后面的0不会舍去 
*/ 
unsigned char float_to_string(double value, char* pdest, unsigned int intgr, unsigned int dec)
{
	char* pstr = (void*)0;
	double fvalue = 0.0;
	char c = 0;
	int tvalue = 0;
	unsigned char zeroflag = 0;
	
	if( (void*)0==pdest || 0==intgr ) return 0;
	
	if(1==intgr) zeroflag = 1;
	
	pstr = pdest;
	if(value<-0.000000000000000001)
	{
		*pstr = '-';
		++pstr;
		value = -value;
	}
	
	tvalue = (int)value%int_pow(10,intgr);
	while(intgr)
	{
		c = DIGITAL_TO_CHAR(tvalue/int_pow(10,intgr-1));
		
		if( !zeroflag  && '0'==c )
		{
			tvalue = tvalue%int_pow(10,intgr-1);
			--intgr;
			continue;
		}
		
		zeroflag = 1;
		
		*pstr = c;
		tvalue = tvalue%int_pow(10,intgr-1);
		--intgr;
		++pstr;
	}
	
	if( !zeroflag ) *pstr++ = '0';
		
		
	/* 如果小数位数为0，则返回整数部分 */
	if(0==dec)
	{
		*pstr = '\0';
		return 1;		
	}
	
	*pstr++ = '.';	
	tvalue = (int)(value*int_pow(10,dec))%int_pow(10,dec);
	while(dec)
	{
		*pstr = DIGITAL_TO_CHAR(tvalue/int_pow(10,dec-1));
		tvalue = tvalue%int_pow(10,dec-1);
		--dec;
		++pstr;
	}
	*pstr = '\0';
	return 1;	
} 


/* 整数转换成字符串，内部其实调用了 float_to_string */
unsigned char int_to_string(int value, char* pdest, unsigned int intgr) 
{
	return float_to_string((double)value, pdest, intgr,0);
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


