#include "../include/gpio_ese.h"

/**
  *@brief Uses GPIO, AFIO registers to configure IO pins
 */
void configure_io(void){
    /********** Configure TIM3 CH1,2 for Encoder Capture **********/
    /** PA6 as Input floating (A-Phase) **/
    /* Default config on reset */
        
    /** PA7 as Input floating (B-Phase) **/
    /* Default config on reset */
    
    /********** Configure TIM4 CH1 for Input Capture **********/
    /** PB6 as Input floating (Z-Phase) **/
    /* Default config on reset */
    
    /********** Configure I2C2 **********/
    /** PB10 as Alternate Function Open Drain @ 2MHz (SCL) **/
    GPIOB->CRH |= GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10_1;
    
    /** PB11 as Alternate Function Open Drain @ 2MHz (SDA) **/
    GPIOB->CRH |= GPIO_CRH_CNF11_1 | GPIO_CRH_MODE11_1;
    
    /********** Configure MPU interrupt **********/
    /** PC6 as Input floating (MPU INT) **/
    /* Default config on reset */
    AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI6_PC; /* EXTI6 source from PC6 */
    EXTI->IMR |= EXTI_IMR_MR6;                /* Unmask EXTI6 */
    EXTI->RTSR |= EXTI_RTSR_TR6;              /* EXTI6 on falling edge */
    NVIC_SetPriority(EXTI9_5_IRQn, 14);       /* Prempted always */
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}
