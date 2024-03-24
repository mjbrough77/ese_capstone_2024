#ifndef USART_ESE_H
#define USART_ESE_H

#include "stm32f10x.h"

void configure_usart3(void);
void prepare_usart3_dma(void);
void send_ready_signal(void);

_Noreturn void send_speed_task(void*);

#endif
