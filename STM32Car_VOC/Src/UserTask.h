
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
	
	
	void app_run(void);   /* 所有任务初始化 */
	
	/* 任务函数 */
	void feed_dog_task(void const* arg);
	
	
	extern IWDG_HandleTypeDef hiwdg;

#endif
