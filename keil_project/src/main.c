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
void vApplicationIdleHook(void);

volatile uint32_t count_idle = 0;

int main(void){
    board_init();

    i2c2Q = xQueueCreate(MPU_READ_ADDRS, sizeof(uint8_t));
    eeprom_logQ = xQueueCreate(1, sizeof(MPUData_t));

    xTaskCreate(mpu_reset_task, "MPU Reset", 128, NULL, 1, NULL);

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

        for(i = 0; i < MPU_RESET_STEPS; i++){
            xQueueSend(i2c2Q, &address, portMAX_DELAY);
            I2C2->CR1 |= I2C_CR1_START;
            vTaskDelay( pdMS_TO_TICKS( 100 ) );
        }

        xTaskCreate(eeprom_write_task, "EEPROM W", 128, NULL, 1, &eeprom_write_handle);
        xTaskCreate(mpu_read_task, "MPU Read", 128, NULL, 1, &mpu_read_handle);

        EXTI->IMR |= EXTI_IMR_MR6; /* mpu_read_task created, unmask INT */

        vTaskDelete(NULL);
        taskYIELD(); /* Co-operative scheduler requires explicit yield */

        (void)param;
    }
}

_Noreturn static void eeprom_write_task(void* param){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const uint8_t eeprom_write_addr = ADDR_EEPROM << 1;
    const uint32_t log_size = sizeof(LogData_t);
    const uint32_t high_address_boundary = (0x20000/log_size) * log_size;
    const uint32_t low_address_boundary = high_address_boundary - (log_size-1);

    uint8_t page_high = log_size-1;
    uint8_t page_low = 0;

    MPUData_t last_mpu_data = {0,0,0};
    LogData_t eeprom_log = {page_high,page_low,0,0,0,0,0,0,0,0,0};

    while(1){
        /* Send log every second */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS( 1000 ));

        if(I2C2->SR2 & I2C_SR2_BUSY){
            vTaskDelay( pdMS_TO_TICKS( 1 ) ); /* T_MPUREAD < 1ms */
            if(I2C2->SR2 & I2C_SR2_BUSY) vTaskSuspendAll();
        }

        xQueueReceive(eeprom_logQ, &last_mpu_data, portMAX_DELAY);

        //eeprom_log.weight_measure = (Weight_t)(ADC1->DR);
        eeprom_log.gyro_x_axis = last_mpu_data.gyro_x_axis;
        eeprom_log.gyro_y_axis = last_mpu_data.gyro_y_axis;
        eeprom_log.gyro_z_axis = last_mpu_data.gyro_z_axis;

        update_log_dma(&eeprom_log);

        xQueueSendToBack(i2c2Q, &eeprom_write_addr, portMAX_DELAY);
        //I2C2->CR1 |= I2C_CR1_START;

        /* Update write bounds */
        page_high += log_size;
        page_low  += log_size;
        if(page_high >= high_address_boundary) page_high = log_size-1;
        if(page_low >= low_address_boundary) page_low = 0;
        
        /* After sending log via queue, we can safely edit the log */
        eeprom_log.address_high = page_high;
        eeprom_log.address_low  = page_low;

        taskYIELD(); /* Co-operative scheduler requires explicit yield */

        (void)param;
    }
}

_Noreturn static void mpu_read_task(void* param){
    const uint8_t mpu_write_addr = ADDR_MPU << 1;
    const uint8_t mpu_fifo_addr = REG_FIFO;
    const uint8_t mpu_read_addr = (ADDR_MPU << 1) + 1;

    while(1){

        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        if(I2C2->SR2 & I2C_SR2_BUSY){
            vTaskDelay( pdMS_TO_TICKS( 3 ) ); /* T_ROMWRITE < 3ms */
            if(I2C2->SR2 & I2C_SR2_BUSY) vTaskSuspendAll();
        }

        xQueueSendToBack(i2c2Q, &mpu_write_addr, portMAX_DELAY);
        xQueueSendToBack(i2c2Q, &mpu_fifo_addr, portMAX_DELAY);
        xQueueSendToBack(i2c2Q, &mpu_read_addr, portMAX_DELAY);
        I2C2->CR1 |= I2C_CR1_START;

        taskYIELD(); /* Co-operative scheduler requires explicit yield */
        (void)param;
    }
}

void vApplicationIdleHook(void){
    count_idle++;
}
