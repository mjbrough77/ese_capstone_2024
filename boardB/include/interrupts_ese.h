#ifndef INTERRUPTS_ESE_H
#define INTERRUPTS_ESE_H

#include "stm32f10x.h"

void enable_tim3IRQ(void);

void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void DMA1_Channel3_IRQHandler(void);
void USART3_IRQHandler(void);

#endif
