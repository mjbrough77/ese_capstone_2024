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
    
    /********** Configure I2C2 **********/
    /** PB10 as Alternate Function Open Drain @ 2MHz **/
    GPIOB->CRH |= GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10_1;
    
    /** PB11 as Alternate Function Open Drain @ 2MHz **/
    GPIOB->CRH |= GPIO_CRH_CNF11_1 | GPIO_CRH_MODE11_1;
    
    /********** Configure MPU interrupt **********/
    /** PC6 as Input floating (MPU INT) **/
    /* Default config on reset */
}
