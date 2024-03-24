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


void DMA1_Channel2_IRQHandler(void){
    USART3->CR3 &= ~USART_CR3_DMAT;
    DMA1->IFCR |= DMA_IFCR_CTCIF2;
}

void DMA1_Channel3_IRQHandler(void){
    xQueueOverwriteFromISR(ultrasonic_dataQ, &ultrasonic_distances, NULL);
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
  *       0       |     GYRO_XOUT[15:8]    |         67         *
  *       1       |     GYRO_XOUT[7:0]     |         68         *
  * ----------------------------------------------------------- *
  *       2       |     GYRO_YOUT[15:8]    |         69         *
  *       3       |     GYRO_YOUT[7:0]     |         70         *
  * ----------------------------------------------------------- *
  *       4       |     GYRO_ZOUT[15:8]    |         71         *
  *       5       |     GYRO_ZOUT[7:0]     |         72         *
  * ----------------------------------------------------------- *
  *
  *@pre The queue `eeprom_mpuQ` has been created
 */
void DMA1_Channel5_IRQHandler(void){
    MPUData_t gyro_data;
    
    I2C2->CR2 &= ~I2C_CR2_DMAEN; /* Requests are disabled at every EOT */
    I2C2->CR1 |= I2C_CR1_STOP;
    
    /* See above explanation */
    gyro_data.gyro_x_axis =  (Gyro_t)((mpu_data[0]<<8) | mpu_data[1]);
    gyro_data.gyro_y_axis =  (Gyro_t)((mpu_data[2]<<8) | mpu_data[3]);
    gyro_data.gyro_z_axis =  (Gyro_t)((mpu_data[4]<<8) | mpu_data[5]);
    
    xQueueOverwriteFromISR(mpu_dataQ, &gyro_data, NULL);
    
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
        I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
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
  *@brief blah
  *
 */

void TIM1_CC_IRQHandler(void){
    TIM1->SR &= ~TIM_SR_CC1IF;
    vTaskNotifyGiveFromISR(find_velocity_left_handle, NULL);
}

/**
  *@brief blah
  *
 */
void TIM4_IRQHandler(void){
    TIM4->SR &= ~TIM_SR_CC1IF;
    vTaskNotifyGiveFromISR(find_velocity_right_handle, NULL);
}

/* Only fires when TC is asserted */
void USART3_IRQHandler(void){
    static uint8_t init = 1;
    USART3->SR &= ~USART_SR_TC; /* Clear interrupt */
    if(init == 1){ USART3->DR = USART_READY; init = 0; } /* Start BoardT init */
}
