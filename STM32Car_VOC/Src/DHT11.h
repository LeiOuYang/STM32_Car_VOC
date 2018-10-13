
/*  DHT11数字温湿度传感器
*   传感器包括电阻式感湿元件和一个NTC测温元件
*   湿度传感器参数：
*			分辨率：16bit
*  	  精度：25摄氏度  正负5%RH
*		温度传感器：
*			分辨率：16bit
* 		精度： 25摄氏度  正负2摄氏度
*   采样周期：
*     大于2秒每次
*  DC 3.3V~5.5V  测量0.3mA  待机 60uA
*  串行数据单总线：每次读取温湿度数据是上次测量的结果，如果需要获取
	 实时数据，需连续读取两次，但不建议连续读取传感器，每次读取传感器
	 间隔大于5秒即可获得准确的数据
	 一次传送40位数据，高位先输出：
		8bit湿度整数数据+8bit湿度小数数据+8bit温度整数数据+8bit温度小数数据+8bit校验位
		注意：温湿度小数部分为0
		校验位数据是前四八位数据和的末八位数据
*
*/

#ifndef DHT11_H
#define DHT11_H

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#define MAX_DATA_BIT_COUNT 40

typedef struct dht11
{
	GPIO_TypeDef* GPIOx;
	unsigned int GPIO_Pin;
	float TEMP;
	float RH;
	unsigned char valid;
	unsigned char data[5];
}DHT11;

void DHT11_io_in_config(DHT11* pdht);
void DHT11_io_out_config(DHT11* pdht);
unsigned char DHT11_start(DHT11* dht);
void DHT11_read_data(DHT11* dht);
void delay10us(unsigned int time);
extern void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif

