
#include "DHT11.h"

static __IO unsigned int system_time = 0;

/* ��ʼ��DHT11 */
void DHT11_init(DHT11* pdht)
{
	if(!pdht) return;
	
	pdht->TEMP = 0;
	pdht->RH = 0;
	pdht->valid = 0;
}

/* ���ø������� */
void DHT11_io_in_config(DHT11* pdht)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.Pin = pdht->GPIO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(pdht->GPIOx, &GPIO_InitStruct);
}

/* ���ÿ�©��� */
void DHT11_io_out_config(DHT11* pdht)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.Pin = pdht->GPIO_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(pdht->GPIOx, &GPIO_InitStruct);
}

/* ���Ϳ�ʼʱ�� 22ms�͵�ƽ��������20us,������������ģʽ��ȡ�˿� */
unsigned char DHT11_start(DHT11* dht)
{
	unsigned int timeout = 0;
	dht->valid = 0;
	
	DHT11_io_out_config(dht);  /* ������� */
	HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, GPIO_PIN_SET); /* Ĭ��������ߵ�ƽ */
	
	/* ���͵�ƽ20ms */
	osDelay(2);
	HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, GPIO_PIN_RESET); 
	osDelay(20);
	
	/* ���ߵ�ƽ20us */
//	HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, GPIO_PIN_SET);
	DHT11_io_in_config(dht);
	delay10us(3ul);
		
	/* �ȴ�DHT11ģ��Ӧ���ź� */
	timeout = 20000;
	while( HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin) )
	{
		--timeout;
		if(!timeout) return 0;
	}
	
	/* DHT11Ӧ���ź�80us */
	timeout = 30000;
	delay10us(2ul);
	while( !HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin) )
	{
		--timeout;
		if(!timeout) return 0;
	}
	
	/* DHT11����׼�� */
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

/* ��ȡ40bit���� */
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
			/* �ȴ��͵�ƽ��ɸߵ�ƽ */
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
			delay10us(4ul); /* ��ʱ40us��ȡ��ƽ״̬ */
			if(!HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin))
			{
				/* ����λΪ0 */
				temp = 0;
			}else
			{
				/* ����λΪ1 */
				temp = 1;
				
				/* �ȴ��ߵ�ƽ��ɵ͵�ƽ */
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


