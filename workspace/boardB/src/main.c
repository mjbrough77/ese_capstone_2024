#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "../../project_types.h"
#include "../include/clocks_ese.h"
#include "../include/gpio_ese.h"
#include "../include/i2c_ese.h"
#include "../include/queues_ese.h"
#include "../include/semaphore_ese.h"
#include "../include/tasks_ese.h"
#include "../include/timers_ese.h"
#include "../include/usart_ese.h"

static void board_init(void);

int main(void){
    board_init();

    /* Mutex for I2C2 access */
    i2c2_mutex = xSemaphoreCreateMutex();
    
    /* Queue creation */
    i2c2Q = xQueueCreate(MPU_READ_ADDRS, sizeof(uint8_t));
    mpu_dataQ = xQueueCreate(1, sizeof(MPUData_t));
    right_wheel_dataQ = xQueueCreate(1, sizeof(WheelVelocity_t));
    left_wheel_dataQ = xQueueCreate(1, sizeof(WheelVelocity_t));
    ultrasonic_dataQ = xQueueCreate(1, sizeof(Distances_t));

    /* Rest of tasks created inside `mpu_reset_task` */
    xTaskCreate(mpu_reset_task, "MPU Reset", 128, NULL, 4, NULL);

    vTaskStartScheduler();

    while(1);
}

static void board_init(void){
    sysclock_init();

    clock_afio();
    clock_gpioa();
    clock_gpiob();
    clock_gpioc();
    clock_adc1();
    clock_dma1();
    clock_i2c2();
    clock_tim1();
    clock_tim2();
    clock_tim3();
    clock_tim4();
    clock_usart3();

    configure_io();
    configure_i2c2();
    configure_tim1();
    configure_tim2();
    configure_tim3();
    configure_tim4();
    configure_usart3();
    
    configure_i2c2_dma();
    prepare_usart3_dma();
}
