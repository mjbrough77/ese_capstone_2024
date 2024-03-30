/**
  *@file timers_ese.c
  *@author Mitchell Brough
  *@brief Library concerned with the use of STM32F103RB hardware timers
  * See bottom of file for VELOCITY_FACTOR explanation
  *@version 1.0
  *@date 2024-03-05
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
  *
 */
#include "../../project_types.h"
#include "../include/tasks_ese.h"
#include "../include/queues_ese.h"

#include "../include/timers_ese.h"

void configure_tim1(void){
    TIM1->CCMR1 |= TIM_CCMR1_CC1S_0;    /* TI1 mapped */
    TIM1->CCER |= TIM_CCER_CC1E;        /* Enable input capture */
    TIM1->PSC = 39;                     /* One count every 2us */

    TIM1->DIER |= TIM_DIER_CC1IE;       /* Enable interrupt */
    NVIC_SetPriority(TIM1_CC_IRQn, 8);
    NVIC_EnableIRQ(TIM1_CC_IRQn);
}

void configure_tim2(void){
    TIM2->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0; /* TI1, TI2 mapped */
    TIM2->SMCR |= TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0;      /* Encoder mode 3 */
}

/**
  *@brief Configures TIM3 in encoder interface mode
  *
  *@pre The TIM3 clock should be enabled
 */
void configure_tim3(void){
    TIM3->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0; /* TI1, TI2 mapped */
    TIM3->SMCR |= TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0;      /* Encoder mode 3 */
}

/**
  *@brief Configures TIM4 to latch count on Z-phase going high
  *
  *@pre The TIM4 clock should be enabled
 */
void configure_tim4(void){
    TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;    /* TI1 mapped */
    TIM4->CCER |= TIM_CCER_CC1E;        /* Enable input capture */
    TIM4->PSC = 39;                     /* One count every 2us */

    TIM4->DIER |= TIM_DIER_CC1IE;       /* Enable interrupt */
    NVIC_SetPriority(TIM4_IRQn, 8);
    NVIC_EnableIRQ(TIM4_IRQn);      
}

void start_encoder_readings(void){
    TIM1->CR1 |= TIM_CR1_CEN;
    TIM2->CR1 |= TIM_CR1_CEN;
    TIM3->CR1 |= TIM_CR1_CEN;
    TIM4->CR1 |= TIM_CR1_CEN;
}

/* Encoder count is assumed to never equal 0xFFFF */
_Noreturn void find_velocity_task(void* timers){
    WheelVelocity_t velocity;   /* Speed with direction */
    int16_t negative_mask;      /* Used to find absolute speed */
    int16_t encoder_count;      /* Encoder mode used a signed counting method */
    uint16_t phaseZ_time;       /* Time for Z-phase to complete one rotation */
    
    EncoderTimers_t* timers_used = (EncoderTimers_t*)timers;
    TIM_TypeDef* z_tim = timers_used->z_phase_timer;
    TIM_TypeDef* e_tim = timers_used->encoder_timer;
    
    while(1){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        phaseZ_time = z_tim->CCR1;
        encoder_count = (int16_t)e_tim->CNT;
        
        /* Calculate velocity using magnitude only */
        negative_mask = encoder_count >> 15;
        encoder_count ^= negative_mask;
        encoder_count -= negative_mask;
        
        /* Timer overflow or timer = 0 cases */
        if(z_tim->SR & TIM_SR_UIF || phaseZ_time == 0) velocity = 0; 
        else velocity = (VELOCITY_FACTOR*encoder_count/phaseZ_time);
        z_tim->SR &= ~TIM_SR_UIF; /* Clear update flag */
        if(negative_mask) velocity = -velocity;
        
        /* Post updated velocity information */
        xQueueOverwrite(timers_used->side, &velocity);
        
        /* Reset Z-phase timer count without clearing CCRx registers */
        z_tim->CR1 |= TIM_CR1_UDIS;
        z_tim->EGR |= TIM_EGR_UG;
        z_tim->CR1 &= ~TIM_CR1_UDIS;

        /* Reset Encoder count */
        e_tim->EGR |= TIM_EGR_UG;
    }
}

/**
  * -------------------------- About VELOCITY_FACTOR ---------------------------
  *
  * Simplifies velocity calculation while maintaining precision
  *
  * To prevent slow floating point computations, we can express the velocity
  * in terms of a multiplication factor. Then, for the final calculation, we
  * only need one multiplication operator and one division--simple!
  *
  * The E6B2-CZ3E encoders are quadrature encoders meaning that each pulse is
  * sampled four times. Our encoders are rated 1024 pulses ber rotation (PPR)
  * so for one full rotation, we instead would count 4096 pulses
  *
  * Based on some preliminary testing, the wheel to motor turn ratio is 30.
  * Although there is no *official* spec for this, 1:30 is common for wheelchair
  * DC motors, so we are confident in this estimate.
  *
  * We are also limited in our time measurements as the 16-bit timers on the
  * STM32F103RB are limited to 16 bits or a total count of 2^16-1 = 65535
  *
  * Speed information to the user is limited to 0.1km/h and with some math
  * we find that each tick of TIM1/TIM4 needs to be > 1.83us, any shorter and
  * the counter would roll over before the Z-phase goes high
  *
  * Encoder count should be reset after one full rotation using the Z-phase
  * so that encoder_count/PPR <= 1 we do this to get a finer precision for
  * measurements by limiting the total number of digits used
  *
  * PPR = 4096
  * Gear Ratio = 30
  * Wheel radius = 15.875cm (6.25")
  * Timer prescaler = 40 -> 20MHz/40 = 500kHz or 2us timer period
  *
  * angular_position    = [(encoder_count/PPR) * 2pi] / Gear Ratio
  *                     = 5.11327e-5 * encoder_count in radians
  *
  * linear_distance     = angular_position * wheel_radius
  *                     = 8.11732e-6 * encoder_count in meters
  *
  * velocity            = linear_distance / (timer_count * timer_period)
  *                     = 4.50865 * encoder_count / timer_count in m/s
  *                     = 14.6112 * encoder_count / timer_count in km/h
  *
  * We can then scale the constant to remove any floating point decimals and
  * just use a scaled integer instead. After scaling by 10^4 we are left with
  * the final value stored as an integer (e.g. 3.2187km/h is stored as 32187)
 */
