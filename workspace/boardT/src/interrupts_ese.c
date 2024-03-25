#include "../../project_types.h"
#include "../include/tasks_ese.h"
#include "../include/queues_ese.h"

#include "../include/interrupts_ese.h"
#include "../include/adc_ese.h"
#include "../include/display_ese.h"
#include "../include/timers_ese.h"
#include "../include/usart_ese.h"

static UsartBuffer_t usart_buffer = 0;

void TIM2_IRQHandler(void){
    vTaskNotifyGiveFromISR(ultrasonic_handle, NULL);
    TIM2->SR &= ~TIM_SR_UIF;
}

/* Updates motor signals */
void TIM3_IRQHandler(void) {
    vTaskNotifyGiveFromISR(motor_control_handle, NULL);
    TIM3->SR &= ~TIM_SR_CC1IF; /* Clear interrupt */
}

void DMA1_Channel2_IRQHandler(void){
    USART3->CR3 &= ~USART_CR3_DMAT;
    DMA1->IFCR |= DMA_IFCR_CTCIF2;
}

/* Gives the new speed data to display task */
void DMA1_Channel3_IRQHandler(void){
    if(usart_buffer == USART_SYS_FAIL) 
        NVIC_SystemReset();
    
    else if(usart_buffer >= USART_CLEAR_ERROR){
        xTaskNotifyFromISR(print_speed_handle, DISPLAY_ERROR, 
                    eSetValueWithOverwrite, NULL);
        xTaskNotifyFromISR(motor_control_handle, usart_buffer,
                    eSetValueWithOverwrite, NULL);
    }
    
    else
        xTaskNotifyFromISR(print_speed_handle, usart_buffer, 
                        eSetValueWithOverwrite, NULL);
    
    DMA1->IFCR |= DMA_IFCR_CTCIF3;
}

/* Updates USART3_Rx channel with buffer info, stops DMA transfers */
void USART3_IRQHandler(void){
    static uint8_t init = 1;
    uint32_t status = USART3->SR;
    
    /* Board B is ready, finish configuring USART on Board T */
    if(init == 1 && status & USART_SR_RXNE && USART3->DR == USART_READY){
        DMA1_Channel3->CMAR = (uint32_t)&usart_buffer; /* Received data buffer */
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
