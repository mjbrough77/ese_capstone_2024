#include "FreeRTOS.h"
#include "queue.h"

#include "../include/interrupts_ese.h"
#include "../include/i2c_ese.h"
#include "../include/queues_ese.h"

void DMA1_Channel4_IRQHandler(void){
    static uint8_t mpu_reset_index = 1;

    /* Initial Reset of MPU */
    if(mpu_reset_index < MPU_RESET_STEPS && DMA1->ISR & DMA_ISR_TCIF4){
        I2C2->CR2 &= ~I2C_CR2_DMAEN;
        DMA1_Channel4->CCR &= (uint16_t)0xFFFE; /* Disable Channel 4 DMA */
        DMA1_Channel4->CMAR = (uint32_t)mpu_init[mpu_reset_index++];
        DMA1_Channel4->CNDTR = MPU_SINGLE_WRITE;
        DMA1_Channel4->CCR |= DMA_CCR4_EN;
    }
    
    DMA1->IFCR |= DMA_IFCR_CTCIF4;
}

void DMA1_Channel5_IRQHandler(void){
    
}

void I2C2_EV_IRQHandler(void){
    volatile uint16_t status = I2C2->SR1;
    static uint8_t restart = 0; /* Restart required on MPU read */
    uint8_t queue_size = (uint8_t)uxQueueMessagesWaitingFromISR(i2c2Q);
    uint8_t address_to_send = 0;
    uint8_t register_to_send = 0;
    
    /* Full queue indicates user wants to read MPU */
    if(queue_size == MPU_READ_ADDRS){ 
        restart = 1;
        I2C2->CR2 |= I2C_CR2_ITBUFEN;
    }
    
    /* Interrupt will re-fire if queue is empty */
    if(status & I2C_SR1_SB){
        if(xQueueReceiveFromISR(i2c2Q, &address_to_send, NULL) == pdPASS)
            I2C2->DR = address_to_send;
    }

    /* Clear ADDR flag */
    else if(status & I2C_SR1_ADDR){
        if(restart != 1) I2C2->CR2 |= I2C_CR2_DMAEN;
        (void)I2C2->SR2;
    }

    /* End of transmission */
    else if(status & I2C_SR1_BTF){
        I2C2->CR1 |= I2C_CR1_STOP;
        I2C2->CR1 &= ~I2C_CR1_STOP;
    }
    
    /* Only runs when MPU FIFO register read */
    else if(status & I2C_SR1_TXE){
        if(xQueueReceiveFromISR(i2c2Q, &register_to_send, NULL) == pdPASS){
            I2C2->DR = register_to_send;
            restart = 0;
            I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
        }
    }
}

void EXTI9_5_IRQHandler(void){
    
}


