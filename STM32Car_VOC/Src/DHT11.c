
#include "DHT11.h"

/* ������������\���ģʽ */
void DHT11_io_in_config(void) 
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void DHT11_io_out_config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;   
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

unsigned char DHT11_start(DHT11* dht)
{
	DHT11_io_out_config();  /* ��������Ϊ���ģʽ */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);  /* ����͵�ƽ */
	osDelay(30);   /* ����18ms�͵�ƽ */
	DHT11_io_in_config(); /* ��������ģʽ���ȴ�ģ���������,��ʱ���ݵ�ƽ��������������*/
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
	
	dht->status = 0;
	dht->time_count = 0;
	dht->start_falg = 1;
	dht->data_start_flag = 0;
	return 1;
}

void DHT11_data_process(DHT11* dht)
{
	unsigned char success = 1;
	if(0==dht) return;
		
	if(dht->start_falg)
	{
		switch(dht->status)
		{
			/* ʶ��Ӧ���ź� */
			case DHT11_NONE:
				if(0==dht->time_count && GPIO_PIN_SET==HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11))
				{
					break;
				}
				dht->data_start_flag = 1;
				if(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11))
				{
					++dht->time_count;
				}else
				{
					if( dht->time_count>=7 && dht->time_count<=8 )  /* Ӧ���ź�Ϊ80us�͵�ƽ */
					{
						dht->time_count = 0;
						dht->status = DHT11_START;
					}else
					{
						success = 0;
					}
				}
				break;
			
			/* ʶ�����ݿ�ʼ���� */				
			case DHT11_START:
				if(GPIO_PIN_SET==HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11))
				{
					++dht->time_count;
				}else
				{
					if( dht->time_count>=7 && dht->time_count<=8 )  /* ���ݿ�ʼ����ź�Ϊ80us�ߵ�ƽ */
					{
						dht->time_count = 0;
						dht->status = DHT11_DATA;
					}else
					{
						success = 0;
					}
				}
				break;
			
			/* ʶ����Ч���ݣ�����Ϊ40 50us��26-28us�ߴ���0��50us��70us�ߴ���1*/
			case DHT11_DATA:
				if(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11))
				{
					++dht->time_count;
				}else
				{
					if( dht->time_count>=4 && dht->time_count<=6 )
					{
						dht->time_count = 0;
						dht->status = DHT11_DATA_1;
					}else
					{
						success = 0;
					}
				}
				break;
				
			case DHT11_DATA_1:
				if(GPIO_PIN_SET==HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11))
				{
					++dht->time_count;
				}else
				{
					if( dht->time_count>=2 && dht->time_count<=4 ) 
					{
						dht->time_count = 0;
						dht->status = DHT11_DATA;
						
						dht->data[dht->data_count++/8] |= 0<<(dht->data_count++%8);
					}else if(dht->time_count>=6 && dht->time_count<=8)
					{
						dht->time_count = 0;
						dht->status = DHT11_DATA;	
						
						dht->data[dht->data_count++%8] |= 1<<(dht->data_count++%8);
					}else
					{
						success = 0;
					}
					if(MAX_DATA_BIT_COUNT==dht->data_count)				
					{
						dht->status = DHT11_DATA_END;
					}
				}
				break;
			case DHT11_DATA_END:
				if(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11))  /* 50us�͵�ƽ�����ź� */
				{
					++dht->time_count;
				}else
				{
					if( dht->time_count>=4 && dht->time_count<=6 )
					{
						dht->time_count = 0;						
						if( (unsigned char)(dht->data[0]+dht->data[1]+dht->data[2]+dht->data[3])==(unsigned char)dht->data[4] )  /* У������ */
						{
							dht->valid = 1;
							dht->start_falg = 0;
							dht->status = DHT11_NONE;
							dht->data_start_flag = 0;
						}
						else
							success = 0;
					}else
					{
						success = 0;
					}
				}
				break;
			default:
				break;
			}
			if(!success)
			{
				dht->data_count = 0;
				dht->start_falg = 0;
				dht->status = 0;
				dht->valid = 0;
				dht->time_count = 0;
				dht->data_start_flag = 0;
		}
	}
}

DHT11* get_dht11(void)
{
	static DHT11 dht = {0,0,0,0,0,0,0,0,0,0};
	
	return &dht;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
//	DHT11_data_process(get_dht11());
}
