#ifndef CLOCKS_ESE_H
#define CLOCKS_ESE_H

#include "stm32f10x.h"

void sysclock_init(void);
void clock_gpioa(void);
void clock_gpiob(void);
void clock_gpioc(void);
void clock_i2c2(void);
void clock_tim2(void);
void clock_tim3(void);
void clock_tim4(void);
void clock_dma(void);
void clock_usart2(void);
void clock_afio(void);

#endif
