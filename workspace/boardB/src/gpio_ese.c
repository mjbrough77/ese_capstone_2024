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

#include "stm32f10x.h"
#include "../include/gpio_ese.h"

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
    /** PC0 as Analog Input (ADC1_IN10) **/
    GPIOC->CRL &= ~GPIO_CRL_CNF0 & ~GPIO_CRL_MODE0;


    /**************************************************************************
     * MPU6050 INTERRUPT PIN
    **************************************************************************/
    /** PC6 as Input floating (EXTI6 Interrupt) **/
    /* Default config on reset */
    AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI6_PC; /* EXTI6 source from PC6 */
    EXTI->RTSR |= EXTI_RTSR_TR6;              /* EXTI6 on falling edge */
    NVIC_SetPriority(EXTI9_5_IRQn, 7);
    NVIC_EnableIRQ(EXTI9_5_IRQn);             /* Line 6 still needs unmasking */


    /**************************************************************************
     * USART (INTER-MCU COMMUNICATIONS)
    **************************************************************************/
    /********** Configure USART3 **********/
    /* Remap PC10 -> Tx, PC11 -> Rx, PC12 -> CK */
    AFIO->MAPR |= AFIO_MAPR_USART3_REMAP_PARTIALREMAP;

    /** PC10 as AFO push-pull @ 2MHz (USART3_Tx) */
    GPIOC->CRH |= GPIO_CRH_MODE10_1 | GPIO_CRH_CNF10_1;
    GPIOC->CRH &= ~GPIO_CRH_CNF10_0;

    /** PC11 as Input w/pull-up (USART3_Rx) */
    /* Default config on reset */
}
