
#include "DHT11.h"

/* 配置引脚输入\输出模式 */
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

void DHT11_start(DHT11* dht)
{
	DHT11_io_out_config();  /* 配置引脚为输出模式 */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);  /* 输出低电平 */
	osDelay(20);   /* 保持18ms低电平 */
	DHT11_io_in_config(); /* 设置输入模式，等待模块输出数据,此时数据电平被上拉电阻拉高*/
	dht->time_count = 0;
	while(GPIO_PIN_RESET!=HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11));
	dht->start_falg = 1;  /* 开始接收模块数据 */
}

void DHT11_data_process(DHT11* dht)
{
	unsigned char success = 1;
	if(0==dht) return;
		
	if(dht->start_falg)
	{
		switch(dht->status)
		{
			/* 识别应答信号 */
			case DHT11_NONE:
				if(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11))
				{
					++dht->time_count;
				}else
				{
					if( dht->time_count>76 && dht->time_count<84 )  /* 应答信号为80us低电平 */
					{
						dht->time_count = 0;
						dht->status = DHT11_START;
					}else
					{
						success = 0;
					}
				}
				break;
			
			/* 识别数据开始传输 */				
			case DHT11_START:
				if(GPIO_PIN_SET==HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11))
				{
					++dht->time_count;
				}else
				{
					if( dht->time_count>78 && dht->time_count<82 )  /* 数据开始输出信号为80us高电平 */
					{
						dht->time_count = 0;
						dht->status = DHT11_DATA;
					}else
					{
						success = 0;
					}
				}
				break;
			
			/* 识别有效数据，数量为40 50us低26-28us高代表0；50us低70us高代表1*/
			case DHT11_DATA:
				if(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11))
				{
					++dht->time_count;
				}else
				{
					if( dht->time_count>48 && dht->time_count<52 )
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
					if( dht->time_count>24 && dht->time_count<30 ) 
					{
						dht->time_count = 0;
						dht->status = DHT11_DATA;
						
						dht->data[dht->data_count++/8] |= 0<<(dht->data_count++%8);
					}else if(dht->time_count>68 && dht->time_count<72)
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
				if(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11))  /* 50us低电平结束信号 */
				{
					++dht->time_count;
				}else
				{
					if( dht->time_count>48 && dht->time_count<52 )
					{
						dht->time_count = 0;						
						if( (unsigned char)(dht->data[0]+dht->data[1]+dht->data[2]+dht->data[3])==(unsigned char)dht->data[4] )  /* 校验数据 */
						{
							dht->valid = 1;
							dht->start_falg = 0;
							dht->status = DHT11_NONE;
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
		}
	}
}
