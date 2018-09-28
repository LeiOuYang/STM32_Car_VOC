#include "UserTask.h"

static osThreadId feed_dog_task_handle;

void app_run(void)
{
	/* ι������ */
		osThreadDef(FeedDogThread, feed_dog_task, osPriorityIdle, 0, 128);
		feed_dog_task_handle = osThreadCreate(osThread(FeedDogThread), NULL);	
	
}


/* ���Ź�ι������ */
void feed_dog_task(void const* arg)
{
	while(1)
	{
		HAL_IWDG_Refresh(&hiwdg);
		osDelay(200);  /* 200ms feed the dog, avoid task wait to long time. if no feed dog, the system reset */
	}
}
/* function code end */
