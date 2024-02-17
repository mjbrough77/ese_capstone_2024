#include "stm32f10x.h"

#include "../include/clocks_ese.h"

/**
  *@brief Configures system clock to 24MHz
 */
void sysclock24_init(void){
    /** Enable HSI and wait for it to be ready */
    RCC->CR |= RCC_CR_HSION;
    while (((RCC->CR) & (RCC_CR_HSION | RCC_CR_HSIRDY)) == 0);
    
    /** Enable HSE with Bypass and wait for it to be ready */
    RCC->CR |= RCC_CR_HSEON | RCC_CR_HSEBYP;
    while (((RCC->CR)&(RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_HSERDY)) == 0);
    
    /** Set HSE as SYSCLK and wait for it to be recognized */
    RCC->CFGR = RCC_CFGR_SW_HSE;
    while (((RCC->CFGR) & (RCC_CFGR_SW_HSE | RCC_CFGR_SWS_HSE)) == 0);

    /**Ensure PLL is disabled */
    RCC->CR &= ~RCC_CR_PLLON;
    
    /** Sets CFGR register such that PLLMUL is 3 (8*3 = 24MHz) */
    RCC->CFGR = 0x00050000;

    /** Enable PLL and wait for it to be ready */
    RCC->CR |= RCC_CR_PLLON;
    while (((RCC->CR) & (RCC_CR_PLLON | RCC_CR_PLLRDY)) == 0);
    
    /** Set PLL as SYSCLK and wait for it to be ready */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while (((RCC->CFGR) & (RCC_CFGR_SW_PLL | RCC_CFGR_SWS_PLL)) == 0);    
}


/******************* Functions to enable related clocks *******************/
void clock_gpioa(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
}

void clock_gpiob(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
}

void clock_gpioc(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
}

void clock_i2c2(void){
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
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

void clock_usart2(void){
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
}

void clock_afio(void){
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
}
