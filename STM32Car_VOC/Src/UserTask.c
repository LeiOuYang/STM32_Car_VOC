
#include "UserTask.h"

#define AWESOME_DEBUG_GPS_ENABLE 0
#define AWESOME_DEBUG_TVOC_ENABLE 0

static system_flag sys_flag = { 0,0,0,(TVOC_PPM_STATUS)0,0,1,1,0,0,0,0};
xQueueHandle button_event_queue;
extern const unsigned char chinese16_16[][32];
static DHT11 dht11;
static LCD lcd;
static NMEA0183 nmea_data;

void app_run(void)
{
	init_system();
	/* 喂狗任务 */
	osThreadDef(Task50MSThread, task_50ms, osPriorityIdle, 0, 256);
	osThreadCreate(osThread(Task50MSThread), NULL);	
	
	/* RGB闪烁任务 */
	osThreadDef(RGBBlinkThread, rgb_blink_task, osPriorityIdle, 0, 128);
	osThreadCreate(osThread(RGBBlinkThread), NULL);
	
	/* 开启更新显示OLED显示屏幕 */
//	osThreadDef(OledTask, update_oled_task, osPriorityIdle, 0, 128);
//	osThreadCreate(osThread(OledTask), NULL);	

	/* gizwits通信模组接收数据处理任务 */
	osThreadDef(GIZWITSTASK, gizwits_data_process_task, osPriorityBelowNormal, 0, 512+256);
	osThreadCreate(osThread(GIZWITSTASK), NULL);
	
	/* 串口2接收数据处理任务,TVOC数据解析	*/
	osThreadDef(TVOCTask, tvoc_task, osPriorityLow, 0, 256);
	osThreadCreate(osThread(TVOCTask), NULL);
	
	/* 串口3接收数据处理任务, GPS数据解析 */
	osThreadDef(GPSRXTask, gps_receive_task, osPriorityLow, 0, 512+128);
	osThreadCreate(osThread(GPSRXTask), NULL);
	
	/* 与通信模组通信任务 */
	osThreadDef(UART1TXTask, usart1_send_task, osPriorityBelowNormal, 0, 256);
	osThreadCreate(osThread(UART1TXTask), NULL);
	
	/* 串口2发送数据处理任务, 更新LCD显示 */
	osThreadDef(UPDATELCD, update_lcd, osPriorityNormal, 0, 256);
	osThreadCreate(osThread(UPDATELCD), NULL);
	
	/* DHT11数据处理任务 */
	osThreadDef(DHT11Task, dht11_process_task, osPriorityRealtime, 0, 256);
	osThreadCreate(osThread(DHT11Task), NULL);
	
	/* 按键队列处理任务 */
	osThreadDef(ButtonTask, button_event_task, osPriorityHigh, 0, 128+64);
	osThreadCreate(osThread(ButtonTask), NULL);	
	
}

