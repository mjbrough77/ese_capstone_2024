#include "../include/clocks_ese.h"
#include "../include/gpio_ese.h"
#include "../include/timers_ese.h"

static void board_init(void);

int main(void){
    
    board_init();
    
    while(1);
}

static void board_init(void){
    sysclock24_init();
    
    clock_afio();
    clock_gpioa();
    clock_tim3();
    
    configure_io();
    configure_tim3();
}
