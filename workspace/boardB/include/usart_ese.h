/**
  *@file usart_ese.h
  *@author Mitchell Brough
  *@brief Library for configuring USART3 and tasks related to transmitting
          data over USART
  *
  *@version 1.0
  *@date 2024-03-30
  *
  *@copyright Copyright (c) 2024
 */

#ifndef USART_ESE_H
#define USART_ESE_H

void configure_usart3(void);
void prepare_usart3_dma(void);
void send_ready_signal(void);

_Noreturn void send_boardT_task(void* param);

#endif
