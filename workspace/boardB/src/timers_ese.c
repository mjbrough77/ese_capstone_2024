/**
  *@file timers_ese.c
  *@author Mitchell Brough
  *@brief Library concerned with the use of STM32F103RB hardware timers
  *
  * See bottom of file for VELOCITY_FACTOR explanation
  *
  *@version 1.0
  *@date 2024-03-05
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#include "../../project_types.h"    /* VELOCITY_FACTOR, self-made typedefs */
#include "../include/tasks_ese.h"   /* FreeRTOS Task API functions */
#include "../include/queues_ese.h"  /* Queues holding velocity data */

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

void configure_tim3(void){
    TIM3->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0; /* TI1, TI2 mapped */
    TIM3->SMCR |= TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0;      /* Encoder mode 3 */
}

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
