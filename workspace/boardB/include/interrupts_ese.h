/**
  *@file interrupts_ese.h
  *@author Mitchell Brough
  *@brief Library for interrupt handlers
  *@version 1.0
  *@date 2024-03-03
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
  *
 */

#ifndef INTERRUPTS_ESE_H
#define INTERRUPTS_ESE_H

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
  * Assumes only data coming to boardB is distance reading from both
  * ultrasonic sensors [um]. A notification is sent to `eeprom_write_task()`
  * to set the flag bit in the log (see project_types.h)
 */
void DMA1_Channel3_IRQHandler(void);

/**
  *@brief Updates I2C2 DMA channel requests during and after initialization.
  *
  * This interrupt triggers every EOT for I2C2_Tx.
  *
  * During initialization, this interrupt will step through the initialization
  * of the MPU6050 as defined in mpu_init (see i2c_ese.h).
  *
  * After initialization, any DMA channels used for receiving data have their
  * buffers assigned, since those buffers are statically allocated in
  * interrupts_ese.c
 */
void DMA1_Channel4_IRQHandler(void);

/**
  *@brief Moves the MPU6050 FIFO data to a queue for use by other tasks
  *
  * This interrupt only executes on EOT for I2C2_Rx.
  *
  * Because I2C2 DMA are requests are disabled after a full Rx transfer,
  * `mpu_data` cannot be modified by the DMA controller while inside this
  * interrupt and hence, `mpu_data` has its access protected
  *
  * The MPU FIFO data storage is as follows
  * ----------------------------------------------------------- *
  *     BYTE #    |         VALUE          |    Register (dec)  *
  * ----------------------------------------------------------- *
  *       0       |     ACCEL_XOUT[15:8]   |         59         *
  *       1       |     ACCEL_XOUT[7:0]    |         60         *
  * ----------------------------------------------------------- *
  *       2       |     ACCEL_YOUT[15:8]   |         61         *
  *       3       |     ACCEL_YOUT[7:0]    |         62         *
  * ----------------------------------------------------------- *
  *       4       |     ACCEL_ZOUT[15:8]   |         63         *
  *       5       |     ACCEL_ZOUT[7:0]    |         64         *
  * ----------------------------------------------------------- *
  *       6       |     GYRO_XOUT[15:8]    |         67         *
  *       7       |     GYRO_XOUT[7:0]     |         68         *
  * ----------------------------------------------------------- *
  *       8       |     GYRO_YOUT[15:8]    |         69         *
  *       9       |     GYRO_YOUT[7:0]     |         70         *
  * ----------------------------------------------------------- *
  *      10       |     GYRO_ZOUT[15:8]    |         71         *
  *      11       |     GYRO_ZOUT[7:0]     |         72         *
  * ----------------------------------------------------------- *
  *
  *@pre The queue `mpu_dataQ` has been created
  *@pre The task `find_tilt_task()` has been created
 */
void DMA1_Channel5_IRQHandler(void);

/**
  *@brief Handles I2C2 events as defined in the reference manual.
  *
  * On MPU6050 reads, we have to manually send the data register because DMA
  * cannot handle requests with length < 2. `ITBUFEN` triggers this interrupt
  * on a TxE or RxNE event, but should be turned off after clearing TxE
  * to allow for future DMA requests to be processed.
  *
  * DMA requests only occur on TxE and RxNE events and hence the DMA controller
  * cannot send an address over I2C
  *
  * `i2c2Q` should only be filled by one task and may only be added to once the
  * queue has been emptied, therefore, its access should be protected so that
  * no task can write to the queue until it is empty. The interrupt uses the
  * length of the queue to identify the type of transfer it is completing, hence
  * the need for this control
 */
void I2C2_EV_IRQHandler(void);

/**
  *@brief Handles errors during I2C2 transmission
  *
  * On an error, this interrupt will notify the `error_control_task()` to
  * reset the I2C2 peripheral.
  *
  * The I2C2 bus is never expected to have errors during transmission, so an
  * execution of this interrupt at anytime warrants review
 */
void I2C2_ER_IRQHandler(void);

/**
  *@brief Unblocks `mpu_read_task()` which starts a read of the MPU6050.
  *
  * This interrupt should only execute on the MPU6050 INT pin going high.
  * INT indicates the MPU6050 data is ready.
  *
  *@pre `mpu_read_task()` has been created with task handle `mpu_read_handle`
 */
void EXTI9_5_IRQHandler(void);

/**
  *@brief Unblocks `find_velocity_task()` to find the right wheel speed
  *
  * This interrupt is triggered on the z-phase of the right encoder going high
  *
  *@pre A task with handle `find_velocity_right_handle` has been created
 */
void TIM1_CC_IRQHandler(void);

/**
  *@brief Unblocks `find_velocity_task()` to find the left wheel speed
  *
  * This interrupt is triggered on the z-phase of the left encoder going high
  *
  *@pre A task with handle `find_velocity_left_handle` has been created
 */
void TIM4_IRQHandler(void);

/**
  *@brief Clears TC bit for multi-buffer USART transmission using the DMA
  *
  * On initialization of boardB, sends a ready signal to boardT
 */
void USART3_IRQHandler(void);

#endif
