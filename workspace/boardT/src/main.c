/**
  *@file main.c
  *@author Mitchell Brough
  *@brief Main executable for our boardT program
  *
  * Initializes all peripherals and starts FreeRTOS scheduler
  *
  *@version 1.0
  *@date 2024-04-09
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

/* Include all libraries in the program just for the sake of it */
#include "FreeRTOS.h"
#include "task.h"

#include "../../project_types.h"
#include "../include/adc_ese.h"
#include "../include/clocks_ese.h"
#include "../include/display_ese.h"
#include "../include/gpio_ese.h"
#include "../include/interrupts_ese.h"
#include "../include/motor_ese.h"
#include "../include/tasks_ese.h"
#include "../include/timers_ese.h"
#include "../include/usart_ese.h"

/**
  *@brief Configures all peripherals before starting up RTOS scheduler
  *
  * This function should be run first before anything else
 */
static void board_init(void);

int main(void){
    board_init();

    xTaskCreate(print_speed_task,"Speed",128,NULL,2,&display_handle);
    xTaskCreate(motor_control_task,"Motor",128,NULL,2,&motor_control_handle);
    xTaskCreate(ultrasonic_data_task,"Ultra",128,NULL,1,&ultrasonic_handle);

    vTaskStartScheduler();
	while(1);
}

static void board_init(void){
    sysclock_init();    /* 40MHz sysclock, 20MHz AHB clock */

    clock_afio();
    clock_gpioa();
    clock_gpiob();
    clock_gpioc();
    clock_adc1();
    clock_dma1();
    clock_tim2();
    clock_tim3();
    clock_tim4();
    clock_usart3();

    configure_io();
    configure_tim2();
    configure_tim3();
    configure_tim4();
    configure_adc1();
    configure_usart3();
    prepare_usart3_dma();

    /* Peripherals fully enabled in USART3_IRQHandler() */
}
