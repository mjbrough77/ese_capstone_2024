#include "../../project_types.h"
#include "../include/tasks_ese.h"

#include "../include/usart_ese.h"
#include "../include/timers_ese.h"

void configure_usart3(void){
    USART3->CR1 |= USART_CR1_UE;    /* Enable USART3 */
    USART3->BRR = 0x20;             /* Baud/bitrate = 625kbps, See RM0008 */
    
    /* Avoid using DMA controller during initialization */
    
    /* Interrupt on TC, RxNE. RxNE needed during initialization then disabled */
    USART3->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE; 
    NVIC_SetPriority(USART3_IRQn, 7);
    NVIC_EnableIRQ(USART3_IRQn);
    
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; /* Enable Tx, Rx */
}

void prepare_usart3_dma(void){
    /* USART3_Tx DMA Channel, send ultrasonic data to Board T */
    DMA1_Channel2->CPAR = (uint32_t)&USART3->DR;
    DMA1_Channel2->CNDTR = sizeof(Distances_t);
    DMA1_Channel2->CCR |= DMA_CCR2_TCIE | DMA_CCR2_MINC | DMA_CCR2_DIR;
    DMA1_Channel2->CCR |= DMA_CCR2_CIRC;
    NVIC_SetPriority(DMA1_Channel2_IRQn, 7);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    /* Finished in ultrasonic_data_task() */
    
    /* USART3_Rx DMA Channel, receives speed data from Board T */
    DMA1_Channel3->CPAR = (uint32_t)&USART3->DR;
    DMA1_Channel3->CNDTR = sizeof(UsartBuffer_t);
    DMA1_Channel3->CCR |= DMA_CCR3_TCIE | DMA_CCR3_MINC;
    DMA1_Channel3->CCR |= DMA_CCR3_CIRC | DMA_CCR3_PL_0;
    NVIC_SetPriority(DMA1_Channel3_IRQn, 5);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    /* Finished in DMA1_Channel3_IRQHandler() */
}
