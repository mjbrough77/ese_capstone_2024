#include "../../project_types.h"
#include "../include/usart_ese.h"

void configure_usart3(void){
    USART3->CR1 |= USART_CR1_UE;    /* Enable USART3 */
    USART3->BRR = 0x823;            /* USARTDIV = 130.3, See RM0008 */
    
    USART3->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR; /* Enable Tx, Rx DMA */
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;     /* Enable Tx, Rx */   
}

void configure_usart3_dma(void){
    /* USART3_Rx DMA Channel */
    DMA1_Channel3->CPAR = (uint32_t)&USART3->DR;
    DMA1_Channel3->CNDTR = sizeof(WheelSpeed_t);
    DMA1_Channel3->CCR |= DMA_CCR3_TCIE | DMA_CCR3_DIR | DMA_CCR3_MINC;
    NVIC_SetPriority(USART3_IRQn, 5);
    NVIC_EnableIRQ(USART3_IRQn);
}


