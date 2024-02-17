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
	I2C2->CR2 |= 0x18; // 0x18 = 24MHz
	I2C2->CCR = 0x78;  // 100kHz 50% DC = 5us high/low = 120 cycles @ 24MHz
	I2C2->TRISE = 25;  // 1000ns / 41.67us = 24 + 1 = 25
    
    I2C2->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN; // Enable event interrupts
    //I2C2->CR2 |= I2C_CR2_ITERREN; // Enable bus error interrupt
    NVIC_EnableIRQ(I2C2_EV_IRQn);
    //NVIC_EnableIRQ(I2C2_ER_IRQn);
    
    I2C2->CR1 |= I2C_CR1_PE;
    I2C2->CR1 |= I2C_CR1_START;
}

void I2C2_EV_IRQHandler(void){
    if(I2C2->SR1 & I2C_SR1_SB){
        (void)I2C2->SR2;
        I2C2->DR = (0x57<<1);
    }
    
    else if(I2C2->SR1 & I2C_SR1_ADDR){
        (void)I2C2->SR2;
    }
    
    else if(I2C2->SR1 & I2C_SR1_TXE){
        I2C2->DR = 0x1;
    }
    
    else if(I2C2->SR1 & I2C_SR1_BTF){
        I2C2->CR1 |= I2C_CR1_STOP;
    }
}

void I2C2_ER_IRQHandler(void){
    if(I2C2->SR1 & I2C_SR1_ARLO){
        I2C2->CR1 |= I2C_CR1_START;
        I2C2->SR1 &= ~I2C_SR1_ARLO;
    }
}
