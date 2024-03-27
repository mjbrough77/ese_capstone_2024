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

#include "math.h"
#include "../../project_types.h"
#include "../include/tasks_ese.h"
#include "../include/queues_ese.h"
#include "../include/semaphore_ese.h"

#include "../include/adc_ese.h"
#include "../include/i2c_ese.h"
#include "../include/timers_ese.h"
#include "../include/usart_ese.h"


const uint8_t mpu_init[MPU_RESET_STEPS][MPU_SINGLE_WRITE] = {
    {REG_PWR_MGMT_1, 0x80},         /* Device Reset */
    {REG_SIGNAL_PATH_RESET, 0x07},  /* Sensor Reset */
    {REG_PWR_MGMT_1, 0x01},         /* PLL with x-axis gyro ref */
    {REG_USER_CTRL, 0x04},          /* Reset FIFO */
    {REG_SMPLRT_DIV, 0x07},         /* Sample rate = 125Hz (ready every 8ms) */
    {REG_CONFIG, 0x01},             /* DLPF setting 1 */
    {REG_GYRO_CONFIG, 0x08},        /* Gyro range +-500 degrees/s */
    {REG_ACCEL_CONFIG, 0x08},       /* Accel range +-4g */
    {REG_FIFO_EN, 0x78},            /* 3-axis gyro, accel into FIFO */
    {REG_USER_CTRL, 0x40},          /* Enable FIFO */
    {REG_INT_ENABLE, 0x01}          /* Data ready interrupt enable */
};


/**************************************************************************
 * Configuration Functions
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
    I2C2->CR2 |= I2C_CR2_ITERREN;               /* Error interrupt enable */

    NVIC_SetPriority(I2C2_EV_IRQn, 5); /* ISR Priority >= 5 (FreeRTOS) */
    NVIC_SetPriority(I2C2_ER_IRQn, 5);
    NVIC_EnableIRQ(I2C2_EV_IRQn);
    NVIC_EnableIRQ(I2C2_ER_IRQn);

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
    /* DMA1_Channel5 finished configuration in DMA1_Channel4_IRQHandler() */
    NVIC_SetPriority(DMA1_Channel5_IRQn, 5); /* ISR Priority >= 5 (FreeRTOS) */
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

void enable_mpu_int_pin(void){
    EXTI->IMR |= EXTI_IMR_MR6; /* mpu_read_task created, unmask INT */
}


/**************************************************************************
 * I2C2 Peripheral Tasks
**************************************************************************/
/**
  *@brief Task used to reset MPU, finish system setup
  *
  *@param param unused
*/
_Noreturn void mpu_reset_task(void* param){
#ifndef MPU_RESET_SKIP
    uint32_t i;
    uint8_t address = ADDR_MPU << 1;
#endif
    EncoderTimers_t left_encoder_timers = {TIM1, TIM2, left_wheel_dataQ};
    EncoderTimers_t right_encoder_timers = {TIM4, TIM3, right_wheel_dataQ};
    
    while(1){
        vTaskDelay( pdMS_TO_TICKS( 200 ) );
    #ifndef MPU_RESET_SKIP
        /* I2C2 bus assumed free, no other access to take place */
        for(i = 0; i < MPU_RESET_STEPS; i++){
            xQueueSend(i2c2Q, &address, portMAX_DELAY);
            I2C2->CR1 |= I2C_CR1_START;
            vTaskDelay( pdMS_TO_TICKS( 100 ) );
        }
    #endif
        /* Create tasks, initialize rest of board */
        xTaskCreate(eeprom_write_task,"EEPROM",128,NULL,2,&eeprom_write_handle);
        xTaskCreate(mpu_read_task,"MPU Read",128,NULL,2,&mpu_read_handle);
        xTaskCreate(find_rotation_task,"Rotate",128,NULL,1,&calc_rotation_handle);
        xTaskCreate(send_boardT_task,"Speed",128,NULL,1,&send_boardT_handle);
        xTaskCreate(get_weight_task,"Weight",128,NULL,1,NULL);
        xTaskCreate(find_velocity_task, "RightV",128,
                    (void*)&right_encoder_timers,1,&find_velocity_right_handle);
        xTaskCreate(find_velocity_task,"LeftV",128,
                    (void*)&left_encoder_timers,1,&find_velocity_left_handle);
        
        start_encoder_readings();
        enable_mpu_int_pin();
        send_ready_signal();
        
        vTaskDelete(NULL); /* No further use for this task */
        (void)param;
    }
}

