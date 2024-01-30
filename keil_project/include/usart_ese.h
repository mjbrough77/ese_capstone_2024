#ifndef USART_ESE_H
#define USART_ESE_H

#include "stdint.h"

#define RESET_USART_CR1 ((uint16_t) 0x0000) /**< Reset value of USARTx CR1 */
#define RESET_USART_BRR ((uint16_t) 0x0000) /**< Reset value of USARTx BRR */
#define UART_BUFFER_SIZE 256

void serial_open(void);
void serial_close(void);

#endif
