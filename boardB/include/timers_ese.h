#ifndef TIMERS_ESE_H
#define TIMERS_ESE_H

void configure_tim2(void);
void configure_tim3(void);
void configure_tim4(void);

void start_ultrasonics(void);
uint16_t read_right_ultrasonic(void);
uint16_t read_left_ultrasonic(void);

#endif
