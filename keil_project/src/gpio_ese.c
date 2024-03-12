/**
  *@file gpio_ese.h
  *@author Mitchell Brough
  *@brief Library concerning GPIO/AFIO pin configurations
  *@version 1.0
  *@date 2024-03-04
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
  *
 */

#include "../include/gpio_ese.h"

/**
  *@brief Uses GPIO, AFIO registers to configure IO pins
  *
  *@pre The GPIOx lines used in this function have their clocks enabled
 */
void configure_io(void){

    /**************************************************************************
     * LEFT ENCODER
    **************************************************************************/
    /********** Configure TIM2 CH1,2 for Encoder Capture **********/
    /** PA0 as Input floating (A-Phase) **/
    /* Default config on reset */

    /** PA1 as Input floating (B-Phase) **/
    /* Default config on reset */

    /********** Configure TIM1 CH1 for Input Capture **********/
    /** PA8 as Input floating (Z-Phase) **/
    /* Default config on reset */


    /**************************************************************************
     * RIGHT ENCODER
    **************************************************************************/
    /********** Configure TIM3 CH1,2 for Encoder Capture **********/
    /** PA6 as Input floating (A-Phase) **/
    /* Default config on reset */

    /** PA7 as Input floating (B-Phase) **/
    /* Default config on reset */

    /********** Configure TIM4 CH1 for Input Capture **********/
    /** PB6 as Input floating (Z-Phase) **/
    /* Default config on reset */


    /**************************************************************************
     * I2C2 BUS
    **************************************************************************/
    /** PB10 as Alternate Function Open Drain @ 2MHz (SCL) **/
    GPIOB->CRH |= GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10_1;

    /** PB11 as Alternate Function Open Drain @ 2MHz (SDA) **/
    GPIOB->CRH |= GPIO_CRH_CNF11_1 | GPIO_CRH_MODE11_1;


    /**************************************************************************
     * WEIGHT SENSOR
    **************************************************************************/
    /** PA2 as Analog Input (ADC1_IN2) **/
    GPIOA->CRL &= ~GPIO_CRL_CNF2 & ~GPIO_CRL_MODE2;


    /**************************************************************************
     * MPU6050 INTERRUPT PIN
    **************************************************************************/
    /** PC6 as Input floating (EXTI6 Interrupt) **/
    /* Default config on reset */
    AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI6_PC; /* EXTI6 source from PC6 */
    EXTI->RTSR |= EXTI_RTSR_TR6;              /* EXTI6 on falling edge */
    NVIC_SetPriority(EXTI9_5_IRQn, 5);        /* Prempted always */
    NVIC_EnableIRQ(EXTI9_5_IRQn);             /* Line 6 still needs unmasking */
}
