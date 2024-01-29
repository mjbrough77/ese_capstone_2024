#include "stm32f10x.h"

#include "../include/gpio_ese.h"

/**
  *@brief Uses GPIO, AFIO registers to configure IO pins
 */
void configure_io(void){
    /********** Configure TIM3 CH1,2 for Encoder Capture **********/
    /** PA6 as Input floating (TIM3 CH1) **/
    /* Default config on reset*/
        
    /** PA7 as Input floating (TIM3 CH2) **/
    /* Default config on reset */
    
    /********** Configure TIM4 CH1 for Input Capture **********/
    /** PB6 as Input floating (TIM4 CH1) **/
    /* Default config on reset */
}
