/**
  *@file main.c
  *@author Emily Schwab
  *@brief main file for the MCU used for manual controls
  *
  *@version 1.0
  *@date 2024-01-19
  *
  *@copyright Copyright (c) 2024 Emily Schwab
  *
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "../../project_types.h"
#include "../include/adc_ese.h"
#include "../include/clocks_ese.h"
#include "../include/display_ese.h"
#include "../include/gpio_ese.h"
#include "../include/interrupts_ese.h"
#include "../include/queues_ese.h"
#include "../include/tasks_ese.h"
#include "../include/timers_ese.h"
#include "../include/usart_ese.h"

static void board_init(void);

int main(void){
    board_init();
    
    speedQ = xQueueCreate(1, sizeof(ChairSpeed_t));
    
    xTaskCreate(print_speed_task,"Speed",128,NULL,1,&print_speed_handle);
    xTaskCreate(ultrasonic_data_task,"Ultra",128,NULL,1,&ultrasonic_handle);
    
    /* Peripherals enabled in USART3_IRQHandler */
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
    clock_tim2();
    clock_tim3();
    clock_tim4();
    clock_usart3();
    
    configure_io();
    configure_tim2();
    configure_tim3();   /* Starts motor driver signals, init to stop */
    configure_tim4();
    configure_adc1();
}
