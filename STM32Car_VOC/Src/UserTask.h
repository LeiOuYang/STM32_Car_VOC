
#ifndef USER_TASK_H
#define USER_TASK_H
	#include "HardwareConfig.h"
	#include "stm32f1xx_hal.h"
	#include "cmsis_os.h"
	#include "main.h"
	
	#define RGB_TEST_MASK_BIT_VALID 		(1<<1)
	#define RGB_TEST_MASK_BIT_INVALID		(~(1<<1))
	#define OLED_TEST_MASK_BIT_VALID  	(1<<2)
	#define OLED_TEST_MASK_BIT_INVALID	(~(1<<2))
	#define BEEP_TEST_MASK_BIT_VALID  	(1<<3)
	#define BEEP_TEST_MASK_BIT_INVALID  (~(1<<3))
	
	#define OLED_PAGE1_UPDATE_VALID  		(1<<1)
	#define OLED_PAGE1_UPDATE_INVALID		(~(1<<1))	
	
	#define OLED_UPDATE_AREA_AIR_YES (1<<0)
	#define OLED_UPDATE_AREA_AIR_NO  (~(1<<0))
	
	#define OLED_AUTHOR_UPDATE_YES (1<<1)
	#define OLED_AUTHOR_UPDATE_NO (~(1<<1))
	
	typedef enum SENSOR_ERROR_STATUS
	{
		/* bit 1表示错误或者不存在    0-存在  */
		SENSOR_TVOC_HEALTHY = 0x01,   /* TVOC传感器正常 */
		SENSOR_DHT11_HEALTHY = 0x02,  /* DHT11温度湿度传感器正常 */
	
	}SENSOR_ERROR_STATUS;
		
	typedef struct system_flag
	{
		unsigned char tvoc_ppm;
		unsigned char temprtr;
		unsigned char humidity;
		TVOC_PPM_STATUS tvoc_level;
		unsigned char oled_update_area;
		unsigned char open_rgb:1;
		unsigned char open_beep:1;
		unsigned char button_click:1;
		unsigned char rgb_sw: 1;/* 0-默认状态显示   1-可按键切换*/
	  unsigned char rgb_list; /* 0-red, 1-green, 2-blue, 3-yellow, 4-Purple, 5-close */
		
		unsigned char sensor_healthy;  /* 传感器错误标志 枚举SENSOR_ERROR_STATUS */
	}system_flag;
	
	static xSemaphoreHandle mutex_usart1_tx;
	static xSemaphoreHandle mutex_usart2_tx;
	extern xQueueHandle button_event_queue;
	
	static AirSensor* p_air_sensor;	
	static DHT11* p_dht11;
	
	void app_run(void);   /* 所有任务初始化 */
	
	static void init_system(void);
	
	/* 任务函数 */
	static void feed_dog_task(void const* arg);
	static void led_blink_task(void const* arg);
	static void rgb_blink_task(void const* arg);
	static void beep_task(void const* arg);
	static void initUsartIT(UART_HandleTypeDef *huart);
	static void usart1_send_task(void const* arg);
	static void usart1_receive_task(void const* arg);
	static void usart2_send_task(void const* arg);
	static void usart2_receive_task(void const* arg);
	static void update_oled_task(void const* arg);
	static void dht11_process_task(void* arg);
	
	static void update_sensor_status(void);
	static void sensor_error_display(void);
	static void button_event_task(void const* arg);
	
	
	extern IWDG_HandleTypeDef hiwdg;
	extern UART_HandleTypeDef huart1;
	extern UART_HandleTypeDef huart2;
	extern TIM_HandleTypeDef htim2;

#endif
