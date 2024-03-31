/**
  *@file usart_ese.h
  *@author Mitchell Brough
  *@brief Library for configuring USART3 and tasks related to transmitting
  *       data over USART
  *
  *@version 1.0
  *@date 2024-03-30
  *
  *@copyright Copyright (c) 2024
 */

#ifndef USART_ESE_H
#define USART_ESE_H

/**
  *@brief Sets USART3 baud rate, enables Tx, Rx, DMA_Rx
  *
  * The receiver is enabled with DMA transfers to begin accepting
  * ultrasonic data. DMA transfers are not yet enabled on the transmitter
  * because we first send a "ready" byte so that boardT can finish initializing.
  * The next received data will be from the ultrasonics
  *
  * We only ever expect to receive ultrasonic data over USART
  *
  *@pre USART3 clock enabled
  *@pre AHB clock at 20MHz
 */
void configure_usart3(void);

/**
  *@brief Configures most of USART3 DMA controllers
  *
  * While most of DMA1_Ch2 and DMA1_Ch3 (Tx, Rx for USART3) are configured,
  * buffers have yet to be assigned. In general, DMA controllers that
  * transmit data have their buffers assigned inside tasks and DMA controllers
  * that receive data have their buffers assigned after the MPU6050 is
  * reset.
  *
  * Note that a DMA channel must be disabled to assign a buffer to it
  *
  *@pre DMA1 clock is enabled
 */
void prepare_usart3_dma(void);

/**
  *@brief Enables USART3 interrupts on the TC flag
  *
  * On initialization, DMA_Ch2 is disabled because we only want to transmit
  * one byte of data to indicate to boardT that this board has finished
  * initialization.
  *
  * When the USART3 transmitter is enabled, TC immediately goes high to prepare
  * the next data to be sent. So when we unmask the TC interrupt, the program
  * will immediately enter the `USART3_IRQHandler()`
  *
  * On first entry of `USART3_IRQHandler()` the ready byte will be sent
  * but subsequent entries will only clear TC because for multi-buffered
  * transmission using the DMA, TC must be low before DMA requests can be
  * processed.
 */
void send_ready_signal(void);

/**
  *@brief Sends data over USART3 for boardT to process
  *
  * If none of our sensor values have been exceeded (tilt, weight, etc.) then
  * this task will only send the net forward speed of the wheelchair.
  *
  * When a sensor value is exceeded, then a STOP command for the motor
  * will be sent. No other data is sent over USART until the sensor goes
  * back below the excess reading, at which point a CLEAR signal will be sent
  * and speed data will resume transmission.
  *
  * As of now, the data currently sent over USART is:
  * READY signal
  * Speed data
  * STOP signal
  * CLEAR signal
  *
  * This task assumes there is never concurrent access to USART3 and therefore
  * does *not* have any error controls to check if other data is in transmission
  *
  *@param param unused
 */
_Noreturn void send_boardT_task(void* param);

#endif
