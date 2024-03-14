#ifndef ADC_ESE_H
#define ADC_ESE_H

#include "stm32f10x.h"

void configure_adc1(void);
uint32_t read_joystick_x(void);
uint32_t read_joystick_y(void);

#endif
