#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "../include/clocks_ese.h"
#include "../include/gpio_ese.h"
#include "../include/i2c_ese.h"
#include "../include/timers_ese.h"
#include "../include/queues_ese.h"
#include "../include/tasks_ese.h"

static void board_init(void);
static void mpu_reset_task(void*);
static void eeprom_write_task(void*);
static void mpu_read_task(void*);

int main(void){
    board_init();

    i2c2Q = xQueueCreate(MPU_READ_ADDRS, sizeof(uint8_t));
    eeprom_logQ = xQueueCreate(1, sizeof(MPUData_t));

    xTaskCreate(mpu_reset_task, "MPU Reset", 128, NULL, 0, NULL);

    vTaskStartScheduler();

    while(1);
}

static void board_init(void){
    sysclock_init();

    clock_afio();
    clock_gpiob();
    clock_gpioc();
    clock_i2c2();
    clock_dma();

    configure_io();
    configure_i2c2();
    configure_i2c2_dma();
}

_Noreturn static void mpu_reset_task(void* param){
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

_Noreturn static void eeprom_write_task(void* param){
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

_Noreturn static void mpu_read_task(void* param){
    const uint8_t MPU_WRITE_ADDR = ADDR_MPU << 1;
    const uint8_t MPU_READ_ADDR = (ADDR_MPU << 1) + 1;
    const uint8_t MPU_FIFO_ADDR = REG_FIFO;

    while(1){
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
