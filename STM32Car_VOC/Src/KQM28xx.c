
#include "KQM28xx.h"

/* 根据输入的数据，解析数据 */
void update(char data)
{
	AirSensor* pas = 0;
	pas = get_air_sensor();  /* 获取传感器对象 */
	
	/* 收到起始标志，并且还没有开始解析 */
	if( FRAME_START==data && AIR_SENSOR_NONE==pas->parse_flag )
	{
		pas->parse_flag = AIR_SENSOR_START;
		pas->check_sum += data;
		pas->error = 0;
		pas->air_ppm = 0.0;
		
		return;
	}else 
		pas->health = 0;
	
	switch(pas->parse_flag)
	{
		case AIR_SENSOR_START:			
			pas->air_ppm = (float)(((unsigned short int)data)<<8);  /* 高位数据 */
			pas->parse_flag = AIR_SENSOR_DATA1;
			pas->check_sum += data;
			break;
		case AIR_SENSOR_DATA1:
			pas->air_ppm = pas->air_ppm + data;
			pas->parse_flag = AIR_SENSOR_DATA2;
			pas->check_sum += data;
			break;
		case AIR_SENSOR_DATA2:
			if((unsigned char)pas->check_sum == (unsigned char)data)
			{
				if(!pas->init && (unsigned short int)pas->air_ppm!=0xffff)
				{
					pas->init = 1;
				}
				pas->air_ppm = pas->air_ppm * 0.1;  /* 转化成ppm数据 */
				pas->health = 1;
				pas->error = 0;
			}else
			{
				pas->error = 1;
				pas->air_ppm = 0.0;
				pas->health = 0;
			}
			pas->parse_flag = AIR_SENSOR_NONE;
			pas->check_sum = 0;
			break;
	}
}

TVOC_PPM_STATUS convert_level(unsigned int data)
{
	if(data==0xffff) return 0;
	if(data>=0 && data<=15)
		return TVOC_PPM_00;
	else if(data>15 && data<=55)
		return TVOC_PPM_01;
	else if(data>55 && data<=100)
		return TVOC_PPM_02;
	else if(data>100 && data<=150)
		return TVOC_PPM_03;
	else
		return TVOC_PPM_DANGER;		
}

AirSensor* get_air_sensor(void)
{
	static AirSensor air_sensor = { update, convert_level, 0.0, 0, AIR_SENSOR_NONE,0, 0 };
	
	return &air_sensor;
}
