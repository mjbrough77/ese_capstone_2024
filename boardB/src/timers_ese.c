#include "../include/adc_ese.h"
#include "../include/timers_ese.h"


void configure_tim3(void){
    /* Channels 1 & 2, PWM Mode 1, Preload enable, fast enable */
	TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE  | TIM_CCMR1_OC1FE; 
	TIM3->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;	
	TIM3->CCMR1 |= TIM_CCMR1_OC2PE  | TIM_CCMR1_OC2FE;
    
    /* Enable Channels 1 & 2 */
	TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
    
    /* PWM timings */
	TIM3->PSC = 1999;   /* 20MHz/2000 = 10kHz -> 0.1ms period */
	TIM3->ARR = 199;    /* Frequency =  0.1ms * 200 = 20ms */
	TIM3->CCR1 = 15;    /* Pulse width = 1500us (Stopped) */
	TIM3->CCR2 = 15;    /* Pulse width = 1500us (Stopped) */
    
    TIM3->CR1 |= TIM_CR1_ARPE; /* Buffer ARR register */
    TIM3->EGR |= TIM_EGR_UG;   /* Initialize all registers */
    
    TIM3->CR1 |= TIM_CR1_CEN;  /* Enable counter */ 
}

void enable_tim3IRQ(void){
    TIM3->DIER |= TIM_DIER_CC1IE; /* Update signal on next pulse */
	NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM3_IRQHandler(void) {
	TIM3->CCR1 = (20 - (uint16_t)(read_joystick_y() * 11 / 4095));
	TIM3->CCR2 = (10 + (uint16_t)(read_joystick_x() * 11 / 4095));
	TIM3->SR &= ~TIM_SR_CC1IF; /* Clear interrupt */
}
