#include "../../project_types.h"
#include "../include/tasks_ese.h"
#include "../include/queues_ese.h"

#include "../include/usart_ese.h"
#include "../include/timers_ese.h"

void configure_usart3(void){
    USART3->CR1 |= USART_CR1_UE;    /* Enable USART3 */
    USART3->BRR = 0x20;             /* Baud/bitrate = 625kbps, See RM0008 */
    
    /* Avoid using DMA controller during initialization */
    
    /* Interrupt on TC, RxNE. RxNE needed during initialization then disabled */
    USART3->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE; 
    NVIC_SetPriority(USART3_IRQn, 5);
    NVIC_EnableIRQ(USART3_IRQn);
    
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; /* Enable Tx, Rx */
}

void prepare_usart3_dma(void){
    /* USART3_Tx DMA Channel, send ultrasonic data to Board T */
    DMA1_Channel2->CPAR = (uint32_t)&USART3->DR;
    DMA1_Channel2->CNDTR = sizeof(Distances_t);
    DMA1_Channel2->CCR |= DMA_CCR2_TCIE | DMA_CCR2_MINC | DMA_CCR2_DIR;
    DMA1_Channel2->CCR |= DMA_CCR2_CIRC;
    NVIC_SetPriority(DMA1_Channel2_IRQn, 5);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    /* Finished in ultrasonic_data_task() */
    
    /* USART3_Rx DMA Channel, receives speed data from Board T */
    DMA1_Channel3->CPAR = (uint32_t)&USART3->DR;
    DMA1_Channel3->CNDTR = sizeof(ChairSpeed_t);
    DMA1_Channel3->CCR |= DMA_CCR3_TCIE | DMA_CCR3_MINC;
    DMA1_Channel3->CCR |= DMA_CCR3_CIRC;
    NVIC_SetPriority(DMA1_Channel3_IRQn, 5);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    /* Finished in DMA1_Channel3_IRQHandler() */
}

_Noreturn void ultrasonic_data_task(void* param){
    Distances_t readings = {0,0}; /* Distances in um */
    
    /* Finish configuring DMA_USART3_Tx */
    DMA1_Channel2->CMAR = (uint32_t)&readings;
    DMA1_Channel2->CCR |= DMA_CCR2_EN;
    
    /* Transfer of distance data MUST take < 60ms */
    /* Otherwise, DMA controller has concurrent access to `distances` */
    while(1){
    #ifdef SEND_ULTRASONIC_TASK_SUSPEND
        vTaskSuspend(NULL);
    #endif
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* Unblocks by TIM2 */
        
        readings.right_data = (read_right_ultrasonic()-ULTRASONIC_RIGHT_OFFSET)
                            * HALF_SPEED_OF_SOUND;
        readings.left_data = (read_left_ultrasonic()-ULTRASONIC_LEFT_OFFSET)
                            * HALF_SPEED_OF_SOUND;
        
        USART3->CR3 |= USART_CR3_DMAT; /* Start transfer of ultrasonic data */
        
        (void)param;
    }
}
