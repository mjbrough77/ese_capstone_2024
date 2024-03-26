/**
  *@file interrupts_ese.c
  *@author Mitchell Brough
  *@brief Contains all interrupt handler definitions and serial data buffers
  *
  *@version 1.0
  *@date 2024-03-03
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
  *
 */

#include "../../project_types.h"
#include "../include/queues_ese.h"
#include "../include/tasks_ese.h"

#include "../include/interrupts_ese.h"
#include "../include/i2c_ese.h"
#include "../include/timers_ese.h"

/**
  *@brief Buffer for storing Ultrasonic Data
  * ONLY ACCSSESED BY `DMA1_Channel3_IRQHandler()` and the DMA controller
 */
static Distances_t ultrasonic_distances;

/**
  *@brief Buffer for storing MPU Data
  * ONLY ACCSSESED BY `DMA1_Channel5_IRQHandler()` and the DMA controller
 */
static uint8_t mpu_data[MPU_FIFO_READ];

/**
  *@brief Executes before last byte is sent over USART
 */
void DMA1_Channel2_IRQHandler(void){
    USART3->CR3 &= ~USART_CR3_DMAT; /* Stop DMA transfer requests */
    DMA1->IFCR |= DMA_IFCR_CTCIF2;
}

/**
  *@brief USART data from boardT recieved, place into queue
  * Assumes only data coming to boardB is ultrasonic data
 */
void DMA1_Channel3_IRQHandler(void){
    xQueueOverwriteFromISR(ultrasonic_dataQ, &ultrasonic_distances, NULL);
    if(ultrasonic_distances.left_data < MAX_DISTANCE 
        || ultrasonic_distances.right_data < MAX_DISTANCE )
        xTaskNotifyFromISR(eeprom_write_handle, DISTANCE_EV, eSetBits, NULL);
    
    DMA1->IFCR |= DMA_IFCR_CTCIF3;
}

/**
  *@brief Updates I2C2 DMA channel requests during and after initialization.
  * This interrupt triggers every EOT for I2C2_Tx.
  *
  * During initialization, this interrupt will step through the initialization
  * of the MPU6050 as defined in mpu_init (see i2c_ese.h).
  *
  * After initialization, I2C2_Tx DMA channel is reconfigured to write to the
  * EEPROM. I2C2_Rx is also updated, now placing data in the local global
  * buffer `mpu_data`
  *
 */
void DMA1_Channel4_IRQHandler(void){
    static uint8_t mpu_reset_index = 1;
    static uint8_t init = 1;

    I2C2->CR2 &= ~I2C_CR2_DMAEN; /* Requests are disabled at every EOT */

    /* Initial Reset of MPU, each iteration steps through reset matrix */
    if(mpu_reset_index < MPU_RESET_STEPS){
        DMA1_Channel4->CCR &= (uint16_t)0xFFFE; /* Disable Tx DMA */
        DMA1_Channel4->CMAR = (uint32_t)mpu_init[mpu_reset_index++];
        DMA1_Channel4->CNDTR = MPU_SINGLE_WRITE;
        DMA1_Channel4->CCR |= DMA_CCR4_EN;
    }

    /* Should only execute once */
    else if(init == 1){
        /* Enable DMA_USART3_Rx, assign buffer location */
        DMA1_Channel3->CMAR = (uint32_t)&ultrasonic_distances;
        DMA1_Channel3->CCR |= DMA_CCR5_EN;
        
        /* Reset DMA1_I2C2_Tx for EEPROM write */
        DMA1_Channel4->CCR &= (uint16_t)0xFFFE; /* Disable Tx DMA */
        DMA1_Channel4->CNDTR = sizeof(LogData_t);
        DMA1_Channel4->CCR |= DMA_CCR4_CIRC;
        /* DMA1_I2C2_Tx configuration finished in eeprom_write_task() */
        
        /* Enable DMA_I2C2_Rx, assign buffer location */
        DMA1_Channel5->CMAR = (uint32_t)mpu_data;
        DMA1_Channel5->CCR |= DMA_CCR5_EN;
        
        init = 0;
    }

    DMA1->IFCR |= DMA_IFCR_CTCIF4; /* Clear interrupt */
}

/**
  *@brief Sends the MPU6050 FIFO data to eeprom_write_task for logging.
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
  *@pre The queue `eeprom_mpuQ` has been created
  *@pre The task `calculate_rotation_task` has been created
 */
