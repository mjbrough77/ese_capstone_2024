/**
  *@file usart_ese.c
  *@author Mitchell Brough
  *@brief Library for configuring USART3 and tasks related to transmitting
  *       data over USART
  *
  *@version 1.0
  *@date 2024-03-30
  *
  *@copyright Copyright (c) 2024
 */

#include "../../project_types.h"        /* typedefs and macros */
#include "../include/tasks_ese.h"       /* FreeRTOS task API functions */
#include "../include/queues_ese.h"      /* Queue handles + API functions */

#include "../include/usart_ese.h"

void configure_usart3(void){
    USART3->CR1 |= USART_CR1_UE;    /* Enable USART3 */
    USART3->BRR = 0x20;             /* Baud = 625k, See RM0008 */

    USART3->CR3 |= USART_CR3_DMAR;  /* Enable DMA_USART3_Rx */

    /* Configure interrupt, but wait until MPU6050 finished before unmask */
    NVIC_SetPriority(USART3_IRQn, 7);
    NVIC_EnableIRQ(USART3_IRQn);

    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; /* Enable Tx, Rx */
}

void prepare_usart3_dma(void){
    /* USART3_Tx DMA Channel */
    DMA1_Channel2->CPAR = (uint32_t)&USART3->DR;
    DMA1_Channel2->CNDTR = sizeof(UsartBuffer_t);
    DMA1_Channel2->CCR |= DMA_CCR2_TCIE | DMA_CCR2_MINC | DMA_CCR2_DIR;
    DMA1_Channel2->CCR |= DMA_CCR2_CIRC;
    NVIC_SetPriority(DMA1_Channel2_IRQn, 7);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);

    /*
     *DMA1_Channel2 finished configuration in send_boardT_task()
     */

    /* USART3_Rx DMA Channel */
    DMA1_Channel3->CPAR = (uint32_t)&USART3->DR;
    DMA1_Channel3->CNDTR = sizeof(Distances_t);
    DMA1_Channel3->CCR |= DMA_CCR3_TCIE | DMA_CCR3_MINC;
    DMA1_Channel3->CCR |= DMA_CCR3_CIRC;
    NVIC_SetPriority(DMA1_Channel3_IRQn, 9);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);

    /*
     * DMA1_Channel3 finished configuration in DMA1_Channel4_IRQHandler()
     */
}

void send_ready_signal(void){
    USART3->CR1 |= USART_CR1_TCIE;
}

_Noreturn void send_boardT_task(void* param){
    UsartBuffer_t usart_send;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    WheelVelocity_t left_vel = 0;
    WheelVelocity_t right_vel = 0;
    WheelVelocity_t total_velocity = 0;
    uint32_t usart_flag_to_send = 0;
    uint8_t wait_for_clear = 0;

    /* Finish configuring DMA_USART3_Tx */
    DMA1_Channel2->CMAR = (uint32_t)&usart_send;
    DMA1_Channel2->CCR |= DMA_CCR2_EN;

    /* Constantly transfers wheel speed data */
    while(1){
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS( SPEED_SAMPLE_MS ));
        usart_flag_to_send = ulTaskNotifyTake(pdTRUE, NULL);

        if(usart_flag_to_send >= USART_CLEAR_ERROR){
            usart_send = (UsartBuffer_t)usart_flag_to_send;
            if(usart_flag_to_send == USART_CLEAR_ERROR) wait_for_clear = 0;
            else wait_for_clear = 1;
            USART3->CR3 |= USART_CR3_DMAT; /* Send error code to boardT*/
        }

        else if(wait_for_clear == 0){
            xQueuePeek(left_wheel_dataQ, &left_vel, NULL);
            xQueuePeek(right_wheel_dataQ, &right_vel, NULL);
            total_velocity = (left_vel+right_vel)/2;
            if(total_velocity < 0) total_velocity = -total_velocity;
            usart_send = (UsartBuffer_t)total_velocity;
            USART3->CR3 |= USART_CR3_DMAT; /* Send speed to boardT */
        }

        (void)param;
    }
}
