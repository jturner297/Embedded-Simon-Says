#include "timers.h"
/**************************************************************************************************
* File: timers.c
* Defines functions related to timer control and configuration
*************************************************************************************************/
//================================================================================================
// configureSysTickInterrupt()
// @parm: none
// @return: none
// 		Configures the hardware so the SysTick timer will trigger every 1ms
//================================================================================================
void configureSysTickInterrupt(void)
{
	SysTick->CTRL = 0; //disable SysTick timer
	NVIC_SetPriority(SysTick_IRQn, 1); //set priority level at 1
	SysTick->LOAD = SYSTICK_1MS; //set the counter reload value 1ms
	SysTick->VAL = 0; //reset SysTick timer value
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; //use system clock
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //enable SysTick interrupts
}
//================================================================================================
// updateARR()
// @parm: frequency_hz = Value used in the ARR (determines delay)
// @return: none
// 		Updates the ARR value and resets the counter
//================================================================================================
void updateARR (uint32_t frequency_hz){
	STOP_TIM2_FOR_CONFIG(); //stop timer
	TIM2->ARR = ((cntclk/frequency_hz) - 1); //update ARR value
	//  TIM2->CNT = 0; already handled in STOP_TIM2_FOR_CONFIG(); function
}


//================================================================================================
// configureTIM2()
// @parm: none
// @return: none
// 		Configures the general purpose timer TIM2
//================================================================================================
void configureTIM2 (void)
{
	  RCC->APB1ENR1 |= (1 << 0);  // Enable TIM2 clock
	  TIM2->PSC = (SYS_CLK_FREQ/cntclk -1);
	  updateARR(TWO_HZ_SPEED);
	  TIM2->DIER |= (1 << 0);          // Enable update interrupt
	    TIM2->CR1 |= (1 << 0);           // Start timer
	     NVIC_SetPriority(TIM2_IRQn, 0);  //set priority level at 0
	     NVIC_EnableIRQ(TIM2_IRQn);       // Enable interrupt in NVIC
}

//================================================================================================
// RECONFIG_AND_RESTART_TIMER()
// @parm:  frequency_hz = Value used in the ARR (determines delay)
// @return: none
// 		Stops TIM2 and reconfigures it at the desired frequency. After reconfiguration, the timer
//		is immediately restarted.
//================================================================================================
void RECONFIG_AND_RESTART_TIMER(uint32_t frequency_hz){
	updateARR(frequency_hz);//stop timer and set ARR value to 2HZ
	START_TIM2(); //start timer
}

//================================================================================================
// STOP_TIM2()
// @parm: none
// @return: none
// 		Stops TIM2
//================================================================================================
void STOP_TIM2(void){
	stopTIM2_MACRO;
	TIM2->CNT = 0;

}
//================================================================================================
// START_TIM2()
// @parm: none
// @return: none
// 		Start TIM2
//================================================================================================
void START_TIM2(void){
   TIM2->CNT = 0;          // Reset counter (critical!)
   TIM2->SR = 0;           // Clear pending interrupts
   startTIM2_MACRO;  // Enable timer
}
//================================================================================================
// STOP_TIM2_FOR_CONFIG()
// @parm: none
// @return: none
// 		Stops TIM2 for purpose of reconfiguration
//================================================================================================
void STOP_TIM2_FOR_CONFIG(void) {
	stopTIM2_MACRO;   // Stop timer
    TIM2->CNT = 0;               // Reset counter
    TIM2->EGR = TIM_EGR_UG;      // Force register update
}
//================================================================================================
// STOP_TIM2_SAFELY()
// @parm: none
// @return: none
// 		Safetly stops TIM2
//================================================================================================
void STOP_TIM2_SAFELY(void) {
	stopTIM2_MACRO;   // Disable timer
    TIM2->EGR = TIM_EGR_UG;      // Force update event (syncs registers)
    TIM2->SR = 0;                // Clear pending interrupts
}
