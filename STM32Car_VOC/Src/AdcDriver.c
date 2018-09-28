
#include "AdcDriver.h"

#define num_to_char(x) ("0123456789."[x])

static uint32_t adc1_dma_buff[BUFF_COUNT] = {1,2,3,4,5,6,7,8,9,10};

uint32_t* get_adc1_dma_buff(void)
{
	return adc1_dma_buff;
}

uint32_t get_adc1_buff_count(void)
{
	return BUFF_COUNT;
}

uint8_t get_adc1_channel_count(void)
{
	return CHANNEL_COUNT;
}

uint32_t average_filter(uint32_t* buff, uint16_t len)
{
	uint32_t max = 0;
	uint32_t min = 0;
	uint32_t sum = 0;
	uint32_t i = 0;
	uint32_t* p = 0;
	
	if(0==buff || 0==len)
		return 0;
	
	max = min = *buff;
	p = buff;
	
	for( i=0; i<len; ++i )
	{
		sum += *p;
		if(max<*p)
			max = *p;
		if(min>*p)
			min = *p;
		++p;
	}
	return (uint32_t)((sum-max-min)/(len-2));	
}

/* 获取内部温度 */
float get_adc1_temp(void)
{
	uint32_t buff[10] = {0,0,0,0,0,0,0,0,0,0};
	uint16_t i = 0;
	uint16_t j = CHANNEL_TEMPSENSOR;
	uint32_t avr = 0;
	
	for( ; i<10; ++i)
	{
		buff[i] = adc1_dma_buff[j]&((uint32_t)0x00ffff);
		j = j + CHANNEL_COUNT;
	}
	avr = average_filter(buff, 10);
	return (float)(((1.43-(3.3/4096.0)*avr)/4.3) + 25);    //(V 25 - V SENSE ) / Avg_Slope} + 25  VREFINT = 1.20V
}

/* 获取内部参考电压 */
float get_adc1_refint_vol(void)
{
	uint32_t buff[10] = {0,0,0,0,0,0,0,0,0,0};
	uint16_t i = 0;
	uint16_t j = CHANNEL_VREFINT;
	uint32_t avr = 0;
	
	for( ; i<10; ++i)
	{
		buff[i] = adc1_dma_buff[j]&((uint32_t)0x0ffff);
		j = j + CHANNEL_COUNT;
	}
	avr = average_filter(buff, 10);
	return (float)((1.2/avr)*4096);    //(V 25 - V SENSE ) / Avg_Slope} + 25  VREFINT = 1.20V
}


/* 指定整数位和小数点位数获取浮点字符串, ic-整数位数个数，dc-小数点位数，返回总位数, 限制整数9位，小数7位*/
//unsigned int float_to_string(double data, unsigned int ic, unsigned int dc, char* buff, unsigned int len)
//{
//	int intc = 0;
//	int dotc = 0;
//	unsigned int dotn = dc;
//	unsigned int intn = ic;
//	int i = 0;
//	int index = 0;
//	unsigned int temp = 0;
//	unsigned char zeroPass = 0;
//	unsigned char one = 0;
//	
//	if(0==buff || 0==len || 0==ic) return 0;
//	if(ic>9) intn = 9;
//	if(dc>7) dotn = 7;
//	if(ic+dc+1>len) 
//	{
//		dotn = len-1-ic;
//	}
//	
//	intc = (int)data;  /* 获取整数位 */
//	dotc = (int)((data-intc)*pow(10, dotn)); /* 将要保留的小数位转换成整数 */
//	
//	if(intc<0)
//	{
//		intc = -intc;
//		dotc = -dotc;
//		buff[0] = '-';
//		++index;
//	}
//	
//	if(intc>=1000000000) intc = 999999999;
//	
//	for(i=intn-1; i>0; --i)
//	{
//		temp = (unsigned int)pow(10, i);
//		one = (unsigned int)(intc/temp);
//		if(one>0 && 0==zeroPass)
//		{
//			zeroPass = 1;
//		}
//		if(1==zeroPass)
//		{
//			buff[index] = num_to_char(one);
//			++index;
//		}
//		intc = intc%temp;
//	}
//	
//	buff[index] = '.';
//	++index;
//	
//	for(i=dotn-1; i>0; --i)
//	{
//		temp = (unsigned int)pow(10, i);
//		one = (unsigned int)(dotc/temp);

//		buff[index] = num_to_char(one);
//		++index;

//		dotc = dotc%temp;
//	}
//	buff[index] = '\0';
//	
//	return index+1;
//	
//}

