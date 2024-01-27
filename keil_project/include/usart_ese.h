/**
  *@file usart_enel452.h
  *@author Mitchell Brough 200239781
  *@brief Macros and functions realted to USART, see usart.c
  *@date 2023-10-26
 */

#ifndef USART_ESE_H
#define USART_ESE_H

#include "stdint.h"

#define RESET_USART_CR1 ((uint16_t) 0x0000) /**< Reset value of USARTx CR1 */
#define RESET_USART_BRR ((uint16_t) 0x0000) /**< Reset value of USARTx BRR */

#define SEND_WAIT_TIME ( (uint32_t) 2400 )  /**< ~(10*(1/baud))/(1/sysclk) */

void serial_open(void);
void serial_close(void);
int sendbyte(uint8_t);
void echo_receiver(uint8_t);

#endif
