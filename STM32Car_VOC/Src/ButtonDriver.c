
#include "ButtonDriver.h"

void init_button(Button* bt, unsigned char id)
{
	if(0==bt) return;
	
	bt->ago_down_time = 0;
	bt->clicked = 0;
	bt->release = 0;
	bt->double_click = 0;
	bt->down_time = 0;
	bt->id = id;
	bt->status = BUTTON_STATUS_NONE;
	
	return;
}

//BUTTON_STATUS update_button_event(Button* bt, BUTTON_STATUS event)
//{
//	if(0==bt) return BUTTON_STATUS_NONE;
//	
//	if()
//}

ButtonClicked button_is_release(Button* bt)
{
	if(0==bt) return BUTTON_NONE;
	
	if(bt->release==1)
	{
		reset_button(bt);
		return BUTTON_UP;
	}else return BUTTON_NONE;
}

ButtonClicked button_is_clicked(Button* bt)
{
	if(0==bt) return BUTTON_NONE;
	
	if(bt->clicked==1 || bt->release==1)
	{
		if(bt->release==1) reset_button(bt);
		return BUTTON_DOWN;
	}else return BUTTON_NONE;
}

void reset_button(Button* bt)
{
	if(0==bt) return;
	
	bt->ago_down_time = 0;
	bt->clicked = 0;
	bt->release = 0;
	bt->double_click = 0;
	bt->down_time = 0;
	bt->status = BUTTON_STATUS_NONE;
	
	return;
}

void update_button_down_time(Button* bt)
{

}

unsigned int get_button_down_time(Button* bt)
{
	return 0;
}
