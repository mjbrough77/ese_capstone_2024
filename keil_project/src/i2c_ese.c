/**
  *@file i2c_ese.c
  *@author Mitchell Brough
  *@brief Source file associated with the I2C bus and connected peripherals
  * Contains tasks for peripherals on the bus (EEPROM and MPU)
  *@version 1.0
  *@date 2024-03-04
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
  *
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "../include/i2c_ese.h"
#include "../include/tasks_ese.h"
#include "../include/queues_ese.h"

/* Macros to suspend tasks for debugging */
#define EEPROM_TASK_SUSPEND
//#define MPU_TASK_SUSPEND    


/**************************************************************************
 * MPU RESET ARRAY (GLOBAL CONST, SAFE TO ACCESS)
**************************************************************************/
const uint8_t mpu_init[MPU_RESET_STEPS][MPU_SINGLE_WRITE] = {
    {REG_PWR_MGMT_1, 0x80},         /* Device Reset */
    {REG_SIGNAL_PATH_RESET, 0x07},  /* Sensor Reset */
    {REG_PWR_MGMT_1, 0x01},         /* PLL with x-axis gyro ref */
    {REG_USER_CTRL, 0x04},          /* Reset FIFO */
    {REG_SMPLRT_DIV, 0x07},         /* Sample rate = 125Hz (ready every 8ms) */
    {REG_CONFIG, 0x01},             /* DLPF setting 1 */
    {REG_GYRO_CONFIG, 0x08},        /* Gyro range +-500 degrees/s */
    {REG_FIFO_EN, 0x70},            /* 3-axis gyro into FIFO */
    {REG_USER_CTRL, 0x40},          /* Enable FIFO */
    {REG_INT_ENABLE, 0x01}          /* Data ready interrupt enable */
};


/**************************************************************************
 * CONFIGURATION FUNCTIONS
**************************************************************************/
/**
  *@brief Configures I2C2 for 400kHz Fast Mode with event interrupts.
  *
  * MPU6050 requires I2C to run at 400kHz
  *
  *@pre I2C2 clock should be enabled
 */
void configure_i2c2(void){
    I2C2->CR2 |= 20;                            /* AHB_clk = 20MHz */
    I2C2->CCR |= I2C_CCR_FS | I2C_CCR_DUTY | 2; /* 400kHz Fm with 16/9 DC */
    I2C2->TRISE = 7;                            /* 300ns / 50ns = 6 + 1 = 7 */
    I2C2->CR2 |= I2C_CR2_ITEVTEN;               /* Event interrupt enable */

    NVIC_SetPriority(I2C2_EV_IRQn, 5); /* ISR Priority >= 5 (FreeRTOS) */
    NVIC_EnableIRQ(I2C2_EV_IRQn);

    I2C2->CR1 |= I2C_CR1_PE;    /* Peripheral Enable */
    I2C2->CR1 |= I2C_CR1_ACK;   /* Acknowledge on Rx */
    I2C2->CR2 |= I2C_CR2_LAST;  /* NACK on next DMA EOT */
}

/**
  *@brief Configures the I2C2 DMA channels for device initialization
  *
  *@pre DMA1 clock should be enabled
 */
