
#ifndef TIMERS_H_
#define TIMERS_H_
#include "main.h"
/**************************************************************************************************
* File: timers.h
* Header file for timers.c
*************************************************************************************************/
void STOP_TIM2(void);
void START_TIM2(void);
void STOP_TIM2_FOR_CONFIG(void);
void STOP_TIM2_SAFELY(void);

void configureSysTickInterrupt(void);
void updateARR(uint32_t frequency_hz);
void configureTIM2(void);
#endif /* TIMERS_H_ */
