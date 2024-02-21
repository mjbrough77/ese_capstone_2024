#ifndef TIMERS_ESE_H
#define TIMERS_ESE_H

void configure_tim3(void);
void configure_tim4(void);
void configure_systick(void);

void SysTick_Handler(void);
void TIM4_IRQHandler(void);

#endif
