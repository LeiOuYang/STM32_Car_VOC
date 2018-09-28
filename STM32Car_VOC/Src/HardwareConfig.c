
#include "HardwareConfig.h"

static Button button_array[2];

void init_system_button(void)
{
	init_button( &button_array[0], BUTTON_ID_1 );
	init_button( &button_array[1], BUTTON_ID_2 );
}

GPIO_PinState read_button_pin_satus(ButtonId id)
{
	switch(id)
	{
		case BUTTON_ID_1:
		{
			return HAL_GPIO_ReadPin(BUTTON1_CLASS, BUTTON1_PIN);
			//break;
		}
		case BUTTON_ID_2:
		{
			return HAL_GPIO_ReadPin(BUTTON2_CLASS, BUTTON2_PIN);
			//break;
		}
		case BUTTON_ID_3:
		{
			break;
		}
		case BUTTON_ID_4:
		{
			break;
		}			
		default: break;
	}
	
	return GPIO_PIN_RESET;
}

BUTTON_STATUS button_exti_callback(ButtonId id)
{
	GPIO_PinState st = GPIO_PIN_RESET;
	Button *bt = 0;

	st = read_button_pin_satus(id);
	bt = get_button_by_id(id);
	if(GPIO_PIN_RESET==st)   /* 按下 */
	{
		if(bt->status==BUTTON_STATUS_NONE) /* 按键第一次按下 */
		{
			bt->status = BUTTON_STATUS_DOWN_START;
			bt->down_time = 0;
		}
	}
	else      /* 弹回 */
	{
		if(bt->status!=BUTTON_STATUS_UP_START) /* 按键第一次按下 */
		{
			bt->status = BUTTON_STATUS_UP_START;
			//bt->down_time = 0;
		}
	}
	
	return bt->status;
}

Button* get_button_by_id(ButtonId id)
{
	if(id>BUTTON_MAX_NUM) return (Button*)0;
	
	return &button_array[id];
}
