#include "stm32f10x.h"

#include "../include/clocks_ese.h"
#include "../include/gpio_ese.h"
#include "../include/i2c_ese.h"
#include "../include/timers_ese.h"

static void board_init(void);
static void delay(volatile uint32_t);

int main(void){
    
    board_init();
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
    configure_dma();
    configure_systick();
}

static void delay(volatile uint32_t time){
    while(time--);
}
