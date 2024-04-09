/**
  *@file clocks_ese.c
  *@author Mitchell Brough
  *@brief Library concerned with clock control
  *@version 1.0
  *@date 2024-03-04
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#include "stm32f10x.h"
#include "../include/clocks_ese.h"

void sysclock_init(void){
    /* Enable HSE with Bypass and wait for it to be ready */
    RCC->CR |= RCC_CR_HSEON | RCC_CR_HSEBYP;
    while (((RCC->CR) & (RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_HSERDY)) == 0);

    /* Set HSE as SYSCLK and wait for it to be recognized */
    RCC->CFGR = RCC_CFGR_SW_HSE;
    while (((RCC->CFGR) & (RCC_CFGR_SW_HSE | RCC_CFGR_SWS_HSE)) == 0);

    /* Ensure PLL is disabled */
    RCC->CR &= ~RCC_CR_PLLON;

    /* PLLSRC = HSE (HSE = 8MHz, see Nucleo datasheet) */
    /* Sets CFGR register such that PLLMUL is 5 (8MHz * 5 = 40MHz) */
    /* Sets CFGR register such that AHB Prescaler is 2 (40MHz / 2 = 20MHz) */
    RCC->CFGR = 0x000D0080;

    /* Enable PLL and wait for it to be ready */
    RCC->CR |= RCC_CR_PLLON;
    while (((RCC->CR) & (RCC_CR_PLLON | RCC_CR_PLLRDY)) == 0);

    /* Set PLL as SYSCLK and wait for it to be ready */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while (((RCC->CFGR) & (RCC_CFGR_SW_PLL | RCC_CFGR_SWS_PLL)) == 0);
}

void clock_afio(void){
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
}

void clock_gpioa(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
}

void clock_gpiob(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
}

void clock_gpioc(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
}

void clock_adc1(void){
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
}

void clock_dma1(void){
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}

void clock_i2c2(void){
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
}

void clock_tim1(void){
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
}

void clock_tim2(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
}

void clock_tim3(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
}

void clock_tim4(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
}

void clock_usart3(void){
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
}
