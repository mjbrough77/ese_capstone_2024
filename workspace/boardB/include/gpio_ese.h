/**
  *@file gpio_ese.h
  *@author Mitchell Brough
  *@brief Library concerning GPIO/AFIO pin configurations
  *@version 1.0
  *@date 2024-03-04
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
  *
 */

#ifndef GPIO_ESE_H
#define GPIO_ESE_H

/**
  *@brief Uses GPIO, AFIO registers to configure IO pins
  *
  *@pre The GPIOx lines used in this function have their clocks enabled
 */
void configure_io(void);

#endif
