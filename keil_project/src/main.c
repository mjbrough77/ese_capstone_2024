#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "../include/clocks_ese.h"
#include "../include/gpio_ese.h"
#include "../include/i2c_ese.h"
#include "../include/interrupts_ese.h"
#include "../include/timers_ese.h"
#include "../include/queues_ese.h"
#include "../include/tasks_ese.h"

static void board_init(void);
static void mpu_reset_task(void*);
static void delay(uint32_t);
static void eeprom_write_task(void*);

int main(void){
    board_init();

    i2c2Q = xQueueCreate(3, sizeof(uint8_t));

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
    uint32_t i = 0;
    uint8_t address = ADDR_MPU << 1;

    while(1){
        vTaskDelay( pdMS_TO_TICKS( 200 ) );

        while(i < MPU_RESET_STEPS){
            if(!(I2C2->SR2 & I2C_SR2_BUSY)){
                xQueueSend(i2c2Q, &address, portMAX_DELAY);
                I2C2->CR1 |= I2C_CR1_START;
                i++;
            }
            vTaskDelay( pdMS_TO_TICKS( 100 ) );
        }
        xTaskCreate(eeprom_write_task, "EEPROM W", 128, NULL, 1, NULL);
        EXTI->IMR |= EXTI_IMR_MR6;

        vTaskDelete(NULL);
        (void)param;
    }
}

_Noreturn static void eeprom_write_task(void* param){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1){
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS( 1000 ));
        if(!(I2C2->SR2 & I2C_SR2_BUSY)) I2C2->CR1 |= I2C_CR1_START;
        (void)param;
    }
}

static void delay(volatile uint32_t time){
    while(time--);
}
