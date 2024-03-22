/**
  *@file timers_ese.h
  *@author Mitchell Brough
  *@brief Library concerned with the use of STM32F103RB hardware timers
  *@version 1.0
  *@date 2024-03-05
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
  *
 */

#ifndef TIMERS_ESE_H
#define TIMERS_ESE_H

#include "stm32f10x.h"

void configure_tim1(void);
void configure_tim2(void);
void configure_tim3(void);
void configure_tim4(void);
void start_encoder_readings(void);

#endif
