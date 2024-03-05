/**
  *@file interrupts_ese.c
  *@author Mitchell Brough
  *@brief Contains all interrupt handler definitions and buffer for MPU6050 data
  *
  *@version 1.0
  *@date 2024-03-03
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
  *
 */

#include "FreeRTOS.h"
#include "queue.h"

#include "../include/interrupts_ese.h"
#include "../include/i2c_ese.h"
#include "../include/queues_ese.h"
#include "../include/tasks_ese.h"

/**
  *@brief Buffer for storing MPU Data.
  * ONLY ACCSSESED BY `DMA1_Channel4_IRQHandler` and `DMA1_Channel5_IRQHandler`.
  *
  * We do not risk reentrancy because the I2C2 bus may only ever be sending
  * or recieving, not both
  *
 */
static uint8_t mpu_data[MPU_FIFO_READ];

/**
  *@brief Updates I2C2 DMA channel requests during and after initialization.
  * This interrupt triggers every EOT for I2C2_Tx.
  *
  * During initialization, this interrupt will step through the initialization
  * of the MPU6050 as defined in mpu_init (see i2c_ese.h).
  *
  * After initialization, I2C2_Tx DMA channel is reconfigured to write to the
  * EEPROM. I2C2_Rx is also updated, now placing data in the local global
  * buffer `mpu_data`
  *
 */
void DMA1_Channel4_IRQHandler(void){
    static uint8_t mpu_reset_index = 1;
    static uint8_t reconfigure_for_eeprom = 0;

    I2C2->CR2 &= ~I2C_CR2_DMAEN;

    /* Initial Reset of MPU */
    if(mpu_reset_index < MPU_RESET_STEPS){
        DMA1_Channel4->CCR &= (uint16_t)0xFFFE; /* Disable Tx DMA */
        DMA1_Channel4->CMAR = (uint32_t)mpu_init[mpu_reset_index++];
        DMA1_Channel4->CNDTR = MPU_SINGLE_WRITE;
        DMA1_Channel4->CCR |= DMA_CCR4_EN;
        reconfigure_for_eeprom = 1;
    }

    /* Should only execute once */
    else if(reconfigure_for_eeprom == 1){
        DMA1_Channel4->CCR &= (uint16_t)0xFFFE; /* Disable Tx DMA */
        DMA1_Channel4->CNDTR = EEPROM_WRITE;
        DMA1_Channel4->CCR |= DMA_CCR4_CIRC;
        DMA1_Channel4->CCR |= DMA_CCR4_EN;

        /* Assuming DMA1_Channel5 is disabled */
        DMA1_Channel5->CMAR = (uint32_t)mpu_data; /* Update Rx DMA channel */
        DMA1_Channel5->CCR |= DMA_CCR5_EN;

        reconfigure_for_eeprom = 0; /* Prevent statement from running again */
    }

    DMA1->IFCR |= DMA_IFCR_CTCIF4;
}

/**
  *@brief Sends the MPU6050 FIFO data to eeprom_write_task for logging.
  * This interrupt only executes on EOT for I2C2_Rx.
  *
  *@pre The queue `eeprom_logQ` has been created
 */
void DMA1_Channel5_IRQHandler(void){
    I2C2->CR1 |= I2C_CR1_STOP;
    xQueueOverwriteFromISR(eeprom_logQ, mpu_data, NULL);
    DMA1->IFCR |= DMA_IFCR_CTCIF5;
}

/**
  *@brief Handles I2C2 events as defined in the reference manual.
  * WARNING: There is a dead loop in this inerrupt.
  *
  * After preliminary testing, on a ReStart command (i.e. another start is
  * programmed before a stop) there must be a ~30us delay before data can be
  * loaded into the I2C2_DR register. The only quick and easy way to create
  * this delay is with a dead loop.
  *
  * Based on our applications, 30us is much too short of a time for any process
  * to be missed so this is deemed acceptable.
  *
  * On MPU6050 reads, we have to manually send the data register because DMA
  * cannot handle requests with length < 2. `ITBUFEN` triggers this interrupt
  * on a TxE or RxNE event, but should be turned off after clearing TxE
  * to allow for future DMA requests to be processed.
  *
  * DMA requests only occur on TxE and RxNE events and hence the DMA controller
  * cannot send an address over I2C
  *
 */
void I2C2_EV_IRQHandler(void){
    volatile uint16_t status = I2C2->SR1;
    volatile uint32_t delay = 0x2F; /* SEE BELOW */
    static uint8_t restart = 0;     /* ReStart required on MPU read */

    uint8_t queue_size = (uint8_t)uxQueueMessagesWaitingFromISR(i2c2Q);
    uint8_t address_to_send = 0;
    uint8_t register_to_send = 0;

    /* Full queue indicates = read MPU, assumed empty queue after each stop */
    if(queue_size == MPU_READ_ADDRS){
        restart = 1;
        I2C2->CR2 |= I2C_CR2_ITBUFEN; /* Enables interrupt on TxE */
    }

    /* Interrupt will re-fire if queue is empty */
    if(status & I2C_SR1_SB){
        if(xQueueReceiveFromISR(i2c2Q, &address_to_send, NULL) == pdPASS){
            I2C2->DR = address_to_send;
        }
    }

    /* Clear ADDR flag, enable DMA transfers in I2C2 peripheral */
    else if(status & I2C_SR1_ADDR){
        if(!restart) I2C2->CR2 |= I2C_CR2_DMAEN;
        (void)I2C2->SR2;
    }

    /* End of transmission on Tx session */
    else if(status & I2C_SR1_BTF){
        I2C2->CR1 |= I2C_CR1_STOP;
        I2C2->CR1 &= ~I2C_CR1_STOP;
    }

    /* Only runs on MPU FIFO read */
    else if(status & I2C_SR1_TXE){
        if(xQueueReceiveFromISR(i2c2Q, &register_to_send, NULL) == pdPASS){
            I2C2->CR2 &= ~I2C_CR2_ITBUFEN;
            restart = 0;
            I2C2->DR = register_to_send;
            I2C2->CR1 |= I2C_CR1_START;
            while(delay--); /* WARNING: SOME DELAY REQUIRED AFTER RESTART */
        }
    }
}

/**
  *@brief Unblocks `mpu_read_task` which starts a read of the MPU6050.
  * This interrupt should only execute on the MPU6050 INT pin going high.
  * INT indicates the MPU6050 data is ready.
  *
  *@pre `mpu_read_task` has been created with task handle `mpu_read_handle`
 */
void EXTI9_5_IRQHandler(void){
    EXTI->PR |= EXTI_PR_PR6;
    vTaskNotifyGiveFromISR(mpu_read_handle, NULL);
}
