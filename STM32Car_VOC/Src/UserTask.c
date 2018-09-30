#include "UserTask.h"

//static osThreadId feed_dog_task_handle;
//static osThreadId led_blink_task_handle;
//static osThreadId rgb_blink_task_handle;
//static osThreadId beep_task_handle;

void app_run(void)
{
	init_system();
	/* 喂狗任务 */
	osThreadDef(FeedDogThread, feed_dog_task, osPriorityIdle, 0, 128);
	osThreadCreate(osThread(FeedDogThread), NULL);	
	
	/* led闪烁任务 */
	osThreadDef(LedBlinkThread, led_blink_task, osPriorityIdle, 0, 128);
	osThreadCreate(osThread(LedBlinkThread), NULL);	
	
	/* RGB闪烁任务 */
	osThreadDef(RGBBlinkThread, rgb_blink_task, osPriorityIdle, 0, 128);
	osThreadCreate(osThread(RGBBlinkThread), NULL);

	/* 蜂鸣器鸣叫任务 */
//	osThreadDef(BeepBlinkThread, beep_task, osPriorityIdle, 0, 128);
//	beep_task_handle = osThreadCreate(osThread(BeepBlinkThread), NULL);
	
	/* 串口1接收数据处理任务 */
	osThreadDef(UART1RXTask, usart1_receive_task, osPriorityLow, 0, 256);
	osThreadCreate(osThread(UART1RXTask), NULL);
	/* 串口2接收数据处理任务 */
	osThreadDef(UART2RXTask, usart2_receive_task, osPriorityLow, 0, 256);
	osThreadCreate(osThread(UART2RXTask), NULL);
	
	/* 串口1接收数据处理任务 */
	osThreadDef(UART1TXTask, usart1_send_task, osPriorityBelowNormal, 0, 256);
	osThreadCreate(osThread(UART1TXTask), NULL);
	/* 串口2接收数据处理任务 */
	osThreadDef(UART2TXTask, usart2_send_task, osPriorityBelowNormal, 0, 256);
	osThreadCreate(osThread(UART2TXTask), NULL);
	
}


/* 看门狗喂狗任务 */
static void feed_dog_task(void const* arg)
{
	while(1)
	{
		HAL_IWDG_Refresh(&hiwdg);
		osDelay(200);  /* 200ms feed the dog, avoid task wait to long time. if no feed dog, the system reset */
	}
}
/* function code end */

/* 系统运行LED闪烁任务 */
static void led_blink_task(void const* arg)
{
	while(1)
	{
		set_run_led();
		osDelay(50);	
		reset_run_led();
		osDelay(1000); 
	}
}
/* function code end */

/* RGB灯显示/关闭任务 */
static void rgb_blink_task(void const* arg)
{
	unsigned int count = 0;
	
	RGB1_Close();
	RGB2_Close();
	
	while(1)
	{
		osDelay(500);  /* 10Hz */
		
		/*
		switch(count++%5)
		{
			case 0:
				RGB1_Red();
				RGB2_Red();
				break;
			case 1:
				RGB1_Green();
				RGB2_Green();
				break;
			case 2:
				RGB1_Blue();
				RGB2_Blue();
				break;
			case 3:
				RGB1_Yellow();
				RGB2_Yellow();
				break;
			case 4:
				RGB1_Purple();
				RGB2_Purple();
				break;			
			default:
				RGB1_Close();
				RGB2_Close();
				break;
		}
		*/
	}
}
/* function code end */

/* 蜂鸣器鸣叫任务 */
static void beep_task(void const* arg)
{
	while(1)
	{	
		set_beep();
		osDelay(500);
		reset_beep();
		osDelay(2000);
	}
}
/* function code end */

/* 串口1接收处理任务 */
static void usart1_receive_task(void const* arg)
{
	unsigned int data_len = 0;
	char buff[128];
	
	while(1)
	{
		osDelay(20);
		data_len = readBuffLen(USART1_ID); /* 读取串口1缓冲队列中的数据长度 */
		if(data_len>0)
		{
			if(data_len>128) data_len = 128;			
			read(USART1_ID, &buff[0], data_len);
			
			/* 解析数据 */
			write(USART2_ID, &buff[0], data_len);
		}
	}
}
/* function code end */

