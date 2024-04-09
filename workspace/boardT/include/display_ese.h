/**
  *@file drive.h
  *@author Mitchell Brough
  *@brief Function prototypes for controlling and printing to the seven-segment
  *       displays
  *
  *@version 1.0
  *@date 2024-01-19
  *
  *@copyright Copyright (c) 2024 Mitchell Brough
 */

#ifndef DISPLAY_ESE_H
#define DISPLAY_ESE_H

#include "stdint.h" /* uint8_t */

/**
  *@brief Writes a value from 0-9 on the ones digit
  *
  * See schematic for pin information
  *
  * Function originally written by Emily Schwab
  *
  *@param num value from 0-9
  *@pre num has been checked that it is in the bounds 0-9
 */
void write_ones_sevenseg(uint8_t num);

/**
  *@brief Writes a value from 0-9 on the tens digit
  *
  * See schematic for pin information
  *
  * Function originally written by Emily Schwab
  *
  *@param num value from 0-9
  *@pre num has been checked that it is in the bounds 0-9
 */
void write_tens_sevenseg(uint8_t num);

/**
  *@brief Outputs a number to the two seven-segment displays
  *
  *@param digits value from 0-99
  *@pre BL has been set to high
 */
void send_to_display(uint8_t digits);

/**
  *@brief Resets the BL input to the seven-segment decoders
 */
void turn_on_display(void);

/**
  *@brief Sets the BL input to the seven-segment decoders
 */
void turn_off_display(void);

/**
  *@brief Prints new display value
  *
  * Display value is recieved from boardB and passed as notification
  *
  *@param param unused
 */
_Noreturn void print_speed_task(void* param);

#endif
