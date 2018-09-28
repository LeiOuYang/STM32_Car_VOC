
#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#define MAX_BUTTON_ID 255

typedef enum update_status
{
	BUTTON_UPDATE_SUCCESS, BUTTON_UPDATE_FAIL
}UpdateStatus;

typedef enum status
{
	BUTTON_STATUS_DOWN_START,   /* 识别到有按下按键，但有可能是噪音 */
	BUTTON_STATUS_DOWN_ENTER,		/* 程序消除抖动，确认按下 */
	BUTTON_STATUS_UP_START,
	BUTTON_STATUS_UP_ENTER,
	BUTTON_STATUS_NONE,
} BUTTON_STATUS;

typedef enum button_clicked
{
	BUTTON_DOWN=0, BUTTON_UP, BUTTON_NONE
}ButtonClicked;

typedef struct _button
{
	unsigned char id;    						/* button id, a button to a id*/	
	unsigned char clicked: 1;  			/* 1-button down */
	unsigned char double_click: 1;  /* 1-double click */
	unsigned char release: 1;    		/* 1-button up */
	unsigned char : 5;
	unsigned char status;						/* enum  BUTTON_STATUS*/		
	unsigned int down_time;  				/* button down time ms*/	
	unsigned int ago_down_time; 		/* ago button down time */	
}Button;

void init_button(Button* bt, unsigned char id);
void update_button_down_time(Button* bt);
ButtonClicked button_is_release(Button* bt);
void reset_button(Button* bt);

unsigned int get_button_down_time(Button* bt);

#endif
