/**
  *@file usart_enel452.c
  *@author Mitchell Brough 200239781
  *@brief Library realted to USART configuration and sending data over USART
  *
  * ALL FUNCTIONS assume the appropriate USART/GPIO clocks are set before
  * being called
  *
  *@date 2023-11-09
 */

#include "stm32f10x.h"

#include "../include/usart_ese.h"

/**
  *@brief Enables USART to start transmission over the mcu
 */
void serial_open(void){
    USART2->CR1 |= USART_CR1_UE; /**< Enable USART */
    USART2->BRR = 0xD0; /**< Baud Rate = 115200 with SYSCLK = 24MHz */
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE; /**< Enable Tx, Rx */

    NVIC_SetPriority(USART2_IRQn, 5); /**< MUST BE >= 5 */
    USART2->CR1 |= USART_CR1_RXNEIE;  /**< Reciever interrupt enable */
    NVIC_EnableIRQ(USART2_IRQn);
}

/**
  *@brief Stops USART comms, resets registers set in serial_open()
 */
void serial_close(void){
    USART2->CR1 &= RESET_USART_CR1; /**< Reset CR1 register */
    USART2->BRR &= RESET_USART_BRR; /**< Reset BRR register */
    NVIC_DisableIRQ(USART2_IRQn);
}

/**
  *@brief Send one byte of information over USART
  * It is assumed this function will never fail, on faliure, display
  * formatting may be off as the current program does not check the status
  * of this function
  *
  *@param b the byte of information sent on line Tx
  *@return zero on succsess, one on faliure
  *@pre serial_open() has been called
 */
int sendbyte(uint8_t b){
    uint32_t i;

    /** If TC not set in reasonable time failure */
    for(i = SEND_WAIT_TIME; i > 0; i--){
            if((USART2->SR & USART_SR_TC)) break;
    }

    /** If transmit still has not yet occured, considered a failure */
    if(!(USART2->SR & USART_SR_TXE)) return 1;
    USART2->DR = b; return 0;
}

/**
  *@brief Transmits the value received back to the console
  * This is used in cases where we want to see what we are typing while
  * communicating with the mcu
  *
  *@param received_val one byte of data which was just receieved over Rx
 */
void echo_receiver(uint8_t received_val){
    sendbyte(received_val);
}
