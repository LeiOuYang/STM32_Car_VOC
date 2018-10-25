
#ifndef AIR530_H
#define AIR530_H

#include "stm32f1xx_hal.h"

unsigned char air530_crc(char* dest, const char* src);
void air530_config(UART_HandleTypeDef* huart);

#endif
