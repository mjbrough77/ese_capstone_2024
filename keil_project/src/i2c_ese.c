/*
 * FILENAME: i2c_lib.c
 *
 * DESCRIPTION: Library concerned with I2C
 *
 * AUTHOR: Mitchell Brough, 200239781
*/

#include "../include/i2c_ese.h"

const uint8_t mpu_init[MPU_RESET_STEPS][2] = {
    {REG_PWR_MGMT_1, 0x80},             /* Device Reset */
    {REG_SIGNAL_PATH_RESET, 0x07},      /* Sensor Reset */
    {REG_PWR_MGMT_1, 0x01},             /* Clock PLL with x-axis gyro ref */
    {REG_USER_CTRL, 0x04},              /* Reset FIFO */
    {REG_SMPLRT_DIV, 0x07},             /* Reset FIFO */
    {REG_CONFIG, 0x01},                 /* Device Reset */
    {REG_ACCEL_CONFIG, 0x00},           /* Device Reset */
    {REG_FIFO_EN, 0x08},                /* Device Reset */
    {REG_INT_ENABLE, 0x01},             /* Device Reset */
    {REG_USER_CTRL, 0x40}               /* Device Reset */
};

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
    DMA1_Channel4->CMAR = (uint32_t)mpu_init[0];
    DMA1_Channel4->CNDTR = 2;
    DMA1_Channel4->CCR |= DMA_CCR4_TCIE | DMA_CCR4_DIR;
    DMA1_Channel4->CCR |= DMA_CCR4_EN;
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    
//    // I2C2_Rx DMA channel
//    DMA1_Channel5->CPAR = (uint32_t)&I2C2->DR;
//    DMA1_Channel5->CMAR = (uint32_t)readings;
//    DMA1_Channel5->CNDTR = 12;
//    DMA1_Channel5->CCR |= DMA_CCR5_TCIE | DMA_CCR5_CIRC | DMA_CCR5_MINC;
//    DMA1_Channel5->CCR |= DMA_CCR5_EN;
//    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}
