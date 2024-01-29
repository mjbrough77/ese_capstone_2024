#include "stm32f10x.h"

#include "../include/interrupts_ese.h"

void TIM4_IRQHandler(void){
    static uint16_t read_Zphase = 0;
    
    read_Zphase = TIM4->CCR1;
    TIM4->CR1 |= TIM_CR1_UDIS;
    TIM4->EGR |= TIM_EGR_UG;
    TIM4->CR1 &= ~TIM_CR1_UDIS;
}