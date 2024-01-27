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
        
        /********** Configure TIM3 CH1,2,3 for Input Compare **********/
        /** PA6 as Input with Pull-up/Pull-down (TIM3 CH1) **/
        GPIOA->CRL &= ~GPIO_CRL_MODE6_0 & ~GPIO_CRL_CNF6;
        GPIOA->CRL |= GPIO_CRL_MODE6_1;
        
        /** PA7 as Input with Pull-up/Pull-down (TIM3 CH2) **/
        GPIOA->CRL &= ~GPIO_CRL_MODE7_0 & ~GPIO_CRL_CNF7;
        GPIOA->CRL |= GPIO_CRL_MODE7_1;
        
        /** PB0 as Input with Pull-up/Pull-down (TIM3 CH3) **/
        GPIOB->CRH &= ~GPIO_CRL_MODE0_0 & ~GPIO_CRL_CNF0;
        GPIOB->CRL |= GPIO_CRL_MODE0_1;
        
        /********** Configure USART2 IO pins Tx and Rx **********/
        /** PA2 as AFO push-pull @ 50MHz (Tx) */
        GPIOA->CRL &= ~GPIO_CRL_MODE2_0;
        GPIOA->CRL |= GPIO_CRL_CNF2 | GPIO_CRL_MODE2_1;

         /** PA3 as pull-up/pull-down input (Rx) */
        GPIOA->CRL &= ~GPIO_CRL_MODE3 & ~GPIO_CRL_CNF3_0;
        GPIOA->CRL |= GPIO_CRL_CNF3_1;
}
