#include "FreeRTOS.h"
#include "task.h"

#include "../include/clocks_ese.h"
#include "../include/gpio_ese.h"
#include "../include/i2c_ese.h"
#include "../include/timers_ese.h"

static void board_init(void);
static void mpu_reset_task(void* param);

int main(void){
    board_init();
    
    xTaskCreate(mpu_reset_task, "MPU Reset", 128, (void*)0, 1, NULL);
    
    while(1);
}

static void board_init(void){
    sysclock_init();
    
    clock_afio();
    clock_gpiob();
    clock_i2c2();
    clock_dma();
    
    configure_io();
    configure_i2c2();
    configure_dma();
}

_Noreturn static void mpu_reset_task(void* param){
    uint32_t i;
    
    (void)param;
    
    while(1){
        vTaskDelay( pdMS_TO_TICKS( 200 ) );
        
        for(i = 0; i < MPU_RESET_STEPS; i++){
            if(!(I2C2->SR1 & I2C_SR2_BUSY)) I2C2->CR1 |= I2C_CR1_START;
            vTaskDelay( pdMS_TO_TICKS( 100 ) );
        }
        
        vTaskDelete(NULL);
    }
}
