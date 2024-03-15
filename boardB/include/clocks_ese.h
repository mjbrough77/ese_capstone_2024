/**
  *@file clocks_ese.h
  *@author Mitchell Brough
  *@brief Library concered with clock control on the F103RB
  *@version 0.1
  *@date 2024-03-04
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
  *
 */

#ifndef CLOCKS_ESE_H
#define CLOCKS_ESE_H

#include "stm32f10x.h"

void sysclock_init(void);
void clock_gpioa(void);
void clock_gpiob(void);
void clock_gpioc(void);
void clock_tim3(void);
void clock_adc1(void);
void clock_dma(void);
void clock_afio(void);
void clock_usart3(void);

#endif
