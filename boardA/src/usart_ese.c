#include "../include/tasks_ese.h"
#include "../include/queues_ese.h"

#include "../../project_types.h"
#include "../include/timers_ese.h"
#include "../include/usart_ese.h"

void configure_usart3(void){
    USART3->CR1 |= USART_CR1_UE;    /* Enable USART3 */
    USART3->BRR = 0x10;             /* Baud Rate = 1.25Mbps, See RM0008 */
    
    USART3->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE;
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;         /* Enable Tx, Rx */   
}

void configure_usart3_dma(void){
    /* USART3_Tx DMA Channel */
    DMA1_Channel2->CPAR = (uint32_t)&USART3->DR;
    DMA1_Channel2->CNDTR = sizeof(ChairSpeed_t);
    DMA1_Channel2->CCR |= DMA_CCR2_DIR | DMA_CCR2_MINC;
    DMA1_Channel2->CCR |= DMA_CCR2_CIRC;
    
    /* USART3_Rx DMA Channel */
    DMA1_Channel3->CPAR = (uint32_t)&USART3->DR;
    DMA1_Channel3->CNDTR = sizeof(Distances_t);
    DMA1_Channel3->CCR |= DMA_CCR3_TCIE | DMA_CCR3_MINC;
    DMA1_Channel3->CCR |= DMA_CCR3_CIRC;
    
    NVIC_SetPriority(DMA1_Channel3_IRQn, 5);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}

static void set_wheelspeed_buffer_dma(ChairSpeed_t* buffer){
    DMA1_Channel2->CMAR = (uint32_t)buffer;
    DMA1_Channel2->CCR |= DMA_CCR2_EN;
}

_Noreturn void send_wheelspeed_task(void* param){
    ChairSpeed_t total_speed;
    WheelVelocity_t left_vel;
    WheelVelocity_t right_vel;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    set_wheelspeed_buffer_dma(&total_speed);
    
    /* Transfer of distance data MUST take < 5ms */
    /* Otherwise, DMA controller has concurrent access to `wheel_speeds` */
    while(1){
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS( 5 ));
        
        xQueuePeek(left_wheel_dataQ, &left_vel, NULL);
        xQueuePeek(right_wheel_dataQ, &right_vel, NULL);
        total_speed = (ChairSpeed_t)(left_vel+right_vel)/2;
        
        USART3->CR3 |= USART_CR3_DMAT; /* Start transfer of ultrasonic data */
        
        taskYIELD();
        (void)param;
    }
}
