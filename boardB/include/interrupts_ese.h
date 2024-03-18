#ifndef INTERRUPTS_ESE_H
#define INTERRUPTS_ESE_H

#include "stm32f10x.h"

void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void DMA1_Channel3_IRQHandler(void);
void USART3_IRQHandler(void);

#endif
