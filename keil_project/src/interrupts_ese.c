#include "../include/interrupts_ese.h"
#include "../include/i2c_ese.h"

void DMA1_Channel4_IRQHandler(void){
    static uint8_t index = 1;
    
    /* Initial Reset of MPU */
    if(index < 9 && DMA1->ISR & DMA_ISR_TCIF4){
        I2C2->CR2 &= ~I2C_CR2_DMAEN;
        DMA1_Channel4->CCR &= 0xFFFE; /* Disable Channel 4 DMA */
        DMA1_Channel4->CMAR = (uint32_t)mpu_init[index++];
        DMA1_Channel4->CCR |= DMA_CCR4_EN;
    }
    
    DMA1->IFCR |= DMA_IFCR_CTCIF4;
}

void DMA1_Channel5_IRQHandler(void){
    if(DMA1->ISR & DMA_ISR_TCIF5){
        I2C2->CR1 |= I2C_CR1_STOP;
        DMA1->IFCR |= DMA_IFCR_CTCIF5;
    }
}

void I2C2_EV_IRQHandler(void){  
    volatile uint16_t status = I2C2->SR1;
    
    if(status & I2C_SR1_SB);
    
    else if(status & I2C_SR1_ADDR){
        (void)I2C2->SR2;
    }
    
    else if(status & I2C_SR1_BTF){
        I2C2->CR1 |= I2C_CR1_STOP;
    }
    
    else if(status & I2C_SR1_STOPF){
        I2C2->CR1 |= I2C_CR1_SWRST;
        I2C2->CR1 &= ~I2C_CR1_SWRST;
        configure_i2c2();
    }
}

void I2C2_ER_IRQHandler(void){
    
}
