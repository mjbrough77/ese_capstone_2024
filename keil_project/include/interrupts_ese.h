#ifndef INTERRUPTS_ESE_H
#define INTERRUPTS_ESE_H

#include "stm32f10x.h"

void DMA1_Channel4_IRQHandler(void);
void DMA1_Channel5_IRQHandler(void);
void I2C2_EV_IRQHandler(void);
void EXTI9_5_IRQHandler(void);


#endif
