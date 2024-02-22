/*
 * FILENAME: i2c_lib.c
 *
 * DESCRIPTION: Library concerned with I2C
 *
 * AUTHOR: Mitchell Brough, 200239781
*/

#include "../include/i2c_ese.h"

static uint8_t const write = 1;
static uint8_t readings[12];

void configure_i2c2(void){
    I2C2->CR2 |= 20;  // 20MHz
    I2C2->CCR |= I2C_CCR_FS | I2C_CCR_DUTY | 2; // 400kHz Fm with 16/9 DC  
    I2C2->TRISE = 7; // 300ns / 50ns = 6 + 1 = 7
    
    I2C2->CR2 |= I2C_CR2_ITEVTEN;
    NVIC_EnableIRQ(I2C2_EV_IRQn);
    NVIC_EnableIRQ(I2C2_ER_IRQn);
    
    I2C2->CR1 |= I2C_CR1_PE;
    I2C2->CR1 |= I2C_CR1_ACK;
    I2C2->CR2 |= I2C_CR2_LAST;
    I2C2->CR2 |= I2C_CR2_DMAEN;
}

void configure_dma(void){
    // I2C2_Tx DMA channel
    DMA1_Channel4->CPAR = (uint32_t)&I2C2->DR;
    DMA1_Channel4->CMAR = (uint32_t)&write;
    DMA1_Channel4->CNDTR = 1;
    DMA1_Channel4->CCR |= DMA_CCR4_TCIE | DMA_CCR4_DIR | DMA_CCR4_CIRC;
    DMA1_Channel4->CCR |= DMA_CCR4_EN;
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    
    // I2C2_Rx DMA channel
    DMA1_Channel5->CPAR = (uint32_t)&I2C2->DR;
    DMA1_Channel5->CMAR = (uint32_t)readings;
    DMA1_Channel5->CNDTR = 12;
    DMA1_Channel5->CCR |= DMA_CCR5_TCIE | DMA_CCR5_CIRC | DMA_CCR5_MINC;
    DMA1_Channel5->CCR |= DMA_CCR5_EN;
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}
