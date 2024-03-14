/**
  *@file utilities.c
  *@author Emily Schwab
  *@brief
  *
  *@version 1.0
  *@date 2024-01-19
  *
  *@copyright Copyright (c) 2024 Emily Schwab
 */

#ifndef UTILITIES_ESE_H
#define UTILITIES_ESE_H

#include "stm32f10x.h"

void configure_tim3(void);

void enableTimer3IRQ(void);

void TIM3_IRQHandler(void);

#endif