_Noreturn void find_rotation_task(void* param){
    float accel_x, accel_y, accel_z;    /* [g] acceleration */
    float gyro_x, gyro_z;               /* [deg/s] rotational velocity */
    float angle_x_gyro;                 /* [deg] angle from gyro */
    float angle_x_accel;                /* [deg] angle from accel */
    float x_z_resultant;                /* [g] accel vector on x-z plane */
    float roll, yaw;                    /* [deg] angle about x, z-axis */
    
    uint8_t tilt_exceeded_prev = 0;
    uint8_t tilt_exceeded_next = 0;
    MPUData_t raw_mpu_data = {0,0,0,0,0,0};
    angle_x_gyro = 0.0f;
    roll  = yaw = 0.0f;
    
    while(1){
    #ifdef ROTATION_TASK_SUSPEND
        vTaskSuspend(NULL);
    #endif
        
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        xQueuePeek(mpu_dataQ, &raw_mpu_data, NULL);
        accel_x = raw_mpu_data.accel_x_axis / ACCEL_SENSITIVITY;
        accel_y = raw_mpu_data.accel_y_axis / ACCEL_SENSITIVITY;
        accel_z = raw_mpu_data.accel_z_axis / ACCEL_SENSITIVITY;
        gyro_x = raw_mpu_data.gyro_x_axis / GYRO_SENSITIVITY - GYRO_X_OFFSET;
        gyro_z = raw_mpu_data.gyro_z_axis / GYRO_SENSITIVITY - GYRO_Z_OFFSET;
        
        x_z_resultant = sqrtf( accel_x*accel_x + accel_z*accel_z );
        angle_x_accel = atanf( accel_y / x_z_resultant) * 180.0f/PI;
        angle_x_accel -= ACCEL_X_OFFSET;
        angle_x_gyro += gyro_x * MPU_SAMPLE_TIME;
        
        roll = (0.0f * angle_x_gyro) + (1.0f * angle_x_accel);
        yaw += gyro_z * MPU_SAMPLE_TIME;
        
        if(fabsf(roll) > MAX_TILT || fabsf(yaw) > MAX_TILT){
            xTaskNotify(system_error_handle, MAXTILT_NOTIFY, eSetBits);
            xTaskNotify(eeprom_write_handle, MAXTILT_NOTIFY, eSetBits);
            tilt_exceeded_next = 1;
        }
        
        else
            tilt_exceeded_next = 0;
        
        if(tilt_exceeded_prev == 1 && tilt_exceeded_next == 0){
            xTaskNotify(system_error_handle, CLEAR_ERR_NOTIFY, eSetBits);
        }

        tilt_exceeded_prev = tilt_exceeded_next;
        (void)param;
    }
}

/**
  *@brief Updates the log and starts a write to the EEPROM
  *
  * DMA1_Channel4 has shared access to `eeprom_log` and therefore, the log
  * information should not be updated until a complete write to the EEPROM has
  * occured. Right now this is okay, because it is only updated before the
  * the start of every transmission
  *
  *@param param unused
 */
