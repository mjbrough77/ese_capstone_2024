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
    static uint8_t init = 1;
    
    xQueueOverwriteFromISR(ultrasonic_dataQ, &ultrasonic_distances, NULL);
    DMA1->IFCR |= DMA_IFCR_CTCIF3;
    
    /* Only sends speed data if BoardT gave at least 1 ultrasonic data */
    if(init == 1){
        vTaskNotifyGiveFromISR(send_speed_handle, NULL);
        init = 0;
    }
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
        /* Start DMA_USART3_Rx, place received data in buffer at `ultrasonic_distances` */
        DMA1_Channel3->CMAR = (uint32_t)&ultrasonic_distances;
        DMA1_Channel3->CCR |= DMA_CCR5_EN;
        
        /* Reset DMA1_Channel4 (I2C2_Tx) for EEPROM write */
        DMA1_Channel4->CCR &= (uint16_t)0xFFFE; /* Disable Tx DMA */
        DMA1_Channel4->CNDTR = sizeof(LogData_t);
        DMA1_Channel4->CCR |= DMA_CCR4_CIRC;
        /* DMA1_Channel 4 configuration finished in eeprom_write_task() */
        
        /* Data received over I2C2 placed in buffer at `mpu_data` */
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
    gyro_data.gyro_x_axis =  (Gyro_t)(mpu_data[0]<<8);
    gyro_data.gyro_x_axis += (Gyro_t)mpu_data[1];
    gyro_data.gyro_y_axis =  (Gyro_t)(mpu_data[2]<<8);
    gyro_data.gyro_y_axis += (Gyro_t)mpu_data[3];
    gyro_data.gyro_z_axis =  (Gyro_t)(mpu_data[4]<<8);
    gyro_data.gyro_z_axis += (Gyro_t)mpu_data[5];
    
    xQueueOverwriteFromISR(mpu_dataQ, &gyro_data, NULL);
    
    DMA1->IFCR |= DMA_IFCR_CTCIF5; /* Clear interrupt */
}

/**
  *@brief Handles I2C2 events as defined in the reference manual.
  * WARNING: There is a dead loop in this inerrupt.
  *
  * After preliminary testing, on a ReStart command (i.e. another start is
  * programmed before a stop) there must be a ~30us delay before data can be
  * loaded into the I2C2_DR register. The only quick and easy way to create
  * this delay is with a dead loop.
  *
  * Based on our applications, 30us is much too short of a time for any process
  * to be missed so this is deemed acceptable.
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
    uint16_t status = I2C2->SR1;
    uint32_t delay = 0x2F;          /* SEE BELOW */
    static uint8_t restart = 0;     /* ReStart required on MPU read */

    uint8_t queue_size = (uint8_t)uxQueueMessagesWaitingFromISR(i2c2Q);
    uint8_t address_to_send = 0;
    uint8_t register_to_send = 0;

    /* MPU6050 unique, requires 3 addresses and a restart to read data */
    if(queue_size == MPU_READ_ADDRS){
        restart = 1;
        I2C2->CR2 |= I2C_CR2_ITBUFEN; /* Enables interrupt on TxE (DMA off) */
    }

    /* A start event is followed by a device address send */
    if(status & I2C_SR1_SB){
        if(xQueueReceiveFromISR(i2c2Q, &address_to_send, NULL) == pdPASS){
            I2C2->DR = address_to_send;
        }
    }

    /* Address sent, enable DMA transfers in I2C2 peripheral */
    else if(status & I2C_SR1_ADDR){
        if(!restart) I2C2->CR2 |= I2C_CR2_DMAEN;
        (void)I2C2->SR2;
    }

    /* Program stop (exectues only on a write) */
    else if(status & I2C_SR1_BTF){
        I2C2->CR1 |= I2C_CR1_STOP;
        I2C2->CR1 &= ~I2C_CR1_STOP;
    }

    /* MPU6050 requires a second start during transmission */
    /* Only runs if we are reading data from the MPU6050 */
    else if(status & I2C_SR1_TXE){
        if(xQueueReceiveFromISR(i2c2Q, &register_to_send, NULL) == pdPASS){
            I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
            restart = 0;
            I2C2->DR = register_to_send;
            I2C2->CR1 |= I2C_CR1_START;
            while(delay--); /* WARNING: SOME DELAY REQUIRED AFTER RESTART */
        }
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
  * You'll notice TIM1 and TIM4 have near identical interrupts
  * Keen future coders will have a more efficient implementation
 */

void TIM1_CC_IRQHandler(void){
    WheelVelocity_t velocity;   /* Speed with direction */
    int16_t encoder_count;      /* Encoder mode used a signed counting method */
    uint16_t phaseZ_time;       /* Time for Z-phase to complete one rotation */
    
    phaseZ_time = TIM1->CCR1;
    encoder_count = (int16_t)TIM2->CNT; 
    
    /* Timer overflow or timer = 0 cases */
    if(TIM1->SR & TIM_SR_UIF || phaseZ_time == 0) velocity = 0; 
    else velocity = (WheelVelocity_t)(VELOCITY_FACTOR*encoder_count/phaseZ_time);
    
    /* Post updated velocity information */
    xQueueOverwriteFromISR(left_wheel_dataQ, &velocity, NULL);
    
    /* Reset Z-phase timer count */
    TIM1->CR1 |= TIM_CR1_UDIS;
    TIM1->EGR |= TIM_EGR_UG;
    TIM1->CR1 &= ~TIM_CR1_UDIS;

    /* Reset Encoder count */
    TIM2->EGR |= TIM_EGR_UG;
}

/**
  *@brief blah
  *
 */
void TIM4_IRQHandler(void){
    WheelVelocity_t velocity;   /* Speed with direction */
    int16_t encoder_count;      /* Encoder mode uses a signed counting method */
    uint16_t phaseZ_time;       /* Time for Z-phase to complete one rotation */
    
    phaseZ_time = TIM4->CCR1;
    encoder_count = (int16_t)TIM3->CNT; 
    
    /* Timer overflow or timer = 0 cases */
    if(TIM4->SR & TIM_SR_UIF || phaseZ_time == 0) velocity = 0; 
    else velocity = (WheelVelocity_t)(VELOCITY_FACTOR*encoder_count/phaseZ_time);
    
    /* Post updated velocity information */
    xQueueOverwriteFromISR(right_wheel_dataQ, &velocity, NULL);
    
    /* Reset Z-phase timer count */
    TIM4->CR1 |= TIM_CR1_UDIS;
    TIM4->EGR |= TIM_EGR_UG;
    TIM4->CR1 &= ~TIM_CR1_UDIS;

    /* Reset Encoder count */
    TIM3->EGR |= TIM_EGR_UG;
}

/* On initialization, wait until board T responds before sending speed data */
void USART3_IRQHandler(void){
    static uint8_t init = 1;
    
    USART3->SR &= ~USART_SR_TC; /* Clear interrupt */
    if(init == 1){ USART3->DR = USART_READY; init = 0; } /* Start BoardT init */
    else vTaskNotifyGiveFromISR(send_speed_handle, NULL);
}
