
#include "UserTask.h"

//static osThreadId feed_dog_task_handle;
//static osThreadId led_blink_task_handle;
//static osThreadId rgb_blink_task_handle;
//static osThreadId beep_task_handle;

static system_flag sys_flag = { 0,0,0,0,0,1,1,0,0,0,0};
xQueueHandle button_event_queue;
extern const unsigned char chinese16_16[][32];
static DHT11 dht11;
static LCD lcd;

void app_run(void)
{
	init_system();
	/* ι������ */
	osThreadDef(FeedDogThread, feed_dog_task, osPriorityIdle, 0, 128);
	osThreadCreate(osThread(FeedDogThread), NULL);	
	
	/* led��˸���� */
	osThreadDef(LedBlinkThread, led_blink_task, osPriorityIdle, 0, 128);
	osThreadCreate(osThread(LedBlinkThread), NULL);	
	
	/* RGB��˸���� */
	osThreadDef(RGBBlinkThread, rgb_blink_task, osPriorityIdle, 0, 128);
	osThreadCreate(osThread(RGBBlinkThread), NULL);
	
	/* ����������ʾOLED��ʾ��Ļ */
	osThreadDef(OledTask, update_oled_task, osPriorityIdle, 0, 128);
	osThreadCreate(osThread(OledTask), NULL);	

	/* �������������� */
	osThreadDef(BeepBlinkThread, beep_task, osPriorityIdle, 0, 128);
	osThreadCreate(osThread(BeepBlinkThread), NULL);
	
	/* ����1�������ݴ������� */
	osThreadDef(UART1RXTask, usart1_receive_task, osPriorityLow, 0, 256);
	osThreadCreate(osThread(UART1RXTask), NULL);
	/* ����2�������ݴ������� */
	osThreadDef(UART2RXTask, usart2_receive_task, osPriorityLow, 0, 256);
	osThreadCreate(osThread(UART2RXTask), NULL);
	
	/* ����1�������ݴ������� */
	osThreadDef(UART1TXTask, usart1_send_task, osPriorityBelowNormal, 0, 256);
	osThreadCreate(osThread(UART1TXTask), NULL);
	/* ����2�������ݴ������� */
	osThreadDef(UART2TXTask, usart2_send_task, osPriorityBelowNormal, 0, 512);
	osThreadCreate(osThread(UART2TXTask), NULL);
	
	/* DHT11���ݴ������� */
	osThreadDef(DHT11Task, dht11_process_task, osPriorityRealtime, 0, 256);
	osThreadCreate(osThread(DHT11Task), NULL);
	
	/* �������д������� */
	osThreadDef(ButtonTask, button_event_task, osPriorityHigh, 0, 128);
	osThreadCreate(osThread(ButtonTask), NULL);	
	
}

/* ��ʪ�����ݴ��� */
static void dht11_process_task(void* arg)
{
	unsigned char count = 0;
	osDelay(1500);  /* ��ʱ1.5S�ȴ�ģ���ȶ� */
	HAL_TIM_Base_Start_IT(&htim2); /* ������ʱ��������10us���붨ʱ���ж�*/
	
	while(1)
	{
		if( !dht11.reading )  
		{
//			DHT11_read_data(&dht11);
			if( !DHT11_read_data(&dht11) )
			{
				++count;
				if(count>=4)
				{
					count = 4;
					dht11.exist = 0;
					sys_flag.sensor_healthy &= ~SENSOR_DHT11_HEALTHY;
					
				}
			}else
			{
				count = 0;	
				sys_flag.sensor_healthy |= SENSOR_DHT11_HEALTHY;
			}
		}
			
//		xSemaphoreTake( mutex_usart2_tx, portMAX_DELAY );	
//		usart_lcd_display();
//		xSemaphoreGive(mutex_usart2_tx);
			
		osDelay(3000);  /* ÿ2���ȡһ����ʪ������ */
	}	
}
/* function code end */


/* ���Ź�ι������ */
static void feed_dog_task(void const* arg)
{
	while(1)
	{
		HAL_IWDG_Refresh(&hiwdg);
		update_sensor_status();
		osDelay(100);  /* 200ms feed the dog, avoid task wait to long time. if no feed dog, the system reset */
	}
}
/* function code end */

/* ϵͳ����LED��˸���� */
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

/* RGB����ʾ/�ر����� */
static void rgb_blink_task(void const* arg)
{
	unsigned int step = 0;
	unsigned int count = 0;
	
	RGB1_Close();
	RGB2_Close();
	
	while(1)
	{
		osDelay(20);  /* 50hz */
		
		if(!sys_flag.open_rgb && 0==sys_flag.rgb_sw)
		{
			RGB1_Close();
			RGB2_Close();
			continue;
		}
		
		if(sys_flag.rgb_sw)
		{
			switch(sys_flag.rgb_list)
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
				case 5:
					RGB1_Close();
					RGB2_Close();
					break;
				default:
					RGB1_Close();
					RGB2_Close();
					break;
			}
		}
		
		if(p_air_sensor->init && p_air_sensor->health && 0==sys_flag.rgb_sw)
		{
			switch(sys_flag.tvoc_level)
			{
				case TVOC_PPM_00:  /* �ȼ�0  <1.5ppm */
					RGB1_Green();
					RGB2_Green();
					break;
				case TVOC_PPM_01: /* �ȼ�1  1.5~5.5ppm */
					 RGB1_Blue();
					 RGB2_Blue();
					break;
				case TVOC_PPM_02:  /* �ȼ�2  5.5~10ppm */
					RGB1_Purple();
					RGB2_Purple();				
					break;
				case TVOC_PPM_03:  /* �ȼ�3  >10ppm */
					RGB1_Yellow();
					RGB2_Yellow();
					break;
				case TVOC_PPM_DANGER:
//					RGB1_Red();
//					RGB2_Red();
					break;
				default:
					RGB1_Close();
					RGB2_Close();
					break;
			}
			
			step++;
			switch(step)
			{
					case 0:
					case 2:
						if(TVOC_PPM_DANGER==sys_flag.tvoc_level)
						{
							RGB1_Red();
							RGB2_Red();
						}
						break;
					case 1:
					case 3:
						if(TVOC_PPM_DANGER==sys_flag.tvoc_level)
						{
							RGB1_Close();
							RGB2_Close();
						}				
						break;				
					default:
						step = 0;
						break;
			}		
		}
	}
}
/* function code end */

/* �������������� */
static void beep_task(void const* arg)
{
	while(1)
	{	
		osDelay(100);
		
		if(!sys_flag.open_beep)
		{
			reset_beep();
			continue;
		}
		if(sys_flag.tvoc_level>=TVOC_PPM_03 && p_air_sensor->init)
		{
			set_beep();
			osDelay(500);
			reset_beep();
			osDelay(2000);
		}
	}
}
/* function code end */

/* ����1���մ������� */
static void usart1_receive_task(void const* arg)
{
	unsigned int data_len = 0;
	char buff[128];
	unsigned int i = 0;
	unsigned char count = 0;
	
	while(1)
	{
		osDelay(20);
		data_len = readBuffLen(USART1_ID); /* ��ȡ����1��������е����ݳ��� */
		if(data_len>0)
		{
			if(data_len>128) data_len = 128;			
			read(USART1_ID, &buff[0], data_len);
			
			/* �������� */
			for(i=0; i<data_len; ++i)
			{
				count = 0;
				p_air_sensor->update(buff[i]);
				if(p_air_sensor->health && !p_air_sensor->error)
				{
					/* ����ȼ� */
					sys_flag.tvoc_level = p_air_sensor->convert_level((unsigned int)(p_air_sensor->air_ppm*10));
					sys_flag.tvoc_ppm = (unsigned char)p_air_sensor->air_ppm;
					sys_flag.oled_update_area |= OLED_UPDATE_AREA_AIR_YES;
					
//					write(USART2_ID, &buff[0], data_len);
				}
			}
		}else if(0==data_len)
		{
			count++;
			if(count>=250)
			{
				count = 250;
				sys_flag.sensor_healthy &= ~SENSOR_TVOC_HEALTHY;
			}
		}
	}
}
/* function code end */

/* ����1�������� */
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

