
#include "DHT11.h"

static __IO unsigned int system_time = 0;

/* 初始化DHT11 */
void DHT11_init(DHT11* pdht)
{
	if(!pdht) return;
	
	pdht->TEMP = 0;
	pdht->RH = 0;
	pdht->valid = 0;
}

/* 配置浮空输入 */
void DHT11_io_in_config(DHT11* pdht)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.Pin = pdht->GPIO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(pdht->GPIOx, &GPIO_InitStruct);
}

/* 配置开漏输出 */
void DHT11_io_out_config(DHT11* pdht)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.Pin = pdht->GPIO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(pdht->GPIOx, &GPIO_InitStruct);
}

/* 发送开始时序 22ms低电平，再拉高20us,接着配置输入模式读取端口 */
unsigned char DHT11_start(DHT11* dht)
{
	unsigned int timeout = 0;
	dht->valid = 0;
	
	DHT11_io_out_config(dht);  /* 配置输出 */
	HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, GPIO_PIN_SET); /* 默认先输出高电平 */
	
	/* 拉低电平20ms */
	osDelay(2);
	HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, GPIO_PIN_RESET); 
	osDelay(20);
	
	/* 拉高电平20us */
//	HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, GPIO_PIN_SET);
	DHT11_io_in_config(dht);
	delay10us(3ul);
		
	/* 等待DHT11模块应答信号 */
	timeout = 20000;
	while( HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin) )
	{
		--timeout;
		if(!timeout) return 0;
	}
	
	/* DHT11应答信号80us */
	timeout = 30000;
	delay10us(2ul);
	while( !HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin) )
	{
		--timeout;
		if(!timeout) return 0;
	}
	
	/* DHT11拉高准备 */
	timeout = 30000;
	delay10us(2ul);
	while( HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin) )
	{
		--timeout;
		if(!timeout) 
			return 0;
	}
	
	return 1;
}

/* 读取40bit数据 */
void DHT11_read_data(DHT11* dht)
{
	unsigned char t = 0;
	t = DHT11_start(dht);
	if( t )
	{
		unsigned char count = 0;
		unsigned char timeout = 0;
		unsigned char temp = 0;
//		taskENTER_CRITICAL();
		for(count=0; count<40; ++count)
		{
			/* 等待低电平变成高电平 */
			timeout = 30000;
			delay10us(2ul);
			while( !HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin) )
			{
				--timeout;
				if(!timeout)
				{					
					dht->valid = 0;
					return;
				}
			}
			delay10us(4ul); /* 延时40us读取电平状态 */
			if(!HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin))
			{
				/* 数据位为0 */
				temp = 0;
			}else
			{
				/* 数据位为1 */
				temp = 1;
				
				/* 等待高电平变成低电平 */
				timeout = 30000;
				while( HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin) )
				{
					--timeout;
					if(!timeout)
					{					
						dht->valid = 0;
						return;
					}
				}
			}
			dht->data[count/8] = ((dht->data[count/8])<<(count%8))|temp;			
		}
//		taskEXIT_CRITICAL();
		if((dht->data[0]+dht->data[1]+dht->data[2]+dht->data[3])==(dht->data[4]))
		{
			dht->valid = 1;
			dht->TEMP = dht->data[0];
			dht->TEMP += dht->data[1]&0x7F;
			if(dht->data[1]&0x80) dht->TEMP = -dht->TEMP;
			dht->RH = dht->data[2] + dht->data[3]*0.1;
		}
	}
}

void delay10us(unsigned int time)
{
	system_time = time;
	
	while(0!=system_time);
}

extern void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM2)
	{
		if(0!=system_time) 
			--system_time;
	}
}


