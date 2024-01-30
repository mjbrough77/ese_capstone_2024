#include "stm32f10x.h"

#include "../include/timers_ese.h"

static const uint32_t VELOCITY_SCALE = 621107; // Velocity calculation shortcut

/**
  *@brief Configures TIM3 in encoder interface mode
 */
void configure_tim3(void){
    TIM3->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0; // TI1, TI2 mapped
    TIM3->SMCR |= TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0; // Encoder mode 3 (A&B phase)
    TIM3->CR1 |= TIM_CR1_CEN; // Counter enable
}

void configure_tim4(void){
    TIM4->CCMR1 |= TIM_CCMR1_CC1S_0; // TI1 mapped
    TIM4->CCER |= TIM_CCER_CC1E; // Enable input capture
    TIM4->PSC = 719; // One count every 30us (> 31rpm for valid count)
    
    TIM4->DIER |= TIM_DIER_CC1IE; // Enable interrupt
    NVIC_EnableIRQ(TIM4_IRQn);
    
    TIM4->CR1 |= TIM_CR1_CEN;
}

void TIM4_IRQHandler(void){
    static uint16_t encoder_count;
    static uint16_t phaseZ_time;
    static uint32_t velocity;
    uint32_t *const ptr_vel = &velocity;
    
    phaseZ_time = TIM4->CCR1;
    encoder_count = TIM3->CNT;
    
    velocity = VELOCITY_SCALE * encoder_count / phaseZ_time;
    
    /** Reset Z-phase timer count */
    TIM4->CR1 |= TIM_CR1_UDIS;
    TIM4->EGR |= TIM_EGR_UG;
    TIM4->CR1 &= ~TIM_CR1_UDIS;
    
    /** Reset Encoder count */
    TIM3->EGR |= TIM_EGR_UG;
}
