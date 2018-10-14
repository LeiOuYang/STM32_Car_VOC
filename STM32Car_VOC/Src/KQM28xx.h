
/* �����������ģ��(�������ͺ�KQ-2801)
*  �����������������л����ǿؼ��ģ��--KQM2800TA��KQM2801A
*  ������壺�л����ͻӷ�����>>�������������ƾ���һ����̼�����顢���顢
*					   ���顢�ױ�������ϩ�����������ӡ��ױ����ұ������ױ�����ȩ�ȡ�
*  ������ζ�����̡�ľ�ġ�ֽ��ȼ������������̵�
*	 ������ζ���������������ó�����
*  ���Ũ�ȷ�Χ�� 0.1-30PPM
*  Ԥ��ʱ�䣺3���ӣ��ڼ䴮�����0xffff
*  �����¶ȣ�-20���϶� �� 50���϶�
*  �����ȣ�0.1ppm�ƾ�
*  �����ѹ��4.9V-5.1V
*  ����������75mA-85mA
*  ��������������5��
*  ��������Э�飺��ʼλ1λ��8bit����У��λ��ֹͣλ1λ��9600bps
*	 ���ݸ�ʽ��
*							byte1		|		byte2		|		byte3		|		byte4
*							 0x5F 	|	 �����λ	|  �����λ |	 У��ֵ
*	 ����У��λΪ��byte1 + byte2 + byte3��λ���ݺ͵ĵͰ�λ
*								 Ũ��(ppm) = �������λ*256 + �����λ��*0.1ppm
*  ģ���ϵ�Ĭ��Ϊ��������ģʽ��ÿ���������һ�����ݡ���Ԥ���ڼ䣬��Ч
*	 ����λΪ0xff�����账��
*  KQM2801A�ɽ��п�ѡ�������:
*			����һ֡��������ָ� 0XF5 0XF0 0X30 0X15
*     ÿ�뷢����������ָ� 0X5F 0XF0 0X31 0X80
*		
*  Write By Awesome OuYangLei
*/

#ifndef KQM28XX_H
#define KQM28XX_H

#define FRAME_START 0x5F
#define KQM28XX_UPDATE_MS 20  /* update���������õ����� 20ms */
#define KQM28XX_MAX_ERROR_TIME 5000 /* 5000ms��������ʱ�� */

typedef enum TVOC_PPM_STATUS
{
	TVOC_PPM_00, /* �ȼ�0  <1.5ppm */
	TVOC_PPM_01, /* �ȼ�1  1.5~5.5ppm */
	TVOC_PPM_02, /* �ȼ�2  5.5~10ppm */
	TVOC_PPM_03, /* �ȼ�3  >10ppm */
	TVOC_PPM_DANGER,
}TVOC_PPM_STATUS;

/* ���ݽ���״̬��־ */
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
	unsigned char error_time; /* һ��ʱ�����û�����ݣ�����Ϊ��������������Ĭ��Ϊ5000ms��û�����ݣ�����Ϊ������������*/
	unsigned char health;    /* 5�������������������Ϊ������ ��������Ϊ0������Ϊ1 */
	unsigned char init:1;    /* �������ɹ�������־��������Ч */  
	unsigned char error:1;   /* ���������־�����ݽ�������-0����ȷΪ-1 */
}AirSensor;

void update(char data);
AirSensor* get_air_sensor(void);
TVOC_PPM_STATUS convert_level(unsigned int data);

#endif