/* ����2�������� */
static void usart2_send_task(void const* arg)
{
	uint16_t data_len = 0;
	LoopQueue* sendQueue;
	char send_buff[256];
	float speed = 0;
	unsigned char gps = 0;
	unsigned char error = 0;

	osDelay(2000);
	HAL_UART_Transmit(&huart2, get_lcd_str(), strlen(get_lcd_str()), 1000);
	osDelay(1000);
	HAL_UART_Transmit(&huart2, get_lcd_str(), strlen(get_lcd_str()), 1000);
	
	while(1)
	{
		osDelay(1000);		
//		xSemaphoreTake( mutex_usart2_tx, portMAX_DELAY );
		
		/* ��ʪ�ȸ�����ʾ */
		if(dht11.exist)
		{			
			if(dht11.valid)
			{
				dht11.reading = 1;
				
				osDelay(100);	
				usart_lcd_display_temp(dht11.TEMP, &lcd, 60, 14);
				HAL_UART_Transmit(&huart2, lcd.display_buff, strlen(lcd.display_buff), 150);

				osDelay(100);
				usart_lcd_display_RH(dht11.RH, &lcd, 60, 14);
				HAL_UART_Transmit(&huart2, lcd.display_buff, strlen(lcd.display_buff), 150);
				
				dht11.reading = 0;
				
			}
		}
		
		/* ����TVOC������������ʾ */
		if(p_air_sensor->init)
		{
			osDelay(100);
			usart_lcd_display_TVOC(p_air_sensor->air_ppm, &lcd, 60, 14);
			HAL_UART_Transmit(&huart2, lcd.display_buff, strlen(lcd.display_buff), 150);
			
			osDelay(100);
			usart_lcd_display_airq(sys_flag.tvoc_level, &lcd, 60, 14);
			HAL_UART_Transmit(&huart2, lcd.display_buff, strlen(lcd.display_buff), 150);
		}
		
		osDelay(100);
		gps += 1;
		usart_lcd_display_GPS(gps, &lcd, 60, 3);
		HAL_UART_Transmit(&huart2, lcd.display_buff, strlen(lcd.display_buff), 150);
		
		osDelay(100);
		++speed;
		usart_lcd_display_speed(speed, &lcd,60, 45);
		HAL_UART_Transmit(&huart2, lcd.display_buff, strlen(lcd.display_buff), 150);
		
		osDelay(100);
		usart_lcd_display_error(error++, &lcd,60, 1);
		HAL_UART_Transmit(&huart2, lcd.display_buff, strlen(lcd.display_buff), 150);
	
//		sendQueue = getUsartSendLoopQueue(USART2_ID); /* get send queue */
//		if(sendQueue!=NULL)
//		{		
//			data_len = writeBuffLen(USART2_ID); /* send queue data count */
//			if(data_len>0)
//			{
//				unsigned int i = 0;
//				if(data_len>256) data_len=256;
//				for( i=0; i<data_len; ++i)
//				{
//					send_buff[i] = readCharLoopQueue(sendQueue);
//				}
//				HAL_UART_Transmit_DMA(&huart2, (uint8_t *)send_buff, (uint16_t)data_len); /* DMA send	*/
//			}
//		}		
//		xSemaphoreGive(mutex_usart2_tx);
	}
}
/* function code end */

/* ����2���մ������� */
static void usart2_receive_task(void const* arg)
{
	unsigned int data_len = 0;
	char buff[128];
	while(1)
	{
		osDelay(30);
		data_len = readBuffLen(USART2_ID); /* ��ȡ����1��������е����ݳ��� */
		if(data_len>0)
		{
			if(data_len>128) data_len = 128;			
			read(USART2_ID, &buff[0], data_len);
			
			/* �������� */
			//write(USART2_ID, &buff[0], data_len);
		}
	}
}
/* function code end */

