#include "AwesomeDataProcess.h"

/* ��������̬���� */

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

/* ����ַ����Ƿ�Ϊ�����ַ������Ƿ���1�����򷵻�0 */ 
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

/************  �ڴ洦����   *************/

/* ��ʼ���ڴ�ռ� 
*  ��ʽ������
*		char* src -> ����ʼ���Ļ�����
*		unsigned int len -> ���������ݿռ��С
*		char value -> ��ʼ��ֵ
*  ���ز����� ������Ч����true����Ч����false 
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

/*************** �ַ��������� *************/


/*  �ַ������ȼ��㣬��'\0'Ϊ������ 
*   ��������� 
*		const char* str -> ���������ַ������ȵ�ԭ�ַ��� 
*   ����ֵ�� 
*		�ַ�������,�������ַ���������'\0' 
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

/* �ַ�������
*  ��ԭʼ�ַ������Ƶ���һ���ַ����У�������Ϊ'\0' 
*  �豣��Ŀ���ַ����ռ��㹻
* 
*  ��ʽ������
*		char* dest -> Ŀ���ַ���
* 		const char* src -> ԭʼ�ַ���
*  ���Ƴɹ��ɹ������棬 ���򷵻ؼ� 
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

/* ����һ�����ȵ��ַ��� 
*  ��ԭʼ�ַ������Ƶ���һ���ַ����У�������Ϊ'\0'���߸��Ƴ����㹻 
*  �豣��Ŀ���ַ����ռ��㹻
* 
*  ��ʽ������
*		char* dest -> Ŀ���ַ���
* 		const char* src -> ԭʼ�ַ���
*		unsigend int len -> �����ַ����ĳ��� 
*  ���Ƴɹ��ɹ������棬 ���򷵻ؼ� 
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


/************* ����ת������ ****************/ 

/* ʮ�������ַ�ת��Ϊ���� */
unsigned char data_hex_to_uint(char a)
{
	if (a >= 'A' && a <= 'F')
			return a - 'A' + 10;
	else if (a >= 'a' && a <= 'f')
			return a - 'a' + 10;
	else
			return a - '0';
}

/* �����ַ���������,ʶ����Ų��Ҳ�������
*  �����С������ֻ������������,���ת����������0 
*  ��������� ԭʼ�ַ����� ת���ɹ���־ 0-����  1-��ȷ 
*/
int sring_to_int(const char* pstr, unsigned char* b)
{
	char nega = 1;    /* ����Ϊ1�� ����Ϊ1 */ 
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
	if('-'==*psrc)  /* ����λ */ 
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

/* �ַ���ת����С�� */
double string_to_float(const char* pstr, unsigned char* b) 
{
	char nega = 1;    /* ����Ϊ1�� ����Ϊ1 */ 
	const char* psrc = (void*)0;
	double value = 0.0;
	double lvalue = 0.0; /* С����������� */ 
	unsigned char dotflag = 0;  /* С������  1Ϊ֮ǰ���ֹ�С���� */ 
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
	
	if('-'==*psrc)  /* ����λ */ 
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

/* ������ת��Ϊ�ַ�������������ת��Ϊ�ַ��� 
*  intgrָ������λ������decָ��С��λ���� 
*  �Զ�ȥ��ǰ���0��С��������0������ȥ 
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
		
		
	/* ���С��λ��Ϊ0���򷵻��������� */
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


/* ����ת�����ַ������ڲ���ʵ������ float_to_string */
unsigned char int_to_string(int value, char* pdest, unsigned int intgr) 
{
	return float_to_string((double)value, pdest, intgr,0);
}


/************  �����㷨����    **************/

/* nmea0183Э�����ݷ����֡ͷ'$',������'*'��У��λ
*  У��λ���󷨣�$��*֮����ַ�������Ľ���ٻ����16�����ַ���ʾ 
*  ��ʽ������ char* dest -> ������ɴ洢���ݻ�����
*			  char* src  -> ����װ���� 
*  ���ز����� ������Ч����false���ɹ�����true 
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


