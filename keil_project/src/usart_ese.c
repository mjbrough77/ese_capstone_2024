#include "../include/usart_ese.h"

/**
  *@brief Enables USART to start transmission over the mcu
 */
void serial_open(void){
    USART2->CR1 |= USART_CR1_UE; /**< Enable USART */
    USART2->BRR = 0xD0; /**< Baud Rate = 115200 with SYSCLK = 24MHz */
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE; /**< Enable Tx, Rx */

    USART2->CR1 |= USART_CR1_RXNEIE; /**< Rx interrupt enable */
    USART2->CR1 |= USART_CR1_TXEIE;  /**< Tx interrupt enable */
    NVIC_EnableIRQ(USART2_IRQn);
}

/**
  *@brief Stops USART comms, resets registers set in serial_open()
 */
void serial_close(void){
    NVIC_DisableIRQ(USART2_IRQn);
    USART2->CR1 &= RESET_USART_CR1; /**< Reset CR1 register */
    USART2->BRR &= RESET_USART_BRR; /**< Reset BRR register */
}