/* 温湿度数据处理 */
static void dht11_process_task(void const* arg)
{
	unsigned char count = 0;
	osDelay(1500);  /* 延时1.5S等待模块稳定 */
	HAL_TIM_Base_Start_IT(&htim2); /* 开启定时处理任务，10us进入定时器中断*/
	
	while(1)
	{
		if( !dht11.reading )  
		{
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
		osDelay(3000);  /* 每2秒读取一次温湿度数据 */
	}	
}
/* function code end */


/* 看门狗喂狗任务 */
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
	unsigned int step = 0;
	
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
				case TVOC_PPM_00:  /* 等级0  <1.5ppm */
					RGB1_Green();
					RGB2_Green();
					break;
				case TVOC_PPM_01: /* 等级1  1.5~5.5ppm */
					 RGB1_Blue();
					 RGB2_Blue();
					break;
				case TVOC_PPM_02:  /* 等级2  5.5~10ppm */
					RGB1_Purple();
					RGB2_Purple();				
					break;
				case TVOC_PPM_03:  /* 等级3  >10ppm */
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

/* 蜂鸣器鸣叫任务 */
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

/* 串口1接收处理任务 */
static void usart1_receive_task(void const* arg)
{
	unsigned int data_len = 0;
	char buff[128];
	unsigned int i = 0;
	unsigned char count = 0;
	
	while(1)
	{
		osDelay(20);
		restart_usart(&huart1);
		data_len = readBuffLen(USART1_ID); /* 读取串口1缓冲队列中的数据长度 */
		if(data_len>0)
		{
			if(data_len>128) data_len = 128;			
			read(USART1_ID, &buff[0], data_len);
			
			/* 解析数据 */
			for(i=0; i<data_len; ++i)
			{
				count = 0;
				p_air_sensor->update(buff[i]);
				if(p_air_sensor->health && !p_air_sensor->error)
				{
					/* 换算等级 */
					sys_flag.tvoc_level = p_air_sensor->convert_level((unsigned int)(p_air_sensor->air_ppm*10));
					sys_flag.tvoc_ppm = (unsigned char)p_air_sensor->air_ppm;
					sys_flag.oled_update_area |= OLED_UPDATE_AREA_AIR_YES;
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

/* 串口1发送任务 */
static void usart1_send_task(void const* arg)
{
	LoopQueue* sendQueue;
	short int data_len = 0;
	short int i = 0; 
	char send_buff[200];	
	TickType_t old_time = 0;
	old_time = xTaskGetTickCount();
	
	while(1)
	{
		osDelay(10);	
		
		while(huart1.gState==HAL_UART_STATE_BUSY_TX && xTaskGetTickCount()-old_time<=3);
		
		sendQueue = getUsartSendLoopQueue(USART1_ID); /* get send queue */
		if(sendQueue!=NULL)
		{	
			data_len = writeBuffLen(USART1_ID); /* send queue data count */
			
			if(data_len>0)
			{
				if(data_len>=200) data_len = 200;
				
				for( i=0; i<data_len; ++i)
				{
					send_buff[i] = read_element_loop_queue(sendQueue);
				}				
				
				HAL_UART_Transmit_DMA(&huart1, (uint8_t *)send_buff, (uint16_t)data_len); /* DMA send	*/
			}
		}	
	}
}
/* function code end */

/* 串口2发送任务 */
static void update_lcd(void const* arg)
{
	unsigned char fcolor = 10;
	unsigned char bcolor = 60;
	unsigned int step = 0;

	osDelay(2000);
	HAL_UART_Transmit_DMA(&huart2, "CLR(60);\r\n", strlen("CLR(18);\r\n"));
	osDelay(500);	
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)get_lcd_str(), strlen(get_lcd_str()));
	osDelay(1000);
	HAL_UART_Transmit_DMA(&huart2, "CLR(60);\r\n", strlen("CLR(18);\r\n"));
	osDelay(500);	
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)get_lcd_str(), strlen(get_lcd_str()));
	
	while(1)
	{
		osDelay(500);		
		++step;
		
		/* 温湿度更新显示 */
		if(dht11.exist)
		{			
			if(dht11.valid)
			{
				dht11.reading = 1;
				
				osDelay(200);	
				usart_lcd_display_temp(dht11.TEMP, &lcd, bcolor, 14);
				HAL_UART_Transmit_DMA(&huart2, (uint8_t *)lcd.display_buff, strlen(lcd.display_buff));

				osDelay(200);
				usart_lcd_display_RH(dht11.RH, &lcd, bcolor, 14);
				HAL_UART_Transmit_DMA(&huart2, (uint8_t *)lcd.display_buff, strlen(lcd.display_buff));
				
				dht11.reading = 0;
				
			}
		}
		
		/* 更新TVOC传感器数据显示 */
		if(p_air_sensor->init)
		{
			osDelay(200);
			usart_lcd_display_TVOC(p_air_sensor->air_ppm, &lcd, bcolor, 14);
			HAL_UART_Transmit_DMA(&huart2, (uint8_t *)lcd.display_buff, strlen(lcd.display_buff));
			
			osDelay(200);
			usart_lcd_display_airq(sys_flag.tvoc_level, &lcd, bcolor, 14);
			HAL_UART_Transmit_DMA(&huart2, (uint8_t *)lcd.display_buff, strlen(lcd.display_buff));
		}
		
		osDelay(200);	
		usart_lcd_display_GPS(nmea_data.gpsData.num_sats, &lcd, bcolor, 3);
		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)lcd.display_buff, strlen(lcd.display_buff));
		
		osDelay(300);	
		if(is_good(&nmea_data))
		{
			switch( ((int)(nmea_data.gpsData.ground_speed)) / 10 )
			{
				case 0:
				case 1:
				case 2:
				case 3:
					fcolor = LIME_COLOR;
					break;
				case 4:
				case 5:
					fcolor = CYAN_COLOR;
					break;
				case 6:
				case 7:
					fcolor = ROYAL_BLUE_COLOR;
					break;
				case 8:
				case 9:
					fcolor = ORANGE_COLOR;
					break;
				default:
					fcolor = RED_COLOR;
					break;
			}
			usart_lcd_display_speed(nmea_data.gpsData.ground_speed, &lcd,bcolor, fcolor);
		}
		else
			usart_lcd_display_speed(0.0, &lcd,bcolor, fcolor);
		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)lcd.display_buff, strlen(lcd.display_buff));
		
		osDelay(300);
		utc_time_display(&lcd, &nmea_data.gpsData.date_time, bcolor, 3);
		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)lcd.display_buff, strlen(lcd.display_buff));
	}
}
/* function code end */

