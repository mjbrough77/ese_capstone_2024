#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "../include/clocks_ese.h"
#include "../include/gpio_ese.h"
#include "../include/i2c_ese.h"
#include "../include/timers_ese.h"
#include "../include/queues_ese.h"
#include "../include/tasks_ese.h"

/* Macros to suspend tasks for debugging */
//#define EEPROM_TASK_SUSPEND
//#define MPU_TASK_SUSPEND    

static void board_init(void);

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
