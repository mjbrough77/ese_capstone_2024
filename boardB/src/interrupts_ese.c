#include "../include/tasks_ese.h"
#include "../include/queues_ese.h"

#include "../../project_types.h"
#include "../include/adc_ese.h"
#include "../include/display_ese.h"
#include "../include/interrupts_ese.h"
#include "../include/timers_ese.h"
#include "../include/usart_ese.h"

static WheelVelocity_t speed_data;

void TIM2_IRQHandler(void){
    vTaskNotifyGiveFromISR(ultrasonic_handle, NULL);
    TIM2->SR &= ~TIM_SR_UIF;
}

/* Updates motor signals */
void TIM3_IRQHandler(void) {
    /* Values preloaded, they are applied on the next PWM period */
	TIM3->CCR1 = (uint16_t)(20 - (read_joystick_y() * 11 / 4095));
	TIM3->CCR2 = (uint16_t)(10 + (read_joystick_x() * 11 / 4095));
	TIM3->SR &= ~TIM_SR_CC1IF; /* Clear interrupt */
}

/* Gives the new speed data to display task */
void DMA1_Channel3_IRQHandler(void){
    xQueueOverwriteFromISR(speedQ, &speed_data, NULL);
    DMA1->IFCR |= DMA_IFCR_CTCIF3;
}

/* Updates USART3_Rx channel with buffer info, stops DMA transfers */
void USART3_IRQHandler(void){
    static uint8_t init = 1;
    uint32_t status = USART3->SR;
    
    /* After Board B is done initialization */
    if(init == 1 && status & USART_SR_RXNE){
        DMA1_Channel3->CMAR = (uint32_t)&speed_data;
        DMA1_Channel3->CCR |= DMA_CCR3_EN;  /* Enable USART3_Rx DMA */
        USART3->CR3 |= USART_CR3_DMAR;      /* Enable USART3_Rx requests */
        USART3->CR1 &= ~USART_CR1_RXNEIE;   /* Disable RxNE interrupt */
        
        /* Small, practically inline functions */
        start_joystick_read();
        start_ultrasonics();
        turn_on_display();
        
        (void)USART3->DR;
        init = 0;
    }
    
    else if(status & USART_SR_TC){
        USART3->CR3 &= ~USART_CR3_DMAT;     /* Stop DMA Transfers */
        USART3->SR &= ~USART_SR_TC;         /* Clear interrupt */
    }
}
