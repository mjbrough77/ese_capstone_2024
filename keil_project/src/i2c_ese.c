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
    I2C2->CR1 |= I2C_CR1_START; // I2C Start message (should wait for !busy)
}

void I2C2_EV_IRQHandler(void){
    static uint8_t transmit = 1;
    static uint8_t rw = 0;
    static uint8_t data_left = 2;
    
    volatile uint16_t status = I2C2->SR1;
    
    switch(status){
        case 1:                         // SB = 1
            transmit = 1;
            I2C2->DR = (0x57<<1) + rw;
            break;
        
        case 2:                         // ADDR = 1
            (void)I2C2->SR2;
            break;
        
        case 64:                        // RxNE = 1
            if(data_left == 2){
                I2C2->CR1 |= I2C_CR1_ACK;
                data_left = 1;
            }
            else if(data_left == 0){
                (void)I2C2->DR;
                data_left = 2;
            }
            break;
            
        case 68:                        // RxNE = 1, BTF = 1
            data_left = 0;
            I2C2->CR1 &= ~I2C_CR1_ACK;
            (void)I2C2->DR;
            I2C2->CR1 |= I2C_CR1_STOP;
            (void)I2C2->DR;
            break;
        
        case 128:                       // TxNE = 1
            if(transmit != 1) break;
            I2C2->DR = 0x1;        
            transmit = 0;
            break;
        
        case 132:                       // TxNE = 1, BTF = 1
            I2C2->CR1 |= I2C_CR1_STOP;
            rw ^= 1;
            break;
    }
}
