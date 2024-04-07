/**
  *@file interrupts_ese.c
  *@author Mitchell Brough
  *@brief Contains all interrupt handler definitions and serial data buffers
  *
  *@version 1.0
  *@date 2024-03-03
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
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
    USART3->CR3 &= ~USART_CR3_DMAT; /* Stop DMA transfer requests */
    DMA1->IFCR |= DMA_IFCR_CTCIF2;
}

void DMA1_Channel3_IRQHandler(void){
    xQueueOverwriteFromISR(ultrasonic_dataQ, &ultrasonic_distances, NULL);
    DMA1->IFCR |= DMA_IFCR_CTCIF3;

    if((ultrasonic_distances.left_data < SLOW_DISTANCE_MIN) ||
       (ultrasonic_distances.right_data < SLOW_DISTANCE_MIN)){
       xTaskNotifyFromISR(eeprom_write_handle, DISTANCE_NOTIFY, eSetBits, NULL);
    }
}

void DMA1_Channel4_IRQHandler(void){
    static uint8_t mpu_reset_index = 1;
    static uint8_t init = 1;

    I2C2->CR2 &= ~I2C_CR2_DMAEN; /* Requests should be disabled every EOT */

    /* Initial Reset of MPU6050, each iteration steps through reset matrix */
    if(mpu_reset_index < MPU_RESET_STEPS){
        DMA1_Channel4->CCR &= (uint16_t)0xFFFE; /* Disable Tx DMA */
        DMA1_Channel4->CMAR = (uint32_t)mpu_init[mpu_reset_index++];
        DMA1_Channel4->CNDTR = MPU_SINGLE_WRITE;
        DMA1_Channel4->CCR |= DMA_CCR4_EN;
    }

    /* Should only execute once at end of MPU6050 reset*/
    else if(init == 1){
        /* Enable DMA_USART3_Rx, assign buffer location */
        DMA1_Channel3->CMAR = (uint32_t)&ultrasonic_distances;
        DMA1_Channel3->CCR |= DMA_CCR5_EN;

        /* Reset DMA1_I2C2_Tx for EEPROM write */
        DMA1_Channel4->CCR &= (uint16_t)0xFFFE; /* Disable Tx DMA */
        DMA1_Channel4->CNDTR = sizeof(LogData_t);
        DMA1_Channel4->CCR |= DMA_CCR4_CIRC;

        /*
         * DMA1_I2C2_Tx configuration finished in eeprom_write_task()
         */

        /* Enable DMA_I2C2_Rx, assign buffer location */
        DMA1_Channel5->CMAR = (uint32_t)mpu_data;
        DMA1_Channel5->CCR |= DMA_CCR5_EN;

        init = 0;
    }

    DMA1->IFCR |= DMA_IFCR_CTCIF4; /* Clear interrupt */
}

void DMA1_Channel5_IRQHandler(void){
    MPUData_t fifo_data;
    BaseType_t wake = pdFALSE;
    
    I2C2->CR2 &= ~I2C_CR2_DMAEN;    /* Requests are disabled at every EOT */
    I2C2->CR1 |= I2C_CR1_STOP;      /* Generate stop condition */

    /* See explanation in interrupts_ese.h */
    fifo_data.accel_x_axis = (Accel_t)((mpu_data[0]<<8) | mpu_data[1]);
    fifo_data.accel_y_axis = (Accel_t)((mpu_data[2]<<8) | mpu_data[3]);
    fifo_data.accel_z_axis = (Accel_t)((mpu_data[4]<<8) | mpu_data[5]);
    fifo_data.gyro_x_axis =   (Gyro_t)((mpu_data[6]<<8) | mpu_data[7]);
    fifo_data.gyro_y_axis =   (Gyro_t)((mpu_data[8]<<8) | mpu_data[9]);
    fifo_data.gyro_z_axis =   (Gyro_t)((mpu_data[10]<<8) | mpu_data[11]);

    xQueueOverwriteFromISR(mpu_dataQ, &fifo_data, NULL);
    vTaskNotifyGiveFromISR(find_tilt_handle, &wake);

    DMA1->IFCR |= DMA_IFCR_CTCIF5; /* Clear interrupt */
    
    portYIELD_FROM_ISR(wake);
}

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

void I2C2_ER_IRQHandler(void){
    BaseType_t wake = pdFALSE;
    
    I2C2->SR1 &= ~0xDF00; /* Clear all error flags */
    xTaskNotifyFromISR(system_error_handle, I2C2_ERR_NOTIFY, eSetBits, &wake);
    
    portYIELD_FROM_ISR(wake);
}

void EXTI9_5_IRQHandler(void){
    BaseType_t wake = pdFALSE;
    
    EXTI->PR |= EXTI_PR_PR6;
    vTaskNotifyGiveFromISR(mpu_read_handle, &wake);
    
    portYIELD_FROM_ISR(wake);
}

void TIM1_CC_IRQHandler(void){
    BaseType_t wake = pdFALSE;
    
    TIM1->SR &= ~TIM_SR_CC1IF;
    vTaskNotifyGiveFromISR(find_velocity_right_handle, &wake);
    
    portYIELD_FROM_ISR(wake);
}

void TIM4_IRQHandler(void){
    BaseType_t wake = pdFALSE;
    
    TIM4->SR &= ~TIM_SR_CC1IF;
    vTaskNotifyGiveFromISR(find_velocity_left_handle, &wake);
    
    portYIELD_FROM_ISR(wake);
}

void USART3_IRQHandler(void){
    static uint8_t init = 1;
    USART3->SR &= ~USART_SR_TC; /* Clear interrupt */
    if(init == 1){ USART3->DR = USART_READY; init = 0; } /* Start BoardT init */
}
