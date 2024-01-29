/**
  *@file timers_ese.c
  *@author Mitchell Brough 200239781
  *@brief Functions related to the configuration of hardware timers
  *@date 2023-10-05
 */

#include "stm32f10x.h"

#include "../include/timers_ese.h"

/**
  *@brief Configures TIM3 in encoder interface mode
 */
void configure_tim3(void){
    TIM3->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0; // TI1, TI2 mapped
    TIM3->SMCR |= TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0; // Encoder mode 3 (A&B phase)
    TIM3->ARR = 10240; // Ten encoder turns before reseting
    TIM3->CR1 |= TIM_CR1_CEN; // Counter enable
}

void configure_tim4(void){
    TIM4->CCMR1 |= TIM_CCMR1_CC1S_0; // TI1 mapped
    TIM4->CCER |= TIM_CCER_CC1E; // Enable input capture
    TIM4->PSC = 239;
    
    TIM4->DIER |= TIM_DIER_CC1IE; // Enable interrupt
    NVIC_EnableIRQ(TIM4_IRQn);
    
    TIM4->CR1 |= TIM_CR1_CEN;
}