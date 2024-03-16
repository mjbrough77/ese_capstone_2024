/**
  *@file drive.h
  *@author Emily Schwab
  *@brief
  *
  *@version 1.0
  *@date 2024-01-19
  *
  *@copyright Copyright (c) 2024 Emily Schwab
  *
 */

#ifndef DISPLAY_ESE_H
#define DISPLAY_ESE_H

#include "stm32f10x.h"

void write_ones_sevenseg(uint8_t);
void write_tens_sevenseg(uint8_t);
void send_to_display(uint8_t);
void turn_on_display(void);
void turn_off_display(void);

#endif
