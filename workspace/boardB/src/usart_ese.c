#include "../include/tasks_ese.h"
#include "../include/queues_ese.h"

#include "../../project_types.h"
#include "../include/timers_ese.h"
#include "../include/usart_ese.h"

void configure_usart3(void){
    USART3->CR1 |= USART_CR1_UE;    /* Enable USART3 */
    USART3->BRR = 0x10;             /* Baud Rate = 1.25Mbps, See RM0008 */
    
    USART3->CR3 |= USART_CR3_DMAR;
    USART3->CR1 |= USART_CR1_TCIE;
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; /* Enable Tx, Rx */   
}

void prepare_usart3_dma(void){
    /* USART3_Tx DMA Channel */
    DMA1_Channel2->CPAR = (uint32_t)&USART3->DR;
    DMA1_Channel2->CNDTR = sizeof(ChairSpeed_t);
    DMA1_Channel2->CCR |= DMA_CCR2_DIR | DMA_CCR2_MINC;
    DMA1_Channel2->CCR |= DMA_CCR2_CIRC;
    /* DMA1_Channel2 finished configuration in send_speed_task() */
    
    /* USART3_Rx DMA Channel */
    DMA1_Channel3->CPAR = (uint32_t)&USART3->DR;
    DMA1_Channel3->CNDTR = sizeof(Distances_t);
    DMA1_Channel3->CCR |= DMA_CCR3_TCIE | DMA_CCR3_MINC;
    DMA1_Channel3->CCR |= DMA_CCR3_CIRC;
    /* DMA1_Channel3 finished configuration in DMA1_Channel4_IRQHandler() */
    
    NVIC_SetPriority(DMA1_Channel3_IRQn, 5);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}

_Noreturn void send_speed_task(void* param){
    ChairSpeed_t total_speed;
    WheelVelocity_t left_vel;
    WheelVelocity_t right_vel;
    
    /* Finish configuring DMA_USART3_Tx */
    DMA1_Channel2->CMAR = (uint32_t)&total_speed;
    DMA1_Channel2->CCR |= DMA_CCR2_EN;
    
    /* Constantly transfers wheel speed data */
    while(1){
    #ifdef SEND_SPEED_TASK_SUSPEND
        vTaskSuspend(NULL);
    #endif

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* Unblocks on USART3 TC */
        
        xQueuePeek(left_wheel_dataQ, &left_vel, NULL);
        xQueuePeek(right_wheel_dataQ, &right_vel, NULL);
        total_speed = (ChairSpeed_t)(left_vel+right_vel)/2;
        
        USART3->CR3 |= USART_CR3_DMAT; /* Start transfer of ultrasonic data */
        
        taskYIELD();
        (void)param;
    }
}
