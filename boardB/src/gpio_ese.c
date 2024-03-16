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
     * SEVEN SEGMENT DISPLAY 1
    **************************************************************************/
    /** PB5 as GPO Push-Pull @ 2MHz (D0-1) **/
    GPIOB->CRL |= GPIO_CRL_MODE5_1;
    GPIOB->CRL &= ~GPIO_CRL_CNF5;

    /** PB4 as GPO Push-Pull @ 2MHz (D1-1) WITH REMAP **/
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_NOJNTRST;
    GPIOB->CRL |= GPIO_CRL_MODE4_1;
    GPIOB->CRL &= ~GPIO_CRL_CNF4 & ~GPIO_CRL_MODE4_0;

    /** PB10 as GPO Push-Pull @ 2MHz (D2-1) **/
    GPIOB->CRH |= GPIO_CRH_MODE10_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF10;

    /** PA9 as GPO Push-Pull @ 2MHz (D3-1) **/
    GPIOA->CRH |= GPIO_CRH_MODE9_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF9;
    
    /** PC2 as GPO Push-Pull @ 2MHz (Decimal Point) **/
    GPIOC->CRL |= GPIO_CRL_MODE2_1;
    GPIOC->CRL &= ~GPIO_CRL_CNF2;
    GPIOC->BSRR |= GPIO_BRR_BR2; /* MUST BE LOW TO PREVENT OVER CURRENT */


    /**************************************************************************
     * SEVEN SEGMENT DISPLAY 2
    **************************************************************************/
    /** PA4 as GPO Push-Pull @ 2MHz (D0-2) **/
    GPIOA->CRL |= GPIO_CRL_MODE4_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF4;

    /** PA10 as GPO Push-Pull @ 2MHz (D1-2) **/
    GPIOA->CRH |= GPIO_CRH_MODE10_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF10;

    /** PB9 as GPO Push-Pull @ 2MHz (D2-2) **/
    GPIOB->CRH |= GPIO_CRH_MODE9_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF9;

    /** PB8 as GPO Push-Pull @ 2MHz (D3-2) **/
    GPIOB->CRH |= GPIO_CRH_MODE8_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF8;


    /**************************************************************************
     * SEVEN SEGMENT DISPLAY BLANKING
    **************************************************************************/
    /** PA5 as GPO Push-Pull @ 2MHz (Blanking) **/
    GPIOA->CRL |= GPIO_CRL_MODE5_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF5;
    

    /**************************************************************************
     * TWO POSITION SWITCH
    **************************************************************************/
    /** PC7 as Input with pull-up/pull-down **/
    GPIOC->CRL |= GPIO_CRL_CNF7_1;
    GPIOC->CRL &= ~GPIO_CRL_CNF7_0;


    /**************************************************************************
     * JOYSTICK
    **************************************************************************/
    /** PC0 as Analog Input (VRX, ADC1_IN10) **/
    GPIOC->CRL &= ~GPIO_CRL_CNF0;

    /** PC1 as Analog Input (VRY, ADC1_IN11) **/
    GPIOC->CRL &= ~GPIO_CRL_CNF1;

    /** PB0 as Floating Input (SW) */
    /* Default config on reset */


    /**************************************************************************
     * MOTOR DRIVER
    **************************************************************************/
    /********** Configure TIM3 CH1,2 for PWM Output **********/
    /** PA6 as AFO push-pull @ 2MHz (S2, Ch1) **/
    GPIOA->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_CNF6_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF6_0;

    /** PA7 as AFO push-pull @ 2MHz (S1, Ch2) **/
    GPIOA->CRL |= GPIO_CRL_MODE7_1 | GPIO_CRL_CNF7_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF7_0;


    /**************************************************************************
     * ULTRASONIC SENSORS
    **************************************************************************/
    /********** Configure TIM2 CH2 for PWM Output **********/
    /** PA1 as AFO push-pull @ 2MHz (TRIG) **/
    GPIOA->CRL |= GPIO_CRL_MODE1_1 | GPIO_CRL_CNF1_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF1_0;

    /********** Configure TIM4 CH1,2 for Input Capture **********/
    /** PB6 as Floating Input (RIGHT SENSOR ECHO) */
    /* Default config on reset */
    
    /** PB7 as Floating Input (LEFT SENSOR ECHO) */
    /* Default config on reset */
    
    
    /**************************************************************************
     * USART (INTER-MCU COMMUNICATIONS)
    **************************************************************************/
    /********** Configure USART3 **********/
    /* Remap PC10 -> Tx, PC11 -> Rx, PC12 -> CK */
    AFIO->MAPR |= AFIO_MAPR_USART3_REMAP_PARTIALREMAP;
    
    /** PC10 as AFO push-pull @ 2MHz (USART3_Tx) */
    GPIOC->CRH |= GPIO_CRH_MODE10_1 | GPIO_CRH_CNF10_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF10_0;
    
    /** PB6 as Floating Input (USART3_Rx) */
    /* Default config on reset */
}
