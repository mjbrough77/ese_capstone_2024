/**
  *@file adc_ese.c
  *@author Mitchell Brough
  *@brief Configuration of ADC1 and attached devices
  *
  *@version 1.0
  *@date 2024-03-30
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
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
    uint32_t adc_reading;
    float voltage_reading;
    float user_weight;
    uint8_t weight_error_next = 0;
    uint8_t weight_error_prev = 0;

    while(1){
    #ifdef WEIGHT_TASK_SUSPEND
        vTaskSuspend(NULL);
    #endif

        /* Weight sampled at arbitrary times, asynchronous */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS( WEIGHT_SAMPLE_MS ));

        /* Convert to weight, resolution, tare determined during testing */
        adc_reading = ADC1->DR;
        voltage_reading = (float)adc_reading * ADC_RESOLUTION - VOLTAGE_TARE;
        user_weight = voltage_reading / WEIGHT_RESOLUTION;

        /* Transition to error state if outside weight bounds */
        if(user_weight > MAX_WEIGHT || user_weight < MIN_WEIGHT){
            weight_error_next = 1;
        }
        else
            weight_error_next = 0;

        /* Error is sent if outside weight bounds, clear if weight normalized */
        if(weight_error_prev == 0 && weight_error_next == 1){
            xTaskNotify(system_error_handle, WEIGHT_NOTIFY, eSetBits);
            xTaskNotify(eeprom_write_handle, WEIGHT_NOTIFY, eSetBits);
        }
        else if(weight_error_prev == 1 && weight_error_next == 0){
            xTaskNotify(system_error_handle, ERROR_CTRL_CLEAR_WEIGHT, eSetBits);
        }

        weight_error_prev = weight_error_next;
        (void)param;
    }
}
