/**************************************** 
Capstone Utilities
ENEL 417

Name: Emily Schwab
SID: 200426400
Date Created: 2024-01-19
****************************************/

#include "stm32f10x.h"

void clkInit(int);
void ioInit(void);

void adcInit(void);
uint32_t adcRead(uint32_t);

void timInit(void);

void enableTimer3IRQ(void);

void TIM3_IRQHandler(void);

