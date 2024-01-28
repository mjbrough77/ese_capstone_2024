#include "stm32f10x.h"

#include "../include/timers_ese.h"

// Confiures tim3 as encoder input
void configure_tim3(void){
    TIM3->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0; // TI1, TI2 mapped
    TIM3->SMCR |= TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0; // Capture on both A,B phase
    TIM3->ARR = 10240; // Ten encoder turns before reseting
    TIM3->CR1 |= TIM_CR1_CEN; // Counter enable
}
