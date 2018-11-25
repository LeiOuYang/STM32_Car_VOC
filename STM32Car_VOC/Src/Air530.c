
#include "air530.h"

const char AIR530_config[] = "$PGKC115,1,0,1,0*2A\r\n$PGKC242,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*36\r\n";  /* 默认配置GPS+BEIDOU,輸出GGA、VTG、RMC字段 */

/* 校验和带原始字符串返回 */
unsigned char air530_crc_str(char* dest, const char* src)
{
	unsigned crc = 0;
	
	if(0==dest || 0==src) return 0;
	
	*dest++ =  '$';
	
	while(*src)
	{		
		*dest = *src;
		crc ^= (unsigned char)*src;
		++src;
		++dest;
	}
	*dest++ = '*';
	*dest++ = NUM_TO_HEX(crc/16);
	*dest++ = NUM_TO_HEX(crc%16);
	*dest++ = '\r';
	*dest++ = '\n';
	*dest = '\0';
	
	return 1;
}

/* 配置GPS模块输出 */
void air530_config(UART_HandleTypeDef* huart)
{
	unsigned int count = 0;
	while(huart->gState==HAL_UART_STATE_BUSY_TX && count++<10000);
	
	HAL_UART_Transmit_DMA(huart,(uint8_t*)AIR530_config, strlen(AIR530_config));
	osDelay(10);
}
