
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

typedef enum DHT11_status
{
	DHT11_NONE,
	DHT11_START,
	DHT11_DATA,
	DHT11_DATA_1,
	DHT11_DATA_END,
	
}DHT11_status;

typedef struct dht11
{
	unsigned int time_count;
	unsigned char data[5];  
	unsigned char data_count;
	unsigned char start_falg;
	unsigned char data_start_flag;
	unsigned char status;
	unsigned char valid;
}DHT11;

void DHT11_io_in_config(void);
void DHT11_io_out_config(void);
unsigned char DHT11_start(DHT11* dht);
void DHT11_data_process(DHT11* dht);
DHT11* get_dht11(void);
extern void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif

