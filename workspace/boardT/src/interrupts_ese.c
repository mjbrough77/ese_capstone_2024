#include "../../project_types.h"
#include "../include/tasks_ese.h"
#include "../include/queues_ese.h"

#include "../include/interrupts_ese.h"
#include "../include/adc_ese.h"
#include "../include/display_ese.h"
#include "../include/timers_ese.h"
#include "../include/usart_ese.h"

static ChairSpeed_t speed_data = 0;

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

void DMA1_Channel2_IRQHandler(void){
    USART3->CR3 &= ~USART_CR3_DMAT;
    DMA1->IFCR |= DMA_IFCR_CTCIF2;
}

/* Gives the new speed data to display task */
void DMA1_Channel3_IRQHandler(void){
    xQueueOverwriteFromISR(speedQ, &speed_data, NULL);
    vTaskNotifyGiveFromISR(print_speed_handle, NULL);   /* Print new speed */
    DMA1->IFCR |= DMA_IFCR_CTCIF3;
}

/* Updates USART3_Rx channel with buffer info, stops DMA transfers */
void USART3_IRQHandler(void){
    static uint8_t init = 1;
    uint32_t status = USART3->SR;
    
    /* Board B is ready, finish configuring USART on Board T */
    if(init == 1 && status & USART_SR_RXNE && USART3->DR == USART_READY){
        DMA1_Channel3->CMAR = (uint32_t)&speed_data; /* Received data buffer */
        DMA1_Channel3->CCR |= DMA_CCR3_EN;           /* Enable DMA_USART3_Rx */
        USART3->CR1 &= ~USART_CR1_RXNEIE;            /* Disable RxNE interrupt */
        USART3->CR3 |= USART_CR3_DMAR;               /* Enable USART3_Rx requests */

        start_joystick_read();
        turn_on_display();
        start_ultrasonics();
        
        init = 0;
    }
    
    else if(status & USART_SR_TC){
        USART3->SR &= ~USART_SR_TC; /* Clear interrupt */
    }
}
