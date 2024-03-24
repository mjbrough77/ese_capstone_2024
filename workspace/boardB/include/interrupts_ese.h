/**
  *@file interrupts_ese.h
  *@author Mitchell Brough
  *@brief Library for interrupt handlers
  *@version 1.0
  *@date 2024-03-03
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
  *
 */

#ifndef INTERRUPTS_ESE_H
#define INTERRUPTS_ESE_H

void DMA1_Channel2_IRQHandler(void);
void DMA1_Channel3_IRQHandler(void);
void DMA1_Channel4_IRQHandler(void);
void DMA1_Channel5_IRQHandler(void);
void I2C2_EV_IRQHandler(void);
void I2C2_ER_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void TIM1_CC_IRQHandler(void);
void TIM4_IRQHandler(void);
void USART3_IRQHandler(void);
    
#endif
