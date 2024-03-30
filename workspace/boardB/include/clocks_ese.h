/**
  *@file clocks_ese.h
  *@author Mitchell Brough
  *@brief Library of prototypes concerned with clock control
  *@version 1.0
  *@date 2024-03-04
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
  *
 */

#ifndef CLOCKS_ESE_H
#define CLOCKS_ESE_H

/**************************************************************************
 * System clock initialization
**************************************************************************/
/**
  *@brief Configures SYSCLK to 40MHz, AHB clock to 20MHz
  *
  * This function should always be run first on device reset
 */
void sysclock_init(void);

/**************************************************************************
 * Enable peripheral clocks
**************************************************************************/
/**
  *@brief All the functions below enable the clock on the realted peripheral
  *
  * These functions should be run immediately after `sysclock_init()`
 */
void clock_afio(void);
void clock_gpioa(void);
void clock_gpiob(void);
void clock_gpioc(void);
void clock_adc1(void);
void clock_dma1(void);
void clock_i2c2(void);
void clock_tim1(void);
void clock_tim2(void);
void clock_tim3(void);
void clock_tim4(void);
void clock_usart3(void);

#endif