void configure_i2c2_dma(void){
    /* I2C2_Tx DMA channel, configured to reset MPU6050 */
    DMA1_Channel4->CPAR = (uint32_t)&I2C2->DR;
    DMA1_Channel4->CMAR = (uint32_t)mpu_init[0];
    DMA1_Channel4->CNDTR = MPU_SINGLE_WRITE;
    DMA1_Channel4->CCR |= DMA_CCR4_TCIE | DMA_CCR4_DIR | DMA_CCR4_MINC;
    DMA1_Channel4->CCR |= DMA_CCR4_EN;

    NVIC_SetPriority(DMA1_Channel4_IRQn, 5); /* ISR Priority >= 5 (FreeRTOS) */
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    /* I2C2_Rx DMA channel, configured to read MPU6050 */
    DMA1_Channel5->CPAR = (uint32_t)&I2C2->DR;
    DMA1_Channel5->CNDTR = MPU_FIFO_READ;
    DMA1_Channel5->CCR |= DMA_CCR5_TCIE | DMA_CCR5_MINC | DMA_CCR5_CIRC;

    NVIC_SetPriority(DMA1_Channel5_IRQn, 5); /* ISR Priority >= 5 (FreeRTOS) */
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

/**
  *@brief Gives DMA_Channel4 (I2C2_Tx) the buffer location of the eeprom log
  *
  *@param log The data to be saved on the EEPROM
  *@pre DMA1 clock should be enabled
 */
void update_log_dma(LogData_t *log){
    DMA1_Channel4->CMAR = (uint32_t)log;
    DMA1_Channel4->CCR |= DMA_CCR4_EN;
}


/**************************************************************************
 * I2C2 Peripheral Tasks
**************************************************************************/
_Noreturn void mpu_reset_task(void* param){
    uint32_t i;
    uint8_t address = ADDR_MPU << 1;

    while(1){
        vTaskDelay( pdMS_TO_TICKS( 200 ) );

        /* I2C2 bus assumed free, no other access to take place */
        for(i = 0; i < MPU_RESET_STEPS; i++){
            xQueueSend(i2c2Q, &address, portMAX_DELAY);
            I2C2->CR1 |= I2C_CR1_START;
            vTaskDelay( pdMS_TO_TICKS( 100 ) );
        }

        xTaskCreate(eeprom_write_task, "EEPROM W", 128, NULL, 0, &eeprom_write_handle);
        xTaskCreate(mpu_read_task, "MPU Read", 128, NULL, 0, &mpu_read_handle);

        EXTI->IMR |= EXTI_IMR_MR6; /* mpu_read_task created, unmask INT */

        vTaskDelete(NULL); /* No further use for this task */
        taskYIELD(); /* Co-operative scheduler requires explicit yield */

        (void)param;
    }
}

_Noreturn void eeprom_write_task(void* param){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    const uint32_t LOG_SIZE = sizeof(LogData_t);
    const uint8_t CTRL_BLOCK = 0x2; /* Block bit of the control byte */
    
    uint8_t control_byte = ADDR_EEPROM << 1;
    uint16_t address_to_write = 0x0000;
    
    /* Big endian storage, have to break down otherwise address in reverse */
    uint8_t address_high = 0x00;
    uint8_t address_low  = 0x00;
    
    /* Used to check page bounds */
    PageNum_t new_page_num = 0;
    PageNum_t old_page_num = 0;
    
    /* Initialize log data */
    MPUData_t last_mpu_data = {0,0,0};
    LogData_t eeprom_log = {address_high,address_low,0,0,0,0,0,0,0,0,0};

    /* Set log address for DMA */
    update_log_dma(&eeprom_log);
    
    while(1){
        #ifdef EEPROM_TASK_SUSPEND
            vTaskSuspend(NULL);
        #endif
        
        /**************************** PERIOD = 1s ****************************/
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS( 1000 ));
        
        /**************************** ERROR CHECK ****************************/
        if(I2C2->SR2 & I2C_SR2_BUSY){
            vTaskDelay( pdMS_TO_TICKS( 1 ) ); /* T_MPUREAD < 1ms */
            
            /* Bus error, suspend scheduler */
            if(I2C2->SR2 & I2C_SR2_BUSY) 
                vTaskSuspendAll();
        }
        
        /**************************** UPDATE LOG ****************************/
        /* MPU data must be read between write sessions */
        xQueueReceive(eeprom_logQ, &last_mpu_data, portMAX_DELAY);

        eeprom_log.address_high = address_high;
        eeprom_log.address_low  = address_low;
//        eeprom_log.weight_measure = (Weight_t)(ADC1->DR);
//        eeprom_log.ultrasonic_left;
//        eeprom_log.ultrasonic_right;
        eeprom_log.gyro_x_axis = last_mpu_data.gyro_x_axis;
        eeprom_log.gyro_y_axis = last_mpu_data.gyro_y_axis;
        eeprom_log.gyro_z_axis = last_mpu_data.gyro_z_axis;
//        eeprom_log.left_wheel_speed;
//        eeprom_log.right_wheel_speed;
//        eeprom_log.event_flags;
        
        /**************************** START WRITE ***************************/
        xQueueSendToBack(i2c2Q, &control_byte, portMAX_DELAY);
        I2C2->CR1 |= I2C_CR1_START;
        
        /*********************** UPDATE WRITE ADDRESS ************************/
        address_to_write += LOG_SIZE;
        
        /* Will this write extend beyond the page bound? */
        new_page_num = (address_to_write + LOG_SIZE) / PAGE_SIZE;
        
        /* Move to next addressable block of memory space */
        if(new_page_num != 0 && new_page_num % PAGE_PER_BLOCK == 0){
            control_byte ^= CTRL_BLOCK;
            new_page_num = 0;
        }
        
        /* Move to start of next page boundary if not enough space */
        if(new_page_num != old_page_num)
            address_to_write = PAGE_SIZE * new_page_num;
        
        /* Update addresses */
        address_high = (address_to_write & 0xFF00) >> 8;
        address_low  = (address_to_write & 0xFF);

        old_page_num = new_page_num;       
        
        /****************************** YIELD ********************************/
        taskYIELD(); /* Co-operative scheduler requires explicit yield */
        (void)param;
    }
}

_Noreturn void mpu_read_task(void* param){
    const uint8_t MPU_WRITE_ADDR = ADDR_MPU << 1;
    const uint8_t MPU_READ_ADDR = (ADDR_MPU << 1) + 1;
    const uint8_t MPU_FIFO_ADDR = REG_FIFO;

    while(1){
        #ifdef MPU_TASK_SUSPEND
            vTaskSuspend(NULL);
        #endif
       
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if(I2C2->SR2 & I2C_SR2_BUSY){
            vTaskDelay( pdMS_TO_TICKS( 2 ) ); /* T_EEPROMW < 2ms */
            
            /* Bus error, suspend scheduler */
            if(I2C2->SR2 & I2C_SR2_BUSY) 
                vTaskSuspendAll(); 
        }

        xQueueSendToBack(i2c2Q, &MPU_WRITE_ADDR, portMAX_DELAY);
        xQueueSendToBack(i2c2Q, &MPU_FIFO_ADDR, portMAX_DELAY);
        xQueueSendToBack(i2c2Q, &MPU_READ_ADDR, portMAX_DELAY);
        I2C2->CR1 |= I2C_CR1_START;

        taskYIELD(); /* Co-operative scheduler requires explicit yield */
        (void)param;
    }
}
