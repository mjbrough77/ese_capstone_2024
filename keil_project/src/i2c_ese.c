/*
 * FILENAME: i2c_lib.c
 *
 * DESCRIPTION: Library concerned with I2C
 *
 * AUTHOR: Mitchell Brough, 200239781
*/

#include "../include/i2c_ese.h"

const uint8_t mpu_init[MPU_RESET_STEPS][MPU_SINGLE_WRITE] = {
    {ADDR_MPU, REG_PWR_MGMT_1, 0x80},         /* Device Reset */
    {ADDR_MPU, REG_SIGNAL_PATH_RESET, 0x07},  /* Sensor Reset */
    {ADDR_MPU, REG_PWR_MGMT_1, 0x01},         /* PLL with x-axis gyro ref */
    {ADDR_MPU, REG_USER_CTRL, 0x04},          /* Reset FIFO */
    {ADDR_MPU, REG_SMPLRT_DIV, 0x07},         /* Sample rate = 1kHz */
    {ADDR_MPU, REG_CONFIG, 0x01},             /* DLPF setting 1 */
    {ADDR_MPU, REG_ACCEL_CONFIG, 0x00},       /* Accelerometer +-2g */
    {ADDR_MPU, REG_FIFO_EN, 0x78},            /* 3-axis gyro into FIFO */
    {ADDR_MPU, REG_INT_ENABLE, 0x01},         /* Data ready interrupt enable */
    {ADDR_MPU, REG_USER_CTRL, 0x40}           /* Enable FIFO */
};

void configure_i2c2(void){
    I2C2->CR2 |= 20;  // 20MHz
    I2C2->CCR |= I2C_CCR_FS | I2C_CCR_DUTY | 2; // 400kHz Fm with 16/9 DC
    I2C2->TRISE = 7; // 300ns / 50ns = 6 + 1 = 7
    I2C2->CR2 |= I2C_CR2_ITEVTEN; /* Event interrupt enable */
    
    NVIC_SetPriority(I2C2_EV_IRQn, 5); 
    NVIC_EnableIRQ(I2C2_EV_IRQn);

    I2C2->CR1 |= I2C_CR1_PE;
    I2C2->CR1 |= I2C_CR1_ACK;
    I2C2->CR2 |= I2C_CR2_LAST;
    I2C2->CR2 |= I2C_CR2_DMAEN;
}

void configure_dma(void){
    /* I2C2_Tx DMA channel */
    DMA1_Channel4->CPAR = (uint32_t)&I2C2->DR;
    DMA1_Channel4->CMAR = (uint32_t)mpu_init[0];
    DMA1_Channel4->CNDTR = MPU_SINGLE_WRITE;
    DMA1_Channel4->CCR |= DMA_CCR4_TCIE | DMA_CCR4_DIR;
    DMA1_Channel4->CCR |= DMA_CCR4_EN;
    
    NVIC_SetPriority(DMA1_Channel4_IRQn, 5);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

//    // I2C2_Rx DMA channel
//    DMA1_Channel5->CPAR = (uint32_t)&I2C2->DR;
//    DMA1_Channel5->CMAR = (uint32_t)readings;
//    DMA1_Channel5->CNDTR = 12;
//    DMA1_Channel5->CCR |= DMA_CCR5_TCIE | DMA_CCR5_CIRC | DMA_CCR5_MINC;
//    DMA1_Channel5->CCR |= DMA_CCR5_EN;
//    
//    NVIC_SetPriority(DMA1_Channel5_IRQn, 5);
//    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}
