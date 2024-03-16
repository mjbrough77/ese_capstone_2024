#include "../../project_types.h"
#include "../include/adc_ese.h"
#include "../include/interrupts_ese.h"

static uint8_t speed_data[sizeof(WheelSpeed_t)];
static Ultrasonic_t ultrasonic_data[ULTRASONIC_COUNT];

void enable_tim3IRQ(void){
	NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM3_IRQHandler(void) {
    /* Values preloaded, hold on next PWM period */
	TIM3->CCR1 = (20 - (uint16_t)(read_joystick_y() * 11 / 4095));
	TIM3->CCR2 = (10 + (uint16_t)(read_joystick_x() * 11 / 4095));
	TIM3->SR &= ~TIM_SR_CC1IF; /* Clear interrupt */
    
    /* Send ultrasonic data somewhere */
}

void TIM4_IRQHandler(void){
    uint16_t right_ultrasonic_read = TIM4->CCR1;
    uint16_t left_ultrasonic_read = TIM4->CCR2;
    
    ultrasonic_data[0] = (right_ultrasonic_read-ULTRASONIC_RIGHT_OFFSET)*HALF_SPEED_OF_SOUND;
    ultrasonic_data[1] = (left_ultrasonic_read-ULTRASONIC_LEFT_OFFSET)*HALF_SPEED_OF_SOUND;
}


void DMA1_Channel3_IRQHandler(void){
    
}

void DMA1_Channel4_IRQHandler(void){
    
}
