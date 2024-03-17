#include "../../project_types.h"
#include "../include/adc_ese.h"
#include "../include/display_ese.h"
#include "../include/interrupts_ese.h"
#include "../include/timers_ese.h"

static WheelSpeed_t speed_data;
static Ultrasonic_t ultrasonic_data[ULTRASONIC_COUNT];

void enable_tim3IRQ(void){
	NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM3_IRQHandler(void) {
    /* Values preloaded, they are applied on the next PWM period */
	TIM3->CCR1 = (uint16_t)(20 - (read_joystick_y() * 11 / 4095));
	TIM3->CCR2 = (uint16_t)(10 + (read_joystick_x() * 11 / 4095));
	TIM3->SR &= ~TIM_SR_CC1IF; /* Clear interrupt */
    
    /* Send ultrasonic data somewhere */
}

void TIM4_IRQHandler(void){
    uint16_t right_ultrasonic_read = TIM4->CCR1;
    uint16_t left_ultrasonic_read = TIM4->CCR2;
    
    /* Hand off below info to task */
    
    ultrasonic_data[0] = (right_ultrasonic_read-ULTRASONIC_RIGHT_OFFSET)*HALF_SPEED_OF_SOUND;
    ultrasonic_data[1] = (left_ultrasonic_read-ULTRASONIC_LEFT_OFFSET)*HALF_SPEED_OF_SOUND;
}

void DMA1_Channel3_IRQHandler(void){
    /* Hand off to task */
}

void USART3_IRQHandler(void){
    static uint8_t init = 1;
    uint32_t status = USART3->SR;
    
    if(init == 1 && status & USART_SR_IDLE){
        DMA1_Channel2->CMAR = (uint32_t)ultrasonic_data;
        DMA1_Channel3->CMAR = (uint32_t)&speed_data;
        
        start_joystick_read();
        start_ultrasonics();
        turn_on_display();
        
        DMA1_Channel2->CCR |= DMA_CCR2_EN;  /* Enable USART3_Tx DMA */
        DMA1_Channel3->CCR |= DMA_CCR3_EN;  /* Enable USART3_Rx DMA */
        
        USART3->CR3 |= USART_CR3_DMAR;      /* Enable USART3_Rx requests */
        
        init = 0;
    }
    
    else if(status & USART_SR_TC){
        USART3->CR3 &= ~USART_CR3_DMAT; /* Stop requests till ready */
    }
}
