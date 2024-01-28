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
       
    /** PB0 as Input floating (TIM3 CH3) **/
    /* Default config on reset */
     
//    /********** Configure USART2 IO pins Tx and Rx **********/
//    /** PA2 as AFO push-pull @ 50MHz (Tx) */
//    GPIOA->CRL &= ~GPIO_CRL_MODE2_0;
//    GPIOA->CRL |= GPIO_CRL_CNF2 | GPIO_CRL_MODE2_1;

//     /** PA3 as pull-up/pull-down input (Rx) */
//    GPIOA->CRL &= ~GPIO_CRL_MODE3 & ~GPIO_CRL_CNF3_0;
//    GPIOA->CRL |= GPIO_CRL_CNF3_1;
}
