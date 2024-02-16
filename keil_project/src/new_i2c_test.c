#include "stm32f10x.h"

void configure_i2c2(void){
	I2C2->CR2 |= 0x18; // 0x18 = 24MHz
	I2C2->CCR = 0x78;  // 100kHz 50% DC = 5us high/low = 120 cycles @ 24MHz
	I2C2->TRISE = 25;  // 1000ns / 41.67us = 24 + 1 = 25
    
    I2C2->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN; // Enable event interrupts
    NVIC_EnableIRQ(I2C2_EV_IRQn);
}

void I2C2_EV_IRQHandler(void){
    
    if(I2C2->SR1 & I2C_SR1_SB){
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
