#ifndef USART_ESE_H
#define USART_ESE_H

#include "stm32f10x.h"

void configure_usart3(void);
void configure_usart3_dma(void);
void send_wheelspeed_task(void*);

#endif
