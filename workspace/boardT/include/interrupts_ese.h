/**
  *@file interrupts_ese.h
  *@author Mitchell Brough
  *@brief Library for interrupt handlers
  *@version 1.0
  *@date 2024-03-03
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#ifndef INTERRUPTS_ESE_H
#define INTERRUPTS_ESE_H

/**
  * @brief Unblocks the conversion of ultrasonic data to a distance
  *
  * This interrupt is triggered on the falling edge of ultrasonic with the
  * reading the closest distance to an obstacle
 */
void TIM2_IRQHandler(void);

/**
  * @brief Unblocks update of new motor driver signal
  *
  * This interupts is triggered on the first falling edge of one of the motor
  * driver signals
 */
void TIM3_IRQHandler(void);

/**
  *@brief Stops processing USART3_Tx requests after a completed transfer
  *
  * DMA1_Channel2 -> USART3_Tx
  *
  * DMA1_Channel2 is configured in circular mode, so to stop sending the same
  * buffer information repeatedly, we stop DMA requests from USART3_Tx in this
  * interrupt
 */
void DMA1_Channel2_IRQHandler(void);

/**
  *@brief USART data from boardT recieved, place into queue
  *
  * Assumes data coming from boardB is display information, a notification
  * to stop, or a notification to clear a stop. Only one piece of information
  * is sent at a time, buffer is overwritten on next data recieved
 */
void DMA1_Channel3_IRQHandler(void);

/**
  * @brief Clears TC bit in USART3 or initializes board if ready signal recieved
  *
  * TC bit must be cleared in the case of multi-buffer communication
 */
void USART3_IRQHandler(void);

#endif
