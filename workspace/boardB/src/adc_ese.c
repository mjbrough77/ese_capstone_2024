/**
  *@file adc_ese.c
  *@author Mitchell Brough
  *@brief Configuration of ADC1 and attached devices
  *
  *@version 1.0
  *@date 2024-03-30
  *
  *@copyright Copyright (c) 2024
 */

#include "../../project_types.h"    /* macros for the adc */
#include "../include/tasks_ese.h"   /* for FreeRTOS Task API functions */

#include "../include/adc_ese.h"

void configure_adc1(void){
    ADC1->CR2 |= ADC_CR2_ADON;      /* Enable ADC1 */
    ADC1->CR2 |= ADC_CR2_CONT;      /* Continuous conversion */
    ADC1->SMPR1 |= ADC_SMPR1_SMP10; /* Sample time = 12.6us @ 20MHz */
    ADC1->SQR3 = 0xA;               /* Convert IN10 (PC0) */

	ADC1->CR2 |= ADC_CR2_CAL;       /* Calibrate ADC1 */
    while(ADC1->CR2 & ADC_CR2_CAL);

    ADC1->CR2 |= ADC_CR2_ADON;      /* Start ADC conversions */
}

void find_weight_task(void* param){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    float user_weight;
    uint32_t adc_reading;
    float voltage_reading;
    uint8_t over_weight_next = 0;
    uint8_t over_weight_prev = 0;

    while(1){
    #ifdef WEIGHT_TASK_SUSPEND
        vTaskSuspend(NULL);
    #endif
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS( WEIGHT_SAMPLE_MS ));

        adc_reading = ADC1->DR;
        voltage_reading = (float)adc_reading * ADC_RESOLUTION - WEIGHT_TARE;
        user_weight = voltage_reading / WEIGHT_RESOLUTION;

        if(user_weight > MAX_WEIGHT){
            xTaskNotify(system_error_handle, MAXWEIGHT_NOTIFY, eSetBits);
            xTaskNotify(eeprom_write_handle, MAXWEIGHT_NOTIFY, eSetBits);
            over_weight_next = 1;
        }

        else
            over_weight_next = 0;

        if(over_weight_prev == 1 && over_weight_next == 0){
            xTaskNotify(system_error_handle, CLEAR_ERR_NOTIFY, eSetBits);
        }

        over_weight_prev = over_weight_next;

        (void)param;
    }
}
