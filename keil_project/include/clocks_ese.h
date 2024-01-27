/**
  *@file clocks_enel452.h
  *@author Mitchell Brough 200239781
  *@brief Function declarations, see clocks.c
  *@date 2023-10-05
 */

#ifndef CLOCKS_ESE_H
#define CLOCKS_ESE_H

void sysclock24_init(void);
void clock_gpioa(void);
void clock_gpiob(void);
void clock_gpioc(void);
void clock_tim2(void);
void clock_tim3(void);
void clock_usart2(void);
void clock_afio(void);

#endif
