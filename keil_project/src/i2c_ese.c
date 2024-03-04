#include "../include/i2c_ese.h"

/**************************************************************************
 * MPU RESET ARRAY (GLOBAL CONST, SAFE TO ACCESS, EASY TO IMPLEMENT)
**************************************************************************/
const uint8_t mpu_init[MPU_RESET_STEPS][MPU_SINGLE_WRITE] = {
    {REG_PWR_MGMT_1, 0x80},         /* Device Reset */
    {REG_SIGNAL_PATH_RESET, 0x07},  /* Sensor Reset */
    {REG_PWR_MGMT_1, 0x01},         /* PLL with x-axis gyro ref */
    {REG_USER_CTRL, 0x04},          /* Reset FIFO */
    {REG_SMPLRT_DIV, 0x07},         /* Sample rate = 125Hz (ready every 8ms) */
    {REG_CONFIG, 0x01},             /* DLPF setting 1 */
    {REG_GYRO_CONFIG, 0x08},        /* Gyro range +-500 degrees/s */
    {REG_FIFO_EN, 0x70},            /* 3-axis gyro into FIFO */
    {REG_USER_CTRL, 0x40},          /* Enable FIFO */
    {REG_INT_ENABLE, 0x01}          /* Data ready interrupt enable */
};


/**************************************************************************
 * CONFIGURATION FUNCTIONS
**************************************************************************/
void configure_i2c2(void){
    I2C2->CR2 |= 20;                            /* 20MHz */
    I2C2->CCR |= I2C_CCR_FS | I2C_CCR_DUTY | 2; /* 400kHz Fm with 16/9 DC */
    I2C2->TRISE = 7;                            /* 300ns / 50ns = 6 + 1 = 7 */
    I2C2->CR2 |= I2C_CR2_ITEVTEN;               /* Event interrupt enable */

    NVIC_SetPriority(I2C2_EV_IRQn, 5);
    NVIC_EnableIRQ(I2C2_EV_IRQn);

    I2C2->CR1 |= I2C_CR1_PE;    /* Peripheral Enable */
    I2C2->CR1 |= I2C_CR1_ACK;   /* Acknowledge on Rx */
    I2C2->CR2 |= I2C_CR2_LAST;  /* NACK on next DMA EOT */
}

void configure_i2c2_dma(void){
    /* I2C2_Tx DMA channel, configured to reset MPU6050 */
    DMA1_Channel4->CPAR = (uint32_t)&I2C2->DR;
    DMA1_Channel4->CMAR = (uint32_t)mpu_init[0];
    DMA1_Channel4->CNDTR = MPU_SINGLE_WRITE;
    DMA1_Channel4->CCR |= DMA_CCR4_TCIE | DMA_CCR4_DIR | DMA_CCR4_MINC;
    DMA1_Channel4->CCR |= DMA_CCR4_EN;

    NVIC_SetPriority(DMA1_Channel4_IRQn, 5); /* ISR Priority >= 5 (FreeRTOS) */
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    /* I2C2_Rx DMA channel, configured to read MPU6050 */
    DMA1_Channel5->CPAR = (uint32_t)&I2C2->DR;
    DMA1_Channel5->CNDTR = MPU_FIFO_READ;
    DMA1_Channel5->CCR |= DMA_CCR5_TCIE | DMA_CCR5_MINC | DMA_CCR5_CIRC;

    NVIC_SetPriority(DMA1_Channel5_IRQn, 5);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}


void update_log_dma(LogData_t *log){
    DMA1_Channel4->CCR &= (uint16_t)0xFFFE; /* Disable Channel 4 DMA */
    DMA1_Channel4->CMAR = (uint32_t)log;
    DMA1_Channel4->CCR |= DMA_CCR4_EN;
}
