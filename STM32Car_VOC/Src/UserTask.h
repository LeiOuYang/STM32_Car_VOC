
#ifndef USER_TASK_H
#define USER_TASK_H
	#include "HardwareConfig.h"
	#include "stm32f1xx_hal.h"
	#include "cmsis_os.h"
	#include "main.h"
	#include "gizwits_protocol.h"
	#include "nmea0183.h"
	
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
	
	typedef enum RGB_CONTROL
	{
		RGB_CLOSE_ENABLE = 0X00,
		RGB_RED_ENABLE,
		RGB_GREEN_ENABLE,
		RGB_BLUE_ENABLE,
		RGB_OPEN_ENABLE,
		RGB_LOW_SPEED_ENABLE,
		RGB_MED_SPEED_ENABLE,
		RGB_QUCK_SPEED_ENABLE,
		RGB_DEFAULET
	}RGB_CONTROL;
	
	typedef enum RGB_COLOR
	{
		RGB_COLOR_COLSE = 0X00,
		RGB_COLOR_RED,
		RGB_COLOR_GREEN,
		RGB_COLOR_BLUE,
		RGB_COLOR_YELLOW,
		RGB_COLOR_PURPLE
	}RGB_COLOR;
	
	typedef enum BEEP_CONTROL
	{
		BEEP_CLOSE_ENABLE = 0X00,
		BEEP_LOW_ENBALE,
		BEEP_MED_SPEED_ENABLE,
		BEEP_QUCK_SPEED_ENABLE,
		BEEP_DEFAULET
	}BEEP_CONTROL;

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
	
	typedef struct _dev_control
	{
		unsigned char rgb_enable : 1;
		unsigned char beep_enable : 1;
		unsigned char rgb_color;	
		unsigned short beep_speed;
		unsigned short rgb_speed;
	}dev_control;
	
	static xSemaphoreHandle mutex_usart1_tx;
	static xSemaphoreHandle mutex_usart2_tx;
	static xSemaphoreHandle mutex_read_gps;
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
	static void update_lcd(void const* arg);
	static void tvoc_task(void const* arg);
	static void gps_receive_task(void const* arg);
	static void usart3_send_task(void const* arg);
	static void update_oled_task(void const* arg);
	static void dht11_process_task(void const* arg);
	
	static void update_sensor_status(void);
	static void sensor_error_display(void);
	static void button_event_task(void const* arg);
	
	static void task_50ms(void const* arg);
	
	static void restart_usart(UART_HandleTypeDef *huart);
	
	static void run_led_fun(void);
	static void feed_dog_fun(void);
	static void beep_fun(void);
	
	static void gizwits_data_process_task(void const* arg);
	static void gizwits_process_data_node(const gizwits_status* const pgs);
	static unsigned char gizwits_control_rgb(unsigned int base_time, unsigned char color, unsigned int blink_time);
	static void rgb_color(unsigned char color, unsigned char status);
	
	extern IWDG_HandleTypeDef hiwdg;
	extern UART_HandleTypeDef huart1;
	extern UART_HandleTypeDef huart2;
	extern UART_HandleTypeDef huart3;
	extern TIM_HandleTypeDef htim2;

#endif
