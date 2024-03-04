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
    eeprom_logQ = xQueueCreate(1, sizeof(MPUBuffer_t));

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

        xTaskCreate(eeprom_write_task, "EEPROM W", 128, NULL, 2, &eeprom_write_handle);
        xTaskCreate(mpu_read_task, "MPU Read", 128, NULL, 1, &mpu_read_handle);

        EXTI->IMR |= EXTI_IMR_MR6;

        vTaskDelete(NULL);
        taskYIELD();

        (void)param;
    }
}

_Noreturn static void eeprom_write_task(void* param){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const uint8_t eeprom_write_addr = ADDR_EEPROM << 1;
    uint8_t page_high = 0x15;
    uint8_t page_low = 0x0;
    MPUBuffer_t last_mpu_data = NULL;
    LogData_t eeprom_log = {page_high,page_low,0,0,0,0,0,0,0,0,0};

    while(1){
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS( 1000 ));

        if(I2C2->SR2 & I2C_SR2_BUSY){
            vTaskDelay( pdMS_TO_TICKS( 1 ) ); /* T_MPUREAD < 1ms */
            if(I2C2->SR2 & I2C_SR2_BUSY) vTaskSuspendAll();
        }

        xQueueReceive(eeprom_logQ, &last_mpu_data, portMAX_DELAY);

        eeprom_log.weight_measure = (Weight_t)(ADC1->DR);
        eeprom_log.gyro_x_axis = (GyroRead_t)(last_mpu_data[0]<<8)+(GyroRead_t)last_mpu_data[1];
        eeprom_log.gyro_y_axis = (GyroRead_t)(last_mpu_data[2]<<8)+(GyroRead_t)last_mpu_data[2];
        eeprom_log.gyro_x_axis = (GyroRead_t)(last_mpu_data[4]<<8)+(GyroRead_t)last_mpu_data[3];

        update_log_dma(&eeprom_log);

        xQueueSendToBack(i2c2Q, &eeprom_write_addr, portMAX_DELAY);
        I2C2->CR1 |= I2C_CR1_START;

        page_high = (page_high + 0x15) % 0x20000;
        page_low  = (page_low + 0x15) % 0x1FFEB;
        eeprom_log.address_high = page_high;
        eeprom_log.address_low  = page_low;

        taskYIELD();
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

        xQueueSendToBack(i2c2Q, &mpu_write_addr, NULL);
        xQueueSendToBack(i2c2Q, &mpu_fifo_addr, NULL);
        xQueueSendToBack(i2c2Q, &mpu_read_addr, NULL);
        I2C2->CR1 |= I2C_CR1_START;

        taskYIELD();
        (void)param;
    }
}
