#include "AwesomeDataProcess.h"

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

