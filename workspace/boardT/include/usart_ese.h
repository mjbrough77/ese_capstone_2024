/**
  *@file usart_ese.h
  *@author Mitchell Brough
  *@brief Function prototypes used to setup USART3 and related DMA channel
  *@version 1.0
  *@date 2024-04-09
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#ifndef USART_ESE_H
#define USART_ESE_H

/**
  *@brief Sets up USART3 for communication with board B
  *
  *@pre USART3 clock is enabled
  *@pre AHB clock is 20MHz
 */
void configure_usart3(void);

/**
  *@brief Starts configuration of DMA controller channels for USART
  *
  *@pre DMA1 clock is enabled
 */
void prepare_usart3_dma(void);

#endif
