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

#include "../include/adc_ese.h"
#include "../include/clocks_ese.h"
#include "../include/display_ese.h"
#include "../include/gpio_ese.h"
#include "../include/timers_ese.h"

static void board_init(void);

int main(void){
    board_init();
	
	while(1);
}

static void board_init(void){
    sysclock_init();    /* 40MHz sysclock, 20MHz AHB clock */
    
    clock_afio();
    clock_gpioa();
    clock_gpiob();
    clock_gpioc();
    clock_adc1();
    clock_tim3();
    clock_usart3();
    
    configure_io();
    configure_tim3();
    configure_adc1();
    
    enable_tim3IRQ();
}
