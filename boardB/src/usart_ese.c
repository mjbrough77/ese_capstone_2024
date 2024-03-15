#include "../include/usart_ese.h"

static uint8_t speed_data[SPEED_LENGTH];

void configure_usart3(void){
    USART3->CR1 |= USART_CR1_UE;        /* Enable USART3 */
    USART3->BRR = 0x823;                /* USARTDIV = 130.3, See RM0008 */
    USART3->CR1 |= USART_CR1_RE;        /* Enable reciever */
}

void configure_usart3_dma(void){
    DMA1_Channel3->CPAR = (uint32_t)&USART3->DR;
    DMA1_Channel3->CMAR = (uint32_t)speed_data;
    DMA1_Channel3->CNDTR = SPEED_LENGTH;
    DMA1_Channel3->CCR |= DMA_CCR3_TCIE | DMA_CCR3_DIR | DMA_CCR3_MINC;
    NVIC_SetPriority(USART3_IRQn, 5);
    NVIC_EnableIRQ(USART3_IRQn);
}
