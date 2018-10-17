
#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

	#include "stm32f1xx_hal.h"
	#include "ButtonDriver.h"
	#include "UsartDriver.h"
	#include "KQM28xx.h"
	#include "DHT11.h"
	#include "usart_lcd.h"
	
	/* led io conifg */
	
	#define run_led 0
	#define status_led 1
	
	#define ledAction(led, sta) if(led==run_led)HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, sta);else HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, sta);
	
	#define set_run_led() ledAction(run_led, GPIO_PIN_RESET)
	#define reset_run_led() ledAction(run_led, GPIO_PIN_SET)
	
	#define set_status_led() ledAction(status_led, GPIO_PIN_RESET)
	#define reset_status_led() ledAction(status_led, GPIO_PIN_SET)	
		
	/* end led define */
	
	/* RGB1 */
	
	#define RGB1_RLED(sw) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, sw)
	#define RGB1_GLED(sw) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, sw)
	#define RGB1_BLED(sw) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, sw)
	
	#define RGB1_Red() 		{ RGB1_RLED(1); RGB1_GLED(0); RGB1_BLED(0);}
	#define RGB1_Green() 	{ RGB1_RLED(0); RGB1_GLED(1); RGB1_BLED(0);}
	#define RGB1_Blue()		{ RGB1_RLED(0); RGB1_GLED(0); RGB1_BLED(1);}
	#define RGB1_Close()	{ RGB1_RLED(0); RGB1_GLED(0); RGB1_BLED(0);}
	#define RGB1_Yellow()	{ RGB1_RLED(1); RGB1_GLED(1); RGB1_BLED(0);}
	#define RGB1_Purple() { RGB1_RLED(1); RGB1_GLED(1); RGB1_BLED(1);}
	
	/* end RGB1 */
	
	/* RGB2 */
	#define RGB2_RLED(sw) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, sw)
	#define RGB2_GLED(sw) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, sw)
	#define RGB2_BLED(sw) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, sw)
	
	#define RGB2_Red() 		{ RGB2_RLED(1); RGB2_GLED(0); RGB2_BLED(0);}
	#define RGB2_Green() 	{ RGB2_RLED(0); RGB2_GLED(1); RGB2_BLED(0);}
	#define RGB2_Blue()		{ RGB2_RLED(0); RGB2_GLED(0); RGB2_BLED(1);}
	#define RGB2_Close() 	{ RGB2_RLED(0); RGB2_GLED(0); RGB2_BLED(0);}
	#define RGB2_Yellow()	{ RGB2_RLED(1); RGB2_GLED(1); RGB2_BLED(0);}
	#define RGB2_Purple() { RGB2_RLED(1); RGB2_GLED(1); RGB2_BLED(1);}

	/* end RGB2 */
	
	/* beep io config */
	
	#define set_beep() {HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);}
	#define reset_beep() {HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);}	
	
	/* end beep define */	
	
	/* button define and config */
	#define BUTTON1_CLASS GPIOA
	#define BUTTON2_CLASS	GPIOA
	#define BUTTON1_PIN 	GPIO_PIN_0
	#define BUTTON2_PIN 	GPIO_PIN_1
	
	#define BUTTON_MAX_NUM 4

	typedef enum button_id
	{
		BUTTON_ID_1, BUTTON_ID_2, BUTTON_ID_3, BUTTON_ID_4
	}ButtonId;
	
void init_system_button(void);
GPIO_PinState read_button_pin_satus(ButtonId id);
BUTTON_STATUS button_exti_callback(ButtonId id);
Button* get_button_by_id(ButtonId id);
	
	/* end the button define and config */
#endif

