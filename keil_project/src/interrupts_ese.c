#include "../include/interrupts_ese.h"
#include "../include/i2c_ese.h"
#include "../include/mpu6050_driver.h"

static const RegReset mpu_init[9] = {
    {REG_PWR_MGMT_1, 0x80},
    {REG_SIGNAL_PATH_RESET, 0x07},
    {REG_PWR_MGMT_1, 0x01},
    {REG_SMPLRT_DIV, 0x07},
    {REG_CONFIG, 0x01},
    {REG_ACCEL_CONFIG, 0x00},
    {REG_FIFO_EN, 0x08},
    {REG_INT_ENABLE, 0x01},
    {REG_USER_CTRL, 0x40}
};

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
    static uint8_t startup = 1;
    static uint8_t index_mpu_init = 0;
    static uint8_t reg = 
    static uint8_t rw = 0;
    
    volatile uint16_t status = I2C2->SR1;
    
    if(startup){
        
    }
    
    if(status & I2C_SR1_SB){
        
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
