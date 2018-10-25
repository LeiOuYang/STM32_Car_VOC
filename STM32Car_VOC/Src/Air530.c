
#include "air530.h"

const char hot_run[] = "$PGKC030,1,1*2C\r\n$PGKC115,1,0,1,0*2A\r\n";

/* 校验和带原始字符串返回 */
unsigned char air530_crc_str(char* dest, const char* src)
{
	unsigned char result = 0;
	char crc = 0;
	
	if(0==dest || 0==src) return 0;
	
	*dest++ =  '$';
	
	while(*src++)
	{		
		*dest++ = *src;
		crc ^= *src;
	}
	*dest++ = crc/10 + '0';
	*dest++ = crc%10 + '0';
	*dest = '\0';
	return 1;
}

void air530_config(UART_HandleTypeDef* huart)
{
	HAL_UART_Transmit_DMA(huart,(uint8_t*)hot_run, strlen(hot_run));
	osDelay(10);
}
