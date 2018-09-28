
#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H
	
	#include "stm32f1xx_hal.h"
	
	#define CHANNEL_COUNT 	2
	#define CHANNEL_TEMPSENSOR 0
	#define CHANNEL_VREFINT 1

	#define BUFF_COUNT CHANNEL_COUNT*10
	
	uint32_t* get_adc1_dma_buff(void);
	uint32_t get_adc1_buff_count(void);
	uint8_t get_adc1_channel_count(void);
	uint32_t average_filter(uint32_t* buff, uint16_t len);
	float get_adc1_temp(void);
	float get_adc1_refint_vol(void);
//	unsigned int float_to_string(double data, unsigned int ic, unsigned int dc, char* buff, unsigned int len);

#endif
