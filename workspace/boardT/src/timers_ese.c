#include "../../project_types.h"
#include "../include/tasks_ese.h"

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
	TIM2->CCR1 = 1;     /* Pulse width = 10us (TRIG Pulse) */
    
    TIM2->CR1 |= TIM_CR1_ARPE; /* Buffer ARR register */
    TIM2->EGR |= TIM_EGR_UG;   /* Initialize all registers */
    
    TIM2->DIER |= TIM_DIER_UIE;     /* Interrupt each PWM period */
    NVIC_SetPriority(TIM2_IRQn, 7); /* Priority >= 5 (FreeRTOS) */
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
    NVIC_SetPriority(TIM3_IRQn, 6); /* Priority >= 5 (FreeRTOS) */
    
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
}

void start_ultrasonics(void){
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM4->CR1 |= TIM_CR1_CEN; /* TIM4 should start before TIM2 */
    TIM2->CR1 |= TIM_CR1_CEN;
}

uint16_t read_right_ultrasonic(void){
    return TIM4->CCR2;
}

uint16_t read_left_ultrasonic(void){
    return TIM4->CCR1;
}

_Noreturn void ultrasonic_data_task(void* param){
    Distances_t readings = {0,0}; /* Distances in um */
    uint32_t notify_value = 0;
    uint32_t slow_state_prev = 0;
    uint32_t slow_state_next = 0;
    uint32_t stop_state_prev = 0;
    uint32_t stop_state_next = 0;
    
    
    /* Finish configuring DMA_USART3_Tx */
    DMA1_Channel2->CMAR = (uint32_t)&readings;
    DMA1_Channel2->CCR |= DMA_CCR2_EN;
    
    /* Transfer of distance data MUST take < 60ms */
    /* Otherwise, DMA controller has concurrent access to `distances` */
    while(1){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* Unblocks by TIM2 */
        
        readings.right_data = (read_right_ultrasonic()-ULTRASONIC_RIGHT_OFFSET)*
                                HALF_SPEED_OF_SOUND;
        readings.left_data = (read_left_ultrasonic()-ULTRASONIC_LEFT_OFFSET)* 
                                HALF_SPEED_OF_SOUND;
        
        if( (readings.left_data < SLOW_DISTANCE_MIN) || 
            (readings.right_data < SLOW_DISTANCE_MIN) ){
            slow_state_next = 1;
        }
        else if( (readings.left_data >= SLOW_DISTANCE_MAX) && 
                 (readings.right_data >= SLOW_DISTANCE_MAX) ){
            slow_state_next = 0;
        }
        if(slow_state_prev == 0 && slow_state_next == 1){
            notify_value |= SLOW_CHAIR_NOTIFY;
        }
        else if(slow_state_prev == 1 && slow_state_next == 0){
            notify_value |= RESUME_SPEED_NOTIFY;
        }
        
        if( (readings.left_data < STOP_DISTANCE_MIN) || 
            (readings.right_data < STOP_DISTANCE_MIN) ){
            stop_state_next = 1;
        }
        else if( (readings.left_data >= STOP_DISTANCE_MAX) && 
                 (readings.right_data >= STOP_DISTANCE_MAX) ){
            stop_state_next = 0;
        }
        if(stop_state_prev == 0 && stop_state_next == 1){
            notify_value |= STOP_FROM_ULTRA;
        }
        else if(stop_state_prev == 1 && stop_state_next == 0){
            notify_value |= CLEAR_STOP_FROM_ULTRA;
        }
        
        if(notify_value != 0){
            xTaskNotify(motor_control_handle,notify_value,eSetBits);
            notify_value = 0;
        }
        
        slow_state_prev = slow_state_next;
        stop_state_prev = stop_state_next;
        
        USART3->CR3 |= USART_CR3_DMAT; /* Start transfer of ultrasonic data */
        (void)param;
    }
}
