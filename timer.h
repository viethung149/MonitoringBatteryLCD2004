#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"

#define TIM_channel_MS      TIM4
#define TIM_channel_button     TIM2
void init_timer_interrupt_MS(uint32_t t);
void init_timer_interrupt_US(uint32_t t);
void init_timer_read_button (uint32_t t);
#endif