void DMA1_Channel5_IRQHandler(void){
    MPUData_t fifo_data;
    
    I2C2->CR2 &= ~I2C_CR2_DMAEN;    /* Requests are disabled at every EOT */
    I2C2->CR1 |= I2C_CR1_STOP;      /* Generate stop condition */
    
    /* See above explanation */
    fifo_data.accel_x_axis = (Accel_t)((mpu_data[0]<<8) | mpu_data[1]);
    fifo_data.accel_y_axis = (Accel_t)((mpu_data[2]<<8) | mpu_data[3]);
    fifo_data.accel_z_axis = (Accel_t)((mpu_data[4]<<8) | mpu_data[5]);
    fifo_data.gyro_x_axis =   (Gyro_t)((mpu_data[6]<<8) | mpu_data[7]);
    fifo_data.gyro_y_axis =   (Gyro_t)((mpu_data[8]<<8) | mpu_data[9]);
    fifo_data.gyro_z_axis =   (Gyro_t)((mpu_data[10]<<8) | mpu_data[11]);
    
    xQueueOverwriteFromISR(mpu_dataQ, &fifo_data, NULL);
    vTaskNotifyGiveFromISR(calc_rotation_handle, NULL);
    
    DMA1->IFCR |= DMA_IFCR_CTCIF5; /* Clear interrupt */
}

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
 */
void I2C2_EV_IRQHandler(void){
    static uint8_t restart = 0;     /* ReStart required on MPU read */
    uint16_t status = I2C2->SR1;
    uint8_t queue_size = (uint8_t)uxQueueMessagesWaitingFromISR(i2c2Q);
    uint8_t address_to_send = 0;
    uint8_t register_to_send = 0;

    /* MPU6050 unique, requires 3 addresses and a restart to read data */
    if(queue_size == MPU_READ_ADDRS){
        restart = 2;
        I2C2->CR2 |= I2C_CR2_ITBUFEN; /* Enables interrupt on TxE (DMA off) */
    }

    /* A start event is followed by a device address send */
    if(status & I2C_SR1_SB){
        xQueueReceiveFromISR(i2c2Q, &address_to_send, NULL);
        I2C2->DR = address_to_send;
    }

    /* Address sent, enable DMA transfers in I2C2 peripheral */
    else if(status & I2C_SR1_ADDR){
        if(!restart) I2C2->CR2 |= I2C_CR2_DMAEN;
        (void)I2C2->SR2;
    }

    /* MPU6050 FIFO read step one: send FIFO register address */
    else if(restart == 2 && status & I2C_SR1_TXE){
        xQueueReceiveFromISR(i2c2Q, &register_to_send, NULL);
        I2C2->CR2 &= ~I2C_CR2_ITBUFEN; /* Turn off for DMA transfers */
        I2C2->DR = register_to_send;
        restart--;
    }
    
    /* MPU6050 FIFO read step two: send repeated start */
    else if(restart == 1 && status & I2C_SR1_BTF){
        I2C2->CR1 |= I2C_CR1_START;
        I2C2->CR1 &= ~I2C_CR1_START; /* START needs manual clearing on BTF */
        restart--;
    }
    
    /* Send stop condition (only executes on a write) */
    else if(status & I2C_SR1_BTF){
        I2C2->CR1 |= I2C_CR1_STOP;
        I2C2->CR1 &= ~I2C_CR1_STOP; /* STOP needs manually clearing on BTF */
    }
}

/**
  *@brief An error on the I2C2 bus is unrecoverable, start a system reset
 */
void I2C2_ER_IRQHandler(void){
    BaseType_t wake = pdFALSE;
    I2C2->SR1 &= ~0xDF00; /* Clear all error flags */
    xTaskNotifyFromISR(system_error_handle, I2C2_NOTIFY, eSetBits, &wake);
    portYIELD_FROM_ISR(wake);
}

/**
  *@brief Unblocks `mpu_read_task` which starts a read of the MPU6050.
  * This interrupt should only execute on the MPU6050 INT pin going high.
  * INT indicates the MPU6050 data is ready.
  *
  *@pre `mpu_read_task` has been created with task handle `mpu_read_handle`
 */
void EXTI9_5_IRQHandler(void){
    EXTI->PR |= EXTI_PR_PR6;
    vTaskNotifyGiveFromISR(mpu_read_handle, NULL);
}

/**
  *@brief Unblocks `find_velocity_task()` to find the left wheel speed
  *
  *@pre A task with handle `find_velocity_left_handle` has been created
 */
void TIM1_CC_IRQHandler(void){
    TIM1->SR &= ~TIM_SR_CC1IF;
    vTaskNotifyGiveFromISR(find_velocity_left_handle, NULL);
}

/**
  *@brief Unblocks `find_velocity_task()` to find the right wheel speed
  *
  *@pre A task with handle `find_velocity_right_handle` has been created
 */
void TIM4_IRQHandler(void){
    TIM4->SR &= ~TIM_SR_CC1IF;
    vTaskNotifyGiveFromISR(find_velocity_right_handle, NULL);
}

/**
  *@brief Clears TC bit for multi-buffer USART using the DMA
  * On initialization of boardB, sends a ready signal to boardT
 */
void USART3_IRQHandler(void){
    static uint8_t init = 1;
    USART3->SR &= ~USART_SR_TC; /* Clear interrupt */
    if(init == 1){ USART3->DR = USART_READY; init = 0; } /* Start BoardT init */
}
