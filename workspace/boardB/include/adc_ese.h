#ifndef ADC_ESE_H
#define ADC_ESE_H

#include "stm32f10x.h"

void configure_adc1(void);

void start_joystick_read(void);

_Noreturn void get_weight_task(void*);

#endif
