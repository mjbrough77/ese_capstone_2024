/*
 * FILENAME: i2c_lib.c
 *
 * DESCRIPTION: Library concerned with I2C
 *
 * AUTHOR: Mitchell Brough, 200239781
*/

#include "stm32f10x.h"

#include "../include/i2c_ese.h"

void configure_i2c2(void){
    I2C2->CR2 |= 24;  // 24MHz
    I2C2->CCR |= 120; // 100kHz 50% DC = 5us high/low = 120 cycles @ 24MHz
    I2C2->TRISE = 25; // 1000ns / 41.67us = 24 + 1 = 25
    
    I2C2->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN; // Enable event interrupts
    NVIC_EnableIRQ(I2C2_EV_IRQn);
    
    I2C2->CR1 |= I2C_CR1_PE;
}

void I2C2_EV_IRQHandler(void){
    static uint8_t transmit = 1;
    static uint8_t rw = 0;
    static uint8_t data_left = 2;
    
    volatile uint16_t status = I2C2->SR1;
    
    if(status & I2C_SR1_SB){
        transmit = 1;
        I2C2->DR = (0x57<<1) + rw;
        (void)I2C2->SR2;
        if(rw == 1)
            (void)1;
    }
    
    else if(status & I2C_SR1_ADDR){
        (void)I2C2->SR2;
    }
    
    if(status & I2C_SR1_TXE && transmit == 1){
        I2C2->DR = 0x1;        
        transmit = 0;
    }
    
    else if(status & I2C_SR1_RXNE){
        if(data_left == 2){
            I2C2->CR1 |= I2C_CR1_ACK;
            data_left = 1;
        }
        
        else if(data_left == 1 && status & I2C_SR1_BTF){
            data_left = 0;
            I2C2->CR1 &= ~I2C_CR1_ACK;
            (void)I2C2->DR;
            I2C2->CR1 |= I2C_CR1_STOP;
            (void)I2C2->DR;
        }
        
        else{
            (void)I2C2->DR;
        }
    }
    
    else if(status & I2C_SR1_BTF){
        (void)I2C2->SR1;
        I2C2->CR1 |= I2C_CR1_STOP;
        rw ^= 1;
        data_left = 2;
    }
}
