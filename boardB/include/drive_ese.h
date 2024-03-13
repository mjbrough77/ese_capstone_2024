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

#ifndef DRIVE_ESE_H
#define DRIVE_ESE_H

#include "stm32f10x.h"

void display(uint8_t);	// Display any integer from 0 to 99 on the two digit 7-segment displays
void bcdOne(uint8_t);	// Display any integer from 0 to 9 on the 'ones' 7-segment display
void bcdTen(uint8_t);	// Display any integer from 0 to 9 on the 'tens' 7-segment display

uint8_t manual(void);	// Returns true if manual vs auto (i.e. safety) switch is set to manual, false otherwise

void stop(void);

#endif