/* 串口2接收处理任务 -- TVOC数据处理任务 */
static void tvoc_task(void const* arg)
{
	unsigned int data_len = 0;
	char buff[100];
	unsigned int i = 0;
	unsigned char count = 0;
	
	while(1)
	{
		osDelay(50);
		
		restart_usart(&huart2);
		
		data_len = readBuffLen(USART2_ID); /* 读取串口3缓冲队列中的数据长度 */
		if(data_len>0)
		{
			if(data_len>=100) data_len = 100;		

			/* 将数据读取到缓冲区中 */
			for(i=0; i<data_len; ++i)
			{
				buff[i] = read_char(USART2_ID);   
				#if AWESOME_DEBUG_TVOC_ENABLE
					write_char(USART1_ID, buff[i]);
				#endif	
			}	
			
			/* 解析数据 */
			for(i=0; i<data_len; ++i)
			{
				count = 0;
				p_air_sensor->update(buff[i]);
				if(p_air_sensor->health && !p_air_sensor->error)
				{
					/* 换算等级 */
					sys_flag.tvoc_level = p_air_sensor->convert_level((unsigned int)(p_air_sensor->air_ppm*10));
					sys_flag.tvoc_ppm = (unsigned char)p_air_sensor->air_ppm;
					sys_flag.oled_update_area |= OLED_UPDATE_AREA_AIR_YES;
				}
			}
		}else if(0==data_len)
		{
			count++;
			if(count>=250)
			{
				count = 0;
				sys_flag.sensor_healthy &= ~SENSOR_TVOC_HEALTHY;
			}
		}
	}
}
/* function code end */

/* 串口3接收处理任务 -- GPS原始数据解析 */
static void gps_receive_task(void const* arg)
{
	unsigned int data_len = 0;
	char buff[200];
	unsigned int i = 0;
	unsigned char count = 0;
	unsigned int gpscount = 0;
	
//	TickType_t old_time = 0;
//	old_time = xTaskGetTickCount();   /* 系统时钟计数 -- freertos api */
	
	air530_config(&huart3);	 /* 配置GPS模块 */
	
	while(1)
	{
		osDelay(10);   /* 10ms调用一次 */
		
		restart_usart(&huart3);
		
		data_len = readBuffLen(USART3_ID); /* 读取串口3缓冲队列中的数据长度 */
		if(data_len>0)
		{
			if(data_len>=200) data_len = 200;		

			/* 将数据读取到缓冲区中 */
			for(i=0; i<data_len; ++i)
			{
				buff[i] = read_char(USART3_ID);   
				#if AWESOME_DEBUG_GPS_ENABLE
					write_char(USART1_ID, buff[i]);
				#endif	
			}			
			
			/* 解析数据 */
			for(i=0; i<data_len; ++i)
			{
				if(nmea_decode(&nmea_data, buff[i]))//按字符解析，如果得到一帧数据，则为真
				{
					count = 0;
					break;
				}
			}
		}else if(0==data_len)  /* 如果没数据 */
		{
			count++;
		}
		
		if(count>=250)   /* 2500ms==2.5S内串口没数据 */
		{
			count = 0;
			init_nmea0183(&nmea_data);
			air530_config(&huart3);   /* 重新配置GPS模块 */
		}
	}
}
/* function code end */

/* 串口3发送任务 */
static void usart3_send_task(void const* arg)
{
	uint16_t data_len = 0;
	LoopQueue* sendQueue;
	char send_buff[128];
	
	while(1)
	{
		osDelay(50);	
		
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
					send_buff[i] = read_loop_queue(sendQueue);
				}
				HAL_UART_Transmit_DMA(&huart1, (uint8_t *)send_buff, (uint16_t)data_len); /* DMA send	*/
			}
		}		
		xSemaphoreGive(mutex_usart1_tx);
	}
}
/* function code end */

