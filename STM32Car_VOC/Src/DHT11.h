
/*  DHT11������ʪ�ȴ�����
*   ��������������ʽ��ʪԪ����һ��NTC����Ԫ��
*   ʪ�ȴ�����������
*			�ֱ��ʣ�16bit
*  	  ���ȣ�25���϶�  ����5%RH
*		�¶ȴ�������
*			�ֱ��ʣ�16bit
* 		���ȣ� 25���϶�  ����2���϶�
*   �������ڣ�
*     ����2��ÿ��
*  DC 3.3V~5.5V  ����0.3mA  ���� 60uA
*  �������ݵ����ߣ�ÿ�ζ�ȡ��ʪ���������ϴβ����Ľ���������Ҫ��ȡ
	 ʵʱ���ݣ���������ȡ���Σ���������������ȡ��������ÿ�ζ�ȡ������
	 �������5�뼴�ɻ��׼ȷ������
	 һ�δ���40λ���ݣ���λ�������
		8bitʪ����������+8bitʪ��С������+8bit�¶���������+8bit�¶�С������+8bitУ��λ
		ע�⣺��ʪ��С������Ϊ0
		У��λ������ǰ�İ�λ���ݺ͵�ĩ��λ����
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