/* ����OLED��Ļ��ʾ */
static void update_oled_task(void const* arg)
{	
	char buff[10];
	const unsigned char *pchinese = 0;
	sys_flag.oled_update_area = 0;
		
	OLED_Init();
	OLED_Clear();	
	display_title();
	osDelay(5000);
	OLED_Clear();
	display_string_Font8_16(48, 0, "ppm");
	display_fuhao_Font8_16(64, 3, 1);
	display_string_Font8_16(48, 6, "% RH");
	display_chinese_font16_16(80,0,&chinese16_16[0][0]);
	display_chinese_font16_16(96,0,&chinese16_16[1][0]);
	OLED_ShowString(88,2,">>",8);
		
	while(1)
	{
		osDelay(1500);
		
		if((sys_flag.oled_update_area & OLED_AUTHOR_UPDATE_YES)>0)
		{
			OLED_Clear();	
			display_title();
			osDelay(10000);
			sys_flag.oled_update_area &= OLED_AUTHOR_UPDATE_NO;
			OLED_Clear();
			display_string_Font8_16(48, 0, "ppm");
			display_fuhao_Font8_16(48, 3, 1);
			display_string_Font8_16(32, 6, "% RH");
			display_chinese_font16_16(80,0,&chinese16_16[0][0]);
			display_chinese_font16_16(96,0,&chinese16_16[1][0]);
			OLED_ShowString(88,2,">>",8);
		}
		
		/*���PPM��ʾ����*/
		OLED_Clear_Area(0, 0, 48, 0);
		OLED_Clear_Area(0, 1, 48, 1);
				
		if(p_air_sensor->init)
		{
			float_to_string(p_air_sensor->air_ppm, (char*)buff, 4, 0);
			if(strlen(buff)==1)
			{
				display_string_Font16_16(16, 0, "0");
			}
			if((sys_flag.oled_update_area & OLED_UPDATE_AREA_AIR_YES)>0)  /* �ж��Ƿ���Ҫ���¿���������ʾ���� */
			{
				switch(sys_flag.tvoc_level)
				{
					case TVOC_PPM_00:  /* �ȼ�0  <1.5ppm */
						pchinese = &chinese16_16[2][0];
						break;
					case TVOC_PPM_01: /* �ȼ�1  1.5~5.5ppm */
						pchinese = &chinese16_16[4][0];
						break;
					case TVOC_PPM_02:  /* �ȼ�2  5.5~10ppm */
						pchinese = &chinese16_16[6][0];		
						break;
					case TVOC_PPM_03:  /* �ȼ�3  >10ppm */
						pchinese = &chinese16_16[8][0];
						break;
					case TVOC_PPM_DANGER:
						pchinese = &chinese16_16[8][0];
						break;
					default: pchinese = 0; break;
				}
			}
			display_string_Font16_16(48-strlen(buff)*16, 0, buff);
			if(0!=pchinese)
			{
				OLED_Clear_Area(80, 4, 128, 5);
				display_chinese_font16_16(80,4,pchinese);
				display_chinese_font16_16(96,4,pchinese+32);
			}
		}else
		{
			OLED_ShowString(16, 0, "--", 16);
		}
		
		if(dht11.exist)
		{			
			if(dht11.valid)
			{
				/*����¶���ʾ����*/
				OLED_Clear_Area(0, 4, 64, 4);
				OLED_Clear_Area(0, 3, 64, 3);

				/*����¶���ʾ����*/
				OLED_Clear_Area(0, 6, 48, 6);
				OLED_Clear_Area(0, 7, 48, 7);
				dht11.reading = 1;
				
				float_to_string(dht11.TEMP, (char*)buff, 3, 1);
//				display_string_Font8_16(8, 3, buff);
				display_string_Font16_16(64-strlen(buff)*16, 3, buff);
				
				float_to_string(dht11.RH, (char*)buff, 3, 0);
//				display_string_Font8_16(8, 6, buff);
				display_string_Font16_16(32-strlen(buff)*16, 6, buff);
				
				dht11.reading = 0;
				
			}
		}
		
		sensor_error_display();
	}
}
/* function code end */

/* ��ʼ��ϵͳ���� */
static void init_system(void)
{
	initUsartBuff(USART2_ID);
	initUsartBuff(USART1_ID);
	init_system_button();
	mutex_usart1_tx = xSemaphoreCreateMutex();
	mutex_usart2_tx = xSemaphoreCreateMutex();
	button_event_queue = xQueueCreate( 10, sizeof( Button* ));
	
	p_air_sensor = get_air_sensor(); 
	dht11.GPIOx = GPIOB;
	dht11.GPIO_Pin = GPIO_PIN_10;
	DHT11_init(&dht11);
	
	initUsartIT(&huart1);
	initUsartIT(&huart2);
	
//	write(USART1_ID, "USART1 ENBALE\n", sizeof("USART1 ENBALE\n")/sizeof(char));
//	write(USART2_ID, "USART2 ENBALE\n", sizeof("USART2 ENBALE\n")/sizeof(char));
	
	sys_flag.open_rgb = 0;
	sys_flag.sensor_healthy = 0;
}
/* function code end */

