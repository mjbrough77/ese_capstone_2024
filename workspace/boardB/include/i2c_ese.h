/**
  *@file i2c_ese.h
  *@author Mitchell Brough
  *@brief Library related to the I2C bus and attached peripherals
  *
  * Contains tasks for all I2C2 connected devices and tasks for converting
  * data obtained over I2C
  *
  *@version 1.0
  *@date 2024-03-04
  *
  *@copyright Copyright (c)  Mitchell Brough
 */

#ifndef I2C_ESE
#define I2C_ESE

/**************************************************************************
 * Configuration function prototypes
**************************************************************************/
/**
  *@brief Configures I2C2 for 400kHz Fast Mode with error and event interrupts.
  *
  * MPU6050 requires I2C to run at 400kHz.
  * A clock of 400kHz is only possible if the clock feeding I2C2 is a multiple
  * of or equal to 10MHz.
  * For further explanations of register values, please see `RM0008.pdf`
  *
  *@pre I2C2 clock should be enabled
  *@pre AHB clock should be 20MHz
 */
void configure_i2c2(void);

/**
  *@brief Configures the I2C2 DMA channels for device initialization
  *
  *@pre DMA1 clock should be enabled
 */
void configure_i2c2_dma(void);

/**
  *@brief Unmasks line 6 of the EXTI register
  *
  * EXTI interrupts become pending when the related line is unmasked.
  * This function should be called before the first MPU6050 interrupt is
  * received but should also be called at the *very end* of initialization
  * as to not interrupt the reset process of the MPU6050
  *
  *@pre PC6 has been configured for EXTI
  *@pre The related EXTI interrupt has been enabled in the NVIC
 */
void enable_mpu_int_pin(void);

/**************************************************************************
 * Task prototypes
**************************************************************************/
/**
  *@brief Task used to reset MPU, finish system setup
  *
  * Care should be taken so that this is the only task running.
  * `I2C2_EV_IRQHandler()` and `DMA_Channel4_IRQHandler()` should be the only
  * interrupts running during the reset of the MPU6050.
  * No other task should run even when this task is blocking on a delay--this
  * is why we create most of the tasks inside this one.
  *
  * All tasks created in this function should have a priority lower than
  * this task so that no task can pre-empt
  *
  *@param param unused
  *@pre All clocks and IO lines are configured
  *@post Task is deleted as we have no further use for it
  *@post A ready signal is sent to boardT to begin its initialization
*/
_Noreturn void mpu_reset_task(void* param);

/**
  *@brief Finds the roll and yaw values for the wheelchair
  *
  * This task runs when the MPU6050 data has finished being read and so it
  * is tied to `mpu_read_task()` and `DMA_Channel5_IRQHandler()`.
  * This task unblocks on a notification from `DMA_Channel5_IRQHandler()`.
  * Execution should be less than the sampling period of the MPU6050 as to
  * avoid missing any data. Missed data can skew results since measurements
  * are relativistic.
  *
  * If calculated tilt on any axis is exceeded, the task will notify
  * `error_control_task()`
  *
  *@param param unused
  *@pre `mpu_dataQ` has been created and written to at least once
 */
_Noreturn void find_tilt_task(void* param);

/**
  *@brief Updates the log and starts a write to the EEPROM
  *
  * DMA1_Channel4 has shared access to `eeprom_log` and therefore, the log
  * information should not be updated until a complete write to the EEPROM has
  * occured. Right now, there are no controls for this--when the task unblocks,
  * it is assumes the EEPROM was successfully written to.
  *
  * Controlled access of the I2C2 bus is done through a mutex.
  * Because interrupts are used to indicate end of transmission, we must
  * poll to see if the bus is busy. As of now, this is dependent on the time
  * it takes to read from the MPU6050 which is <1ms
  *
  *@param param unused
  *@pre Data queues accessed by this task have been created
  *@pre I2C2 event interrupts and DMA_Ch4 interrupts are enabled
  *@pre The DMA controller is pointed to the buffer of the next log to send
 */
_Noreturn void eeprom_write_task(void* param);

/**
  *@brief Starts a read of the next available data from the MPU6050 FIFO buffer
  *
  * This task unblocks on the INT pin going high, and therefore is tied to the
  * EXTI interrupt. The EXTI interrupt will notify this task to unblock
  *
  * Controlled access of the I2C2 bus is done through a mutex.
  * Because interrupts are used to indicate end of transmission, we must
  * poll to see if the bus is busy. As of now, this is dependent on the time
  * it takes to write to the EEPROM which is ~1ms. Should EEPROM writes take
  * >2ms then task deadlines are not guaranteed to meet based on scheduling
  * analysis
  *
  *@param param
  *@pre i2c2Q is created
  *@pre I2C2 event interrupts and DMA_Ch5 interrupts are enabled
  *@pre The DMA controller is assigned a buffer to store the MPU data
 */
_Noreturn void mpu_read_task(void* param);

#endif
