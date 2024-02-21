/*
 * FILENAME: i2c_lib.c
 *
 * DESCRIPTION: Library concerned with I2C
 *
 * AUTHOR: Mitchell Brough, 200239781
*/

#include "stm32f10x.h"

#include "../include/i2c_ese.h"

static uint8_t const write = 1;
static uint8_t distance[3];

void configure_dma(void){
//    // I2C2_Tx DMA channel
//    DMA1_Channel4->CPAR = (uint32_t)&I2C2->DR;
//    DMA1_Channel4->CMAR = (uint32_t)&write;
//    DMA1_Channel4->CNDTR = 1;
//    DMA1_Channel4->CCR |= DMA_CCR4_TCIE | DMA_CCR4_DIR | DMA_CCR4_CIRC;
//    DMA1_Channel4->CCR |= DMA_CCR4_EN;
//    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    
    // I2C2_Rx DMA channel
    DMA1_Channel5->CPAR = (uint32_t)&I2C2->DR;
    DMA1_Channel5->CMAR = (uint32_t)distance;
    DMA1_Channel5->CNDTR = 3;
    DMA1_Channel5->CCR |= DMA_CCR5_TCIE | DMA_CCR5_CIRC | DMA_CCR5_MINC;
    DMA1_Channel5->CCR |= DMA_CCR5_EN;
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

void configure_i2c2(void){
    I2C2->CR2 |= 24;  // 24MHz
    I2C2->CCR |= 120; // 100kHz 50% DC = 5us high/low = 120 cycles @ 24MHz
    I2C2->TRISE = 25; // 1000ns / 41.67us = 24 + 1 = 25
    
    I2C2->CR2 |= I2C_CR2_ITEVTEN; // Enable event interrupts
    NVIC_EnableIRQ(I2C2_EV_IRQn);
    NVIC_EnableIRQ(I2C2_ER_IRQn);
    
    I2C2->CR1 |= I2C_CR1_PE;
    I2C2->CR1 |= I2C_CR1_ACK;
    I2C2->CR2 |= I2C_CR2_LAST;
    I2C2->CR2 |= I2C_CR2_DMAEN;
}

void DMA1_Channel4_IRQHandler(void){
    if(DMA1->ISR & DMA_ISR_TCIF4){
        I2C2->CR2 &= ~I2C_CR2_DMAEN;
        DMA1->IFCR |= DMA_IFCR_CTCIF4;
    }
}

void DMA1_Channel5_IRQHandler(void){
    if(DMA1->ISR & DMA_ISR_TCIF5){
        I2C2->CR1 |= I2C_CR1_STOP;
        DMA1->IFCR |= DMA_IFCR_CTCIF5;
    }
}

void I2C2_EV_IRQHandler(void){
    static uint8_t rw = 0;
    volatile uint16_t status = I2C2->SR1;
    
    if(status & I2C_SR1_SB){
        I2C2->DR = (0x57<<1) + rw;
        rw ^= 1;
    }
    
    else if(status & I2C_SR1_ADDR){
        (void)I2C2->SR2;
        I2C2->DR = 1;
    }
    
    else if(status & I2C_SR1_BTF){
        I2C2->CR1 |= I2C_CR1_STOP;
    }
    
    if(status & I2C_SR1_STOPF){
        I2C2->CR1 |= I2C_CR1_SWRST;
        I2C2->CR1 &= ~I2C_CR1_SWRST;
        configure_i2c2();
    }
}

void I2C2_ER_IRQHandler(void){
    
}
