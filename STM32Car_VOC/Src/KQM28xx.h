
/* 空气质量检测模块(传感器型号KQ-2801)
*  本代码适用于深圳市慧联智控检测模块--KQM2800TA、KQM2801A
*  检测气体：有机化和挥发气体>>氨气、氢气、酒精、一氧化碳、甲烷、丙烷、
*					   甘烷、甲苯、苯化烯、丙二醇、酚、甲苯、乙苯、二甲苯、甲醛等。
*  生活烟味：香烟、木材、纸张燃烧烟雾、烹饪油烟等
*	 生活异味：生活垃圾，腐烂臭气等
*  检测浓度范围： 0.1-30PPM
*  预热时间：3分钟，期间串口输出0xffff
*  工作温度：-20摄氏度 至 50摄氏度
*  灵敏度：0.1ppm酒精
*  输入电压：4.9V-5.1V
*  工作电流：75mA-85mA
*  适用寿命：大于5年
*  串口数据协议：起始位1位、8bit、无校验位、停止位1位，9600bps
*	 数据格式：
*							byte1		|		byte2		|		byte3		|		byte4
*							 0x5F 	|	 输出高位	|  输出低位 |	 校验值
*	 其中校验位为：byte1 + byte2 + byte3三位数据和的低八位
*								 浓度(ppm) = （输出高位*256 + 输出低位）*0.1ppm
*  模块上电默认为主动传送模式，每秒主动输出一次数据。在预热期间，有效
*	 数据位为0xff，不予处理。
*  KQM2801A可进行可选配置输出:
*			返回一帧数据配置指令： 0XF5 0XF0 0X30 0X15
*     每秒发送数据配置指令： 0X5F 0XF0 0X31 0X80
*		
*  Write By Awesome OuYangLei
*/

#ifndef KQM28XX_H
#define KQM28XX_H

#define FRAME_START 0x5F
#define KQM28XX_UPDATE_MS 20  /* update函数被调用的周期 20ms */
#define KQM28XX_MAX_ERROR_TIME 5000 /* 5000ms内无数据时间 */

typedef enum TVOC_PPM_STATUS
{
	TVOC_PPM_00, /* 等级0  <1.5ppm */
	TVOC_PPM_01, /* 等级1  1.5~5.5ppm */
	TVOC_PPM_02, /* 等级2  5.5~10ppm */
	TVOC_PPM_03, /* 等级3  >10ppm */
	TVOC_PPM_DANGER,
}TVOC_PPM_STATUS;

/* 数据解析状态标志 */
typedef enum air_sensor_flag
{
	AIR_SENSOR_NONE,
	AIR_SENSOR_START,
	AIR_SENSOR_DATA1,
	AIR_SENSOR_DATA2,
	AIR_SENSOR_CHECK,
}AirSensorFlag;

typedef struct air_sensor
{
	void (*update)(char );
	TVOC_PPM_STATUS (*convert_level)(unsigned int);
	float air_ppm;
	unsigned int check_sum;
	AirSensorFlag parse_flag;
	unsigned char error_time; /* 一段时间段内没有数据，则认为传感器不健康，默认为5000ms内没有数据，则认为传感器不健康*/
	unsigned char health;    /* 5秒内无数据输出，则认为不健康 ，不健康为0，健康为1 */
	unsigned char init:1;    /* 传感器成功启动标志，数据有效 */  
	unsigned char error:1;   /* 解析错误标志，数据解析出错-0，正确为-1 */
}AirSensor;

void update(char data);
AirSensor* get_air_sensor(void);
TVOC_PPM_STATUS convert_level(unsigned int data);

#endif