_Noreturn void eeprom_write_task(void* param){
    const uint32_t LOG_SIZE = sizeof(LogData_t);
    const uint8_t CTRL_BLOCK = 0x2; /* Block bit of the control byte */
    
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint32_t last_event = 0;

    /* Initailize EEPROM locations */
    uint8_t control_byte = ADDR_EEPROM << 1;
    uint16_t address_to_write = 0x0000;
    uint8_t address_high = 0x00;
    uint8_t address_low  = 0x00;
    PageNum_t new_page_num = 0;
    PageNum_t old_page_num = 0;

    /* Initialize log data */
    MPUData_t last_mpu_data = {0,0,0,0,0,0};
    WheelVelocity_t left_wheel_speed = 0;
    WheelVelocity_t right_wheel_speed = 0;
    Distances_t distance_data = {0,0};
    LogData_t eeprom_log = {address_high,address_low,0,0,0,0,0,0,0,0,0,0,0,0};

    /* Set log address for DMA */
    DMA1_Channel4->CMAR = (uint32_t)&eeprom_log;
    DMA1_Channel4->CCR |= DMA_CCR4_EN;

    while(1){
    #ifdef EEPROM_TASK_SUSPEND
        vTaskSuspend(NULL);
    #endif

        /**************************** Period = 1s ****************************/
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS( 1000 ));

        /**************************** Update Log ****************************/
        
        xQueuePeek(mpu_dataQ, &last_mpu_data, NULL);
        xQueuePeek(left_wheel_dataQ, &left_wheel_speed, NULL);
        xQueuePeek(right_wheel_dataQ, &right_wheel_speed, NULL);
        xQueuePeek(ultrasonic_dataQ, &distance_data, NULL);
        xTaskNotifyWait(0,0xFFFFFFFF,&last_event,NULL);
        
        eeprom_log.address_high = address_high;
        eeprom_log.address_low  = address_low;
        eeprom_log.event_flags = (uint8_t)last_event;
        eeprom_log.weight_measure = (Weight_t)(ADC1->DR);
        eeprom_log.ultrasonic_left = distance_data.left_data;
        eeprom_log.ultrasonic_right = distance_data.right_data;
        eeprom_log.left_wheel_speed = left_wheel_speed;
        eeprom_log.right_wheel_speed = right_wheel_speed;
        eeprom_log.accel_x_axis = last_mpu_data.accel_x_axis;
        eeprom_log.accel_y_axis = last_mpu_data.accel_y_axis;
        eeprom_log.accel_z_axis = last_mpu_data.accel_z_axis;
        eeprom_log.gyro_x_axis = last_mpu_data.gyro_x_axis;
        eeprom_log.gyro_y_axis = last_mpu_data.gyro_y_axis;
        eeprom_log.gyro_z_axis = last_mpu_data.gyro_z_axis;

        /**************************** Start Write ***************************/
        xSemaphoreTake(i2c2_mutex, portMAX_DELAY);
            while(I2C2->SR2 & I2C_SR2_BUSY);
            xQueueSendToBack(i2c2Q, &control_byte, portMAX_DELAY);
            I2C2->CR1 |= I2C_CR1_START;
        xSemaphoreGive(i2c2_mutex);

        /*********************** Update Write Address ************************/
        address_to_write += LOG_SIZE;

        /* Will this write extend beyond the page bound? */
        new_page_num = (address_to_write + LOG_SIZE) / PAGE_SIZE;

        /* Move to next addressable block of memory space */
        if(new_page_num != 0 && new_page_num % PAGES_PER_BLOCK == 0){
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

        /* Assumes i2c2Q empty, task will block indefinitely otherwise */
        xSemaphoreTake(i2c2_mutex, portMAX_DELAY);
            while(I2C2->SR2 & I2C_SR2_BUSY);
            xQueueSendToBack(i2c2Q, &MPU_WRITE_ADDR, portMAX_DELAY);
            xQueueSendToBack(i2c2Q, &MPU_FIFO_ADDR, portMAX_DELAY);
            xQueueSendToBack(i2c2Q, &MPU_READ_ADDR, portMAX_DELAY);
            I2C2->CR1 |= I2C_CR1_START;
        xSemaphoreGive(i2c2_mutex);
        
        (void)param;
    }
}