/* 更新OLED屏幕显示 */
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
		
		/*清楚PPM显示区域*/
		OLED_Clear_Area(0, 0, 48, 0);
		OLED_Clear_Area(0, 1, 48, 1);
				
		if(p_air_sensor->init)
		{
			float_to_string(p_air_sensor->air_ppm, (char*)buff, 4, 0);
			if(strlen(buff)==1)
			{
				display_string_Font16_16(16, 0, "0");
			}
			if((sys_flag.oled_update_area & OLED_UPDATE_AREA_AIR_YES)>0)  /* 判断是否需要更新空气质量显示区域 */
			{
				switch(sys_flag.tvoc_level)
				{
					case TVOC_PPM_00:  /* 等级0  <1.5ppm */
						pchinese = &chinese16_16[2][0];
						break;
					case TVOC_PPM_01: /* 等级1  1.5~5.5ppm */
						pchinese = &chinese16_16[4][0];
						break;
					case TVOC_PPM_02:  /* 等级2  5.5~10ppm */
						pchinese = &chinese16_16[6][0];		
						break;
					case TVOC_PPM_03:  /* 等级3  >10ppm */
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
				/*清楚温度显示区域*/
				OLED_Clear_Area(0, 4, 64, 4);
				OLED_Clear_Area(0, 3, 64, 3);

				/*清楚温度显示区域*/
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

/* 每50ms调用一次任务： LED、BEEP和喂狗操作 */
static void task_50ms(void const* arg)
{
	while(1)
	{
		osDelay(50);
		
		run_led_fun();
		beep_fun();
		feed_dog_fun();
	}
}
/* end the function */

/* 初始化系统函数 */
static void init_system(void)
{
	initUsartBuff(USART2_ID);
	initUsartBuff(USART1_ID);
	initUsartBuff(USART3_ID);
	
	init_nmea0183(&nmea_data);
	
	init_system_button();
	mutex_usart1_tx = xSemaphoreCreateMutex();
	mutex_usart2_tx = xSemaphoreCreateMutex();
	button_event_queue = xQueueCreate( 10, sizeof( Button* ));
	
	p_air_sensor = get_air_sensor(); 
	dht11.GPIOx = GPIOA;
	dht11.GPIO_Pin = GPIO_PIN_5;
	DHT11_init(&dht11);
	
	initUsartIT(&huart1);
	initUsartIT(&huart2);
	initUsartIT(&huart3);
	
//	write(USART1_ID, "USART1 ENBALE\n", sizeof("USART1 ENBALE\n")/sizeof(char));
//	write(USART2_ID, "USART2 ENBALE\n", sizeof("USART2 ENBALE\n")/sizeof(char));
	
	sys_flag.open_rgb = 0;
	sys_flag.sensor_healthy = 0;
}
/* function code end */

/* 按键处理代码 */
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
			
			if(BUTTON_STATUS_DOWN_START==bt->status) /* 识别到第一次按下，有可能是噪声 */
			{
				osDelay(10);    /* 消除抖动 */
				if(GPIO_PIN_RESET==read_button_pin_satus(id)) /* 确定按下 */
				{
					bt->status = BUTTON_STATUS_DOWN_ENTER;
					bt->clicked = 1;
				}else
				{
					bt->status = BUTTON_STATUS_NONE;
					bt->down_time = 0;
				}
			}
			else if(BUTTON_STATUS_UP_START==bt->status) /* 按键回弹 */
			{
				osDelay(10);    /* 消除抖动 */
				if(GPIO_PIN_SET==read_button_pin_satus(id)) 
				{
					bt->status = BUTTON_STATUS_UP_ENTER;
					bt->release = 1;
					
					/* 处理按键回弹事件 */
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
					if(b->down_time*30>=3000) /* 按钮一直按下3秒 */
					{
						/* 处理事件 */
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
		
		/*更新传感器健康标志*/
	}
}
/* function code end */ 

static void restart_usart(UART_HandleTypeDef *huart)
{
	if(huart==0) return;
	
	if((huart->ErrorCode |= HAL_UART_ERROR_ORE)!=RESET)
	{
		huart->ErrorCode = HAL_UART_ERROR_NONE;
		huart->RxState = HAL_UART_STATE_BUSY_RX;
		
		//SET_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
		//SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

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
}

/* 初始化串口接收中断 */
static void initUsartIT(UART_HandleTypeDef *huart)
{
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
}

/* 更新传感器标志 */
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

/* 传感器错误提示 */
static void sensor_error_display(void)
{
	OLED_Clear_Area(96, 6, 127, 6);
	OLED_Clear_Area(96, 7, 127, 7);
	
	if( (sys_flag.sensor_healthy&SENSOR_DHT11_HEALTHY)&&(sys_flag.sensor_healthy&SENSOR_TVOC_HEALTHY) )
		return;
	
	if( !(sys_flag.sensor_healthy&SENSOR_DHT11_HEALTHY) )
	{
		/*清楚温度显示区域*/
		OLED_Clear_Area(0, 4, 64, 4);
		OLED_Clear_Area(0, 3, 64, 3);

		/*清楚温度显示区域*/
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



/* led 控制函数 50ms任务 */
static void run_led_fun(void)
{
	static unsigned int step = 0;
	
	++step;
	
	switch(step)
	{
		case 1: 
			set_run_led();
			break;
		case 2:
			reset_run_led();
			break;
		default: break;
	}
	
	if(21==step)
	{
		step = 0;
	}
}

static void feed_dog_fun(void)
{
	static unsigned int step = 0;
	
	++step;
	
	if(2==step)
	{
		step = 0;
		HAL_IWDG_Refresh(&hiwdg);
		update_sensor_status();
	}
	
}

static void beep_fun(void)
{
	static unsigned int step = 0;
	static unsigned char count = 0;
		
	if(!sys_flag.open_beep)
	{
		reset_beep();
		step = 0;
		return;
	}
	
	if(sys_flag.tvoc_level>=TVOC_PPM_03 && p_air_sensor->init)
	{
		if(count++>=20) /*  持续一秒*/
		{
			count = 20;
		}else
			return;
		
		++step;
		switch(step)
		{
			case 1:
				set_beep();
				break;
			case 11:
				reset_beep();
				break;
			default: break;
		}
		if(41==step)
		{
			step = 0;
		}
	}else 
	{
		count = 0;
		step = 0;
		reset_beep();
	}
}

/* gizwits MCU设备接收数据处理任务 */
static void gizwits_data_process_task(void const* arg)
{
	unsigned int data_len = 0;
	char buff[128] = {0};
	unsigned char gizwits_data[150] = {0};
	unsigned int i = 0;
	unsigned char count = 0;
	
	gizwits_pack* p_gizwits_pack_send;
	gizwits_pack gizwits_pack_send_buff;
	gizwits_status* p_gizwits_status;
	gizwits_pack gizwits_pack_rec_buff;
	gizwits_result gizwits_result_rec;
	gizwits_init();	
	p_gizwits_pack_send = get_gizwits();
	p_gizwits_status = (gizwits_status*)get_gizwits_status();
	
	while(1)
	{
		osDelay(20);
		restart_usart(&huart1);
		data_len = readBuffLen(USART1_ID); /* 读取串口1缓冲队列中的数据长度 */
		
		if(data_len>0)
		{
			if(data_len>128) data_len = 128;			
			read(USART1_ID, &buff[0], data_len);
			
			/* 解析数据 */
			for(i=0; i<data_len; ++i)
			{
				count = 0;
				if(GIZWITS_RESULT_OK==gizwits_parse_char(buff[i], &gizwits_pack_rec_buff, &gizwits_result_rec))
				{
					if(gizwits_data_process(&gizwits_pack_rec_buff, p_gizwits_pack_send))
					{
						/* 发送数据 */
						xSemaphoreTake( mutex_usart1_tx, portMAX_DELAY );	
						write(USART1_ID, (char*)p_gizwits_pack_send, p_gizwits_pack_send->length);
						xSemaphoreGive(mutex_usart1_tx);
						
						/* 模组请求发送数据 */
						if(p_gizwits_status->atr_flag&&p_gizwits_status->atr_value)
						{
							/* 处理数据,并准备发送 */
							
						}
					}
				}
			}
		}else if(0==data_len)
		{
			count++;
			if(count>=250)
			{
				count = 250;
			}
		}
	}
}
	


/* function code end */
