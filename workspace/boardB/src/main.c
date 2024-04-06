#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "../../project_types.h"
#include "../include/adc_ese.h"
#include "../include/clocks_ese.h"
#include "../include/gpio_ese.h"
#include "../include/i2c_ese.h"
#include "../include/queues_ese.h"
#include "../include/semaphore_ese.h"
#include "../include/tasks_ese.h"
#include "../include/timers_ese.h"
#include "../include/usart_ese.h"

static void board_init(void);
_Noreturn static void error_control_task(void*);

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

    /* Rest of tasks created inside `mpu_reset_task()` */
    xTaskCreate(mpu_reset_task,"MPU Reset",128,NULL,4,NULL);
    xTaskCreate(error_control_task, "Error",128,NULL,4,&system_error_handle);
    
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
    configure_adc1();
    configure_i2c2();
    configure_tim1();
    configure_tim2();
    configure_tim3();
    configure_tim4();
    configure_usart3();
    
    configure_i2c2_dma();
    prepare_usart3_dma();
}

_Noreturn static void error_control_task(void* param){
    uint32_t error_event = 0;
    uint32_t notify_value = 0;
    
    /* Only services one error at a time, this task preempts all other tasks */
    while(1){
        error_event = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        /* Error on I2C2 bus requires restart of peripheral */
        if(error_event & I2C2_ERR_NOTIFY){
            vTaskSuspend(eeprom_write_handle);
            vTaskSuspend(mpu_read_handle);
            I2C2->CR1 |= I2C_CR1_SWRST;
            xQueueReset(i2c2Q);
            I2C2->CR1 &= ~I2C_CR1_SWRST;
            configure_i2c2();
            vTaskResume(mpu_read_handle);
            vTaskResume(eeprom_write_handle);
        }
        
        /*
         * Because send_boardT is periodic, bits are set, NOT overwritten
         */
        if(error_event & WEIGHT_NOTIFY){
            notify_value |= ERROR_CTRL_WEIGHT_NOTIFY;
        }
        
        else if(error_event & MAXTILT_NOTIFY){
            notify_value |= ERROR_CTRL_TILT_NOTIFY;
        }
        
        else if(error_event & ERROR_CTRL_CLEAR_TILT){
            notify_value |= ERROR_CTRL_CLEAR_TILT;
        }
        
        else if(error_event & ERROR_CTRL_CLEAR_WEIGHT){
            notify_value |= ERROR_CTRL_CLEAR_WEIGHT;
        }
        
        xTaskNotify(send_boardT_handle, notify_value, eSetBits);
        notify_value = 0;
        
        (void)param;
    }
}