/* ����������� */
void button_event_task(void const* arg)
{
	init_system_button();
	Button* bt = 0;	
	unsigned int step = 0;
	
	while(1)
	{
		if( pdPASS==xQueueReceive( button_event_queue, &bt, 30 ) )
		{
			ButtonId id = (ButtonId)bt->id;
			
			if(BUTTON_STATUS_DOWN_START==bt->status) /* ʶ�𵽵�һ�ΰ��£��п��������� */
			{
				osDelay(10);    /* �������� */
				if(GPIO_PIN_RESET==read_button_pin_satus(id)) /* ȷ������ */
				{
					bt->status = BUTTON_STATUS_DOWN_ENTER;
					bt->clicked = 1;
				}else
				{
					bt->status = BUTTON_STATUS_NONE;
					bt->down_time = 0;
				}
			}
			else if(BUTTON_STATUS_UP_START==bt->status) /* �����ص� */
			{
				osDelay(10);    /* �������� */
				if(GPIO_PIN_SET==read_button_pin_satus(id)) 
				{
					bt->status = BUTTON_STATUS_UP_ENTER;
					bt->release = 1;
					
					/* �������ص��¼� */
					if(bt->down_time*30<500 && id==1)
					{
						sys_flag.button_click = 1;
						if(sys_flag.rgb_sw)
						{
							sys_flag.rgb_list++;
							sys_flag.rgb_list %= 5;
						}else
						{
							sys_flag.open_beep = !sys_flag.open_beep;
						}
					}
					
					if(bt->down_time*30<500 && id==0)
					{
						sys_flag.open_rgb = !sys_flag.open_rgb;
						sys_flag.button_click = 1;
					}
					/* end */
					bt->down_time = 0;
					bt->status = BUTTON_STATUS_NONE;
					bt->release = 0;
				}else
				{
					bt->status = BUTTON_STATUS_NONE;
					bt->down_time = 0;
				}
			}
		}
		else
		{
			Button *b = 0;
			unsigned char i = 0;
			for( ; i<BUTTON_MAX_NUM; ++i)
			{
				b = get_button_by_id((ButtonId)i);
				if(b->status==BUTTON_STATUS_DOWN_ENTER)
				{
					++b->down_time;
					if(b->down_time*30>=3000) /* ��ťһֱ����3�� */
					{
						/* �����¼� */
						if(b->down_time*30<3500)
						{
							
						}else if(b->down_time*30>5000)
						{
							if(0==i)
								sys_flag.oled_update_area |= OLED_AUTHOR_UPDATE_YES; 
							else if(1==i)
							{
								sys_flag.rgb_sw = !sys_flag.rgb_sw;
								if(sys_flag.rgb_sw)
								{
									sys_flag.rgb_list = 0;
								}
							}
							b->status = BUTTON_STATUS_NONE;
						}else if(b->down_time*30<3500)
						{
						}
					}else if(b->down_time*30>=5000)
					{
					}
				}
			}
		}
		
		if(sys_flag.button_click)
		{
			++step;
			if(step<5)
			{
				set_beep();
			}
			else
			{
				step = 0;
				reset_beep();
				sys_flag.button_click = 0;
			}			
		}
		
		/*���´�����������־*/
	}
}
/* function code end */ 

/* ��ʼ�����ڽ����ж� */
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

/* ���´�������־ */
static void update_sensor_status(void)
{
//	if(dht11.exist)
//	{
//		sys_flag.sensor_healthy |= SENSOR_DHT11_HEALTHY;
//	}else
//	{
//		sys_flag.sensor_healthy &= ~SENSOR_DHT11_HEALTHY;
//	}
	
	if(p_air_sensor->health)
	{
		sys_flag.sensor_healthy |= SENSOR_TVOC_HEALTHY;
	}else
	{
		sys_flag.sensor_healthy &= ~SENSOR_TVOC_HEALTHY;
	}
}

/* ������������ʾ */
static void sensor_error_display(void)
{
	OLED_Clear_Area(96, 6, 127, 6);
	OLED_Clear_Area(96, 7, 127, 7);
	
	if( (sys_flag.sensor_healthy&SENSOR_DHT11_HEALTHY)&&(sys_flag.sensor_healthy&SENSOR_TVOC_HEALTHY) )
		return;
	
	if( !(sys_flag.sensor_healthy&SENSOR_DHT11_HEALTHY) )
	{
		/*����¶���ʾ����*/
		OLED_Clear_Area(0, 4, 64, 4);
		OLED_Clear_Area(0, 3, 64, 3);

		/*����¶���ʾ����*/
		OLED_Clear_Area(0, 6, 48, 6);
		OLED_Clear_Area(0, 7, 48, 7);
		display_string_Font8_16(96, 6, "E2");
	}
	
	if( !(sys_flag.sensor_healthy&SENSOR_TVOC_HEALTHY) )
	{
		OLED_Clear_Area(0, 0, 48, 0);
		OLED_Clear_Area(0, 1, 48, 1);
		OLED_Clear_Area(80, 4, 128, 5);
		OLED_ShowString(16, 0, "--", 16);
		display_string_Font8_16(96, 6, "E1");
	}
	
	if( (!(sys_flag.sensor_healthy&SENSOR_DHT11_HEALTHY)) && (!(sys_flag.sensor_healthy&SENSOR_TVOC_HEALTHY)))
	{
		display_string_Font8_16(96, 6, "E3");
	}
}


/* function code end */
