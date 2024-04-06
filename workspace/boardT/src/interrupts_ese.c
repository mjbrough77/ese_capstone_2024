#include "../../project_types.h"
#include "../include/tasks_ese.h"

#include "../include/interrupts_ese.h"
#include "../include/adc_ese.h"
#include "../include/display_ese.h"
#include "../include/timers_ese.h"
#include "../include/usart_ese.h"

static UsartBuffer_t usart_buffer = 0;

void TIM2_IRQHandler(void){
    BaseType_t wake = pdFALSE;
    
    vTaskNotifyGiveFromISR(ultrasonic_handle, &wake);
    TIM2->SR &= ~TIM_SR_UIF;
    
    portYIELD_FROM_ISR(wake);
}

/* Updates motor signals */
void TIM3_IRQHandler(void){
    BaseType_t wake = pdFALSE;
    
    vTaskNotifyGiveFromISR(motor_control_handle, &wake);
    TIM3->SR &= ~TIM_SR_CC1IF; /* Clear interrupt */
    
    portYIELD_FROM_ISR(wake);
}

void DMA1_Channel2_IRQHandler(void){
    USART3->CR3 &= ~USART_CR3_DMAT;
    DMA1->IFCR |= DMA_IFCR_CTCIF2;
}

/* Gives the new speed data to display task */
void DMA1_Channel3_IRQHandler(void){
    BaseType_t wake = pdFALSE;
    
    if(usart_buffer >= USART_CLEAR_ERROR){
        xTaskNotifyFromISR(display_handle, DISPLAY_ERROR, 
                    eSetValueWithOverwrite, &wake);
    }
    else{
        xTaskNotifyFromISR(display_handle, usart_buffer, 
                    eSetValueWithOverwrite, &wake);
    }
        
    if(usart_buffer == USART_CLEAR_ERROR){
        xTaskNotifyFromISR(motor_control_handle, CLEAR_ERR_NOTIFY, 
                    eSetBits, &wake);
    }
    else if(usart_buffer == USART_STOP_CHAIR){
        xTaskNotifyFromISR(motor_control_handle, STOP_CHAIR_NOTIFY, 
                    eSetBits, &wake);  
    }
    
    DMA1->IFCR |= DMA_IFCR_CTCIF3;
    
    portYIELD_FROM_ISR(wake);
}

/* Updates USART3_Rx channel with buffer info, stops DMA transfers */
void USART3_IRQHandler(void){
    static uint8_t init = 1;
    uint32_t status = USART3->SR;
    
    /* Board B is ready, finish configuring USART on Board T */
    if(init == 1 && status & USART_SR_RXNE && USART3->DR == USART_READY){
        DMA1_Channel3->CMAR = (uint32_t)&usart_buffer;
        DMA1_Channel3->CCR |= DMA_CCR3_EN;  /* Enable DMA_USART3_Rx */
        USART3->CR1 &= ~USART_CR1_RXNEIE;   /* Disable RxNE interrupt */
        USART3->CR3 |= USART_CR3_DMAR;      /* Enable USART3_Rx requests */

        start_joystick_read();
        turn_on_display();
        start_ultrasonics();
        
        init = 0;
    }
    
    else if(status & USART_SR_TC){
        USART3->SR &= ~USART_SR_TC; /* Clear interrupt */
    }
}
