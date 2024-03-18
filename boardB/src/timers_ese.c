#include "../include/adc_ese.h"
#include "../include/timers_ese.h"

/* Ultrasonic TRIG pin */
void configure_tim2(void){
    /* Channel 2, PWM Mode 1, Preload enable, fast enable */
    TIM2->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;	
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE  | TIM_CCMR1_OC1FE;
    
    TIM2->CR2 |= TIM_CR2_MMS_2;  /* Set OC1REF as TRGO */
	TIM2->CCER |= TIM_CCER_CC1E; /* Enable Channel 1 */
    
    /* PWM timings */
	TIM2->PSC = 199;    /* 20MHz/200 = 100kHz -> 0.01ms period */
	TIM2->ARR = 5999;   /* Period =  0.01ms * 6000 = 60ms */
	TIM2->CCR2 = 1;     /* Pulse width = 10us (TRIG Pulse) */
    
    TIM2->CR1 |= TIM_CR1_ARPE; /* Buffer ARR register */
    TIM2->EGR |= TIM_EGR_UG;   /* Initialize all registers */
    
    TIM2->DIER |= TIM_DIER_UIE;     /* Interrupt each PWM period */
    NVIC_SetPriority(TIM2_IRQn, 5); /* Priority >= 5 (FreeRTOS) */
}

/* Motor driver signal */
void configure_tim3(void){
    /* Channels 1 & 2, PWM Mode 1, Preload enable, fast enable */
	TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE  | TIM_CCMR1_OC1FE; 
	TIM3->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;	
	TIM3->CCMR1 |= TIM_CCMR1_OC2PE  | TIM_CCMR1_OC2FE;
    
	TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E; /* Enable Channels 1 & 2 */
    
    /* PWM timings */
	TIM3->PSC = 1999;   /* 20MHz/2000 = 10kHz -> 0.1ms period */
	TIM3->ARR = 199;    /* Period =  0.1ms * 200 = 20ms */
	TIM3->CCR1 = 15;    /* Pulse width = 1500us (Stopped) */
	TIM3->CCR2 = 15;    /* Pulse width = 1500us (Stopped) */
    
    TIM3->CR1 |= TIM_CR1_ARPE;      /* Buffer ARR register */
    TIM3->EGR |= TIM_EGR_UG;        /* Initialize all registers */
    
    TIM3->DIER |= TIM_DIER_CC1IE;   /* Interrupt on falling edge of PWM */
    NVIC_SetPriority(TIM3_IRQn, 5); /* Priority >= 5 (FreeRTOS) */
    
    TIM3->CR1 |= TIM_CR1_CEN;       /* Start motor signal ASAP */ 
}

/* Ultrasonics, captures falling edge of echo pins CCR1 = RIGHT, CCR2 = LEFT */
void configure_tim4(void){
    TIM4->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0; /* Map TI1, TI2 */
    
    TIM4->SMCR |= TIM_SMCR_SMS_2; /* Reset timer every echo signal */
    TIM4->SMCR |= TIM_SMCR_TS_0;  /* Trigger on ITR1 (TRGO from TIM2) */
    
    TIM4->CCER |= TIM_CCER_CC1P | TIM_CCER_CC2P; /* Latch on falling edge */
    TIM4->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E; /* Enable channels 1 & 2 */
    
    TIM4->PSC = 19; /* 20MHz/20 = 1MHz -> 1us tick */
 
    TIM4->EGR |= TIM_EGR_UG;   /* Initialize all registers */
    
    TIM4->DIER |= TIM_DIER_CC1IE | TIM_DIER_CC2IE; /* Interrupt on ECHO */
}

void start_ultrasonics(void){
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM4->CR1 |= TIM_CR1_CEN; /* TIM4 should start before TIM2 */
    TIM2->CR1 |= TIM_CR1_CEN;
}

uint16_t read_right_ultrasonic(void){
    return TIM4->CCR1;
}

uint16_t read_left_ultrasonic(void){
    return TIM4->CCR2;
}