/* 串口1发送任务 */
static void usart1_send_task(void const* arg)
{
	uint16_t data_len = 0;
	LoopQueue* sendQueue;
	char send_buff[128];
	
	while(1)
	{
		osDelay(50);	

		//HAL_UART_Transmit(&huart1, "Hello World-1\n", sizeof("Hello World-1\n"), 1000);
		
		xSemaphoreTake( mutex_usart1_tx, portMAX_DELAY );		
		sendQueue = getUsartSendLoopQueue(USART1_ID); /* get send queue */
		if(sendQueue!=NULL)
		{		
			data_len = writeBuffLen(USART1_ID); /* send queue data count */
			if(data_len>0)
			{
				unsigned int i = 0;
				if(data_len>128) data_len=128;
				for( i=0; i<data_len; ++i)
				{
					send_buff[i] = readCharLoopQueue(sendQueue);
				}
				HAL_UART_Transmit_DMA(&huart1, (uint8_t *)send_buff, (uint16_t)data_len); /* DMA send	*/
			}
		}		
		xSemaphoreGive(mutex_usart1_tx);
	}
}
/* function code end */

/* 串口2发送任务 */
static void usart2_send_task(void const* arg)
{
	uint16_t data_len = 0;
	LoopQueue* sendQueue;
	char send_buff[128];
	
	while(1)
	{
		osDelay(50);		
		
		//HAL_UART_Transmit(&huart2, "Hello World-2\n", sizeof("Hello World-1\n"), 1000);
		
		xSemaphoreTake( mutex_usart2_tx, portMAX_DELAY );		
		sendQueue = getUsartSendLoopQueue(USART2_ID); /* get send queue */
		if(sendQueue!=NULL)
		{		
			data_len = writeBuffLen(USART2_ID); /* send queue data count */
			if(data_len>0)
			{
				unsigned int i = 0;
				if(data_len>128) data_len=128;
				for( i=0; i<data_len; ++i)
				{
					send_buff[i] = readCharLoopQueue(sendQueue);
				}
				HAL_UART_Transmit_DMA(&huart2, (uint8_t *)send_buff, (uint16_t)data_len); /* DMA send	*/
			}
		}		
		xSemaphoreGive(mutex_usart2_tx);
	}
}
/* function code end */

/* 串口2接收处理任务 */
static void usart2_receive_task(void const* arg)
{
	unsigned int data_len = 0;
	char buff[128];
	while(1)
	{
		osDelay(30);
		data_len = readBuffLen(USART2_ID); /* 读取串口1缓冲队列中的数据长度 */
		if(data_len>0)
		{
			if(data_len>128) data_len = 128;			
			read(USART2_ID, &buff[0], data_len);
			
			/* 解析数据 */
			//write(USART2_ID, &buff[0], data_len);
		}
	}
}
/* function code end */

/* 初始化系统函数 */
static void init_system(void)
{
	initUsartBuff(USART2_ID);
	initUsartBuff(USART1_ID);
	mutex_usart1_tx = xSemaphoreCreateMutex();
	mutex_usart2_tx = xSemaphoreCreateMutex();
	
	initUsartIT(&huart1);
	initUsartIT(&huart2);
	
	write(USART1_ID, "USART1 ENBALE\n", sizeof("USART1 ENBALE\n")/sizeof(char));
	write(USART2_ID, "USART2 ENBALE\n", sizeof("USART2 ENBALE\n")/sizeof(char));
}
/* function code end */

/* 初始化串口接收中断 */
static void initUsartIT(UART_HandleTypeDef *huart)
{
	if(huart==0) return;

	huart->ErrorCode = HAL_UART_ERROR_NONE;
	huart->RxState = HAL_UART_STATE_BUSY_RX;

	/* Process Unlocked */
	__HAL_UNLOCK(huart);

	/* Enable the UART Parity Error Interrupt */
	__HAL_UART_ENABLE_IT(huart, UART_IT_PE);

	/* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
	__HAL_UART_ENABLE_IT(huart, UART_IT_ERR);

	/* Enable the UART Data Register not empty Interrupt */
	__HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
	
	return;
}
/* function code end */
