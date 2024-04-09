/**
  *@file drive.c
  *@author Mitchell Brough
  *@brief Function definitions for controlling and printing to the seven-segment
  *       displays
  *
  *@version 1.0
  *@date 2024-01-19
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#include "../../project_types.h"    /* ChairSpeed_t */

#include "../include/display_ese.h"

void write_tens_sevenseg(uint8_t num){
	switch(num){
		case 0:
			GPIOB->BSRR |= GPIO_BSRR_BR5 | GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 1:
			GPIOB->BSRR |= GPIO_BSRR_BS5 | GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 2:
			GPIOB->BSRR |= GPIO_BSRR_BR5 | GPIO_BSRR_BS4 | GPIO_BSRR_BR10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 3:
			GPIOB->BSRR |= GPIO_BSRR_BS5 | GPIO_BSRR_BS4 | GPIO_BSRR_BR10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 4:
			GPIOB->BSRR |= GPIO_BSRR_BR5 | GPIO_BSRR_BR4 | GPIO_BSRR_BS10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 5:
			GPIOB->BSRR |= GPIO_BSRR_BS5 | GPIO_BSRR_BR4 | GPIO_BSRR_BS10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 6:
			GPIOB->BSRR |= GPIO_BSRR_BR5 | GPIO_BSRR_BS4 | GPIO_BSRR_BS10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 7:
			GPIOB->BSRR |= GPIO_BSRR_BS5 | GPIO_BSRR_BS4 | GPIO_BSRR_BS10;
            GPIOA->BSRR |= GPIO_BSRR_BR9;
			break;
		case 8:
			GPIOB->BSRR |= GPIO_BSRR_BR5 | GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOA->BSRR |= GPIO_BSRR_BS9;
			break;
		case 9:
		default:
			GPIOB->BSRR |= GPIO_BSRR_BS5 | GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOA->BSRR |= GPIO_BSRR_BS9;
			break;
	}
}

void write_ones_sevenseg(uint8_t num){
	switch(num){
		case 0:
            GPIOA->BSRR |= GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOB->BSRR |= GPIO_BSRR_BR9 | GPIO_BSRR_BR8;
            break;
		case 1:
            GPIOA->BSRR |= GPIO_BSRR_BS4 | GPIO_BSRR_BR10;
            GPIOB->BSRR |= GPIO_BSRR_BR9 | GPIO_BSRR_BR8;
			break;
		case 2:
            GPIOA->BSRR |= GPIO_BSRR_BR4 | GPIO_BSRR_BS10;
            GPIOB->BSRR |= GPIO_BSRR_BR9 | GPIO_BSRR_BR8;
			break;
		case 3:
            GPIOA->BSRR |= GPIO_BSRR_BS4 | GPIO_BSRR_BS10;
            GPIOB->BSRR |= GPIO_BSRR_BR9 | GPIO_BSRR_BR8;
			break;
		case 4:
            GPIOA->BSRR |= GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOB->BSRR |= GPIO_BSRR_BS9 | GPIO_BSRR_BR8;
			break;
		case 5:
            GPIOA->BSRR |= GPIO_BSRR_BS4 | GPIO_BSRR_BR10;
            GPIOB->BSRR |= GPIO_BSRR_BS9 | GPIO_BSRR_BR8;
			break;
		case 6:
            GPIOA->BSRR |= GPIO_BSRR_BR4 | GPIO_BSRR_BS10;
            GPIOB->BSRR |= GPIO_BSRR_BS9 | GPIO_BSRR_BR8;
			break;
		case 7:
            GPIOA->BSRR |= GPIO_BSRR_BS4 | GPIO_BSRR_BS10;
            GPIOB->BSRR |= GPIO_BSRR_BS9 | GPIO_BSRR_BR8;
			break;
		case 8:
            GPIOA->BSRR |= GPIO_BSRR_BR4 | GPIO_BSRR_BR10;
            GPIOB->BSRR |= GPIO_BSRR_BR9 | GPIO_BSRR_BS8;
			break;
		case 9:
		default:
            GPIOA->BSRR |= GPIO_BSRR_BS4 | GPIO_BSRR_BR10;
            GPIOB->BSRR |= GPIO_BSRR_BR9 | GPIO_BSRR_BS8;
			break;
	}
}

void send_to_display(uint8_t digits){
    if(digits > 99) digits = 99;        /* Saturate digits to 99 */
    write_tens_sevenseg(digits / 10);   /* Display tens digit */
	write_ones_sevenseg(digits % 10);   /* Display ones digit */
}

void turn_on_display(void){
    GPIOA->BSRR |= GPIO_BSRR_BS5;
}

void turn_off_display(void){
    GPIOA->BSRR |= GPIO_BSRR_BR5;
}

_Noreturn void print_speed_task(void* param){
    ChairSpeed_t current_speed = 0;
    uint32_t display_event = 0;

    while(1){

        /* Unblock on new USART information */
        display_event = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if(display_event == DISPLAY_ERROR)
            send_to_display(DISPLAY_ERROR);

        else{
            current_speed = (ChairSpeed_t)display_event;
            send_to_display(current_speed/SPEED_SCALE);
        }

        (void)param;
    }
}
