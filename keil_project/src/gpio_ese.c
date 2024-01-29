/**
  *@file gpio_enel452.c
  *@author Mitchell Brough 200239781
  *@brief Library realted to gpio configuration and operations
  *
  * ALL FUNCTIONS assume that the appropirate clocks are set before being
  * called
  *
  *@date 2023-10-26
 */

#include "stm32f10x.h"

#include "../include/gpio_ese.h"

/**
  *@brief Uses GPIO, AFIO registers to configure IO pins
 */
void configure_io(void){
        
    /********** Configure TIM3 CH1,2,3 for Input Capture **********/
    /** PA6 as Input floating (TIM3 CH1) **/
    /* Default config on reset*/
        
    /** PA7 as Input floating (TIM3 CH2) **/
    /* Default config on reset */
       
    /** PB6 as Input floating (TIM4 CH1) **/
    /* Default config on reset */
}
