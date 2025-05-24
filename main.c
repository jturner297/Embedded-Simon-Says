#include <stdint.h>
#include <stm32l476xx.h>
#include "main.h"
#include "input.h"
#include "leds.h"
#include "timers.h"
#include "patterns.h"
/***************************************************************************************************
* @file main.c
* @brief Main program body
* @author: Justin Turner
* @version 1.7
**************************************************************************************************
*/

enum gamestates gamestate = start_screen_display;//sets starting gamestate
enum AVAILIBLE_PATTERNS selected_pattern = PATTERN_1; //sets the default selected pattern

volatile uint32_t msTimer = 0; //value used in the SysTick
volatile uint32_t LED_updateFLAG  =0;//value used in TIM2handler to control led animation and updates

uint32_t system_timeSTAMP = 0; //used to create timestamps for game events and leaving states
uint32_t input_index = 0; //used during the test state. Increments with each successful button input

int main(void)
{
	//--initialize hardware------------------------------------------------------------------------
	uint32_t GPIOA_pins[] = {9,7,6,12,11,15}; //LEDs connected to port A's pins
	uint32_t GPIOB_pins[] = {8,9,12,11,6,2,14,4,13,5}; //LEDs connected to port B's pins
	uint32_t GPIOC_pins[] = {8,9,6,5,7 , 10,12};//LEDs connected to port C's pins

	configure_LEDS(GPIOA, GPIOA_pins, GPIOApins_used, 0);//configure port A's LEDs
	configure_LEDS(GPIOB, GPIOB_pins, GPIOBpins_used, 1);//configure port B's LEDs
	configure_LEDS(GPIOC, GPIOC_pins, GPIOCpins_used, 2);//configure port C's LEDs
	configure_external_switches();//configure the switches and their dedicated interrupts
	configureSysTickInterrupt();
	configure_Board_Button(); //configure the board button and its interrupt
	configureTIM2(); //configure general purpose TIM2
	startSysTickTimer_MACRO;

	initialize_patterns();



	//---------------------------------------------------------------------------------------------
	// This main loop continuously drives game transitions/progress by evaluating the current
	// gamestate and executing the appropriate logic for each phase. Depending on the state, it
	// activates or deactivates LEDs, processes button interactions, and manages pattern playback.
	//---------------------------------------------------------------------------------------------
	while (1)
	{
		//DIFFICULTY INDICATOR LED IS ALWAYS ACTIVE
		DIFFICULTY_INDICATOR_LED_ON(selected_pattern);

		//handle player win
		if (input_index == PATTERNS[selected_pattern]->length ){ //if the player has won...
	    	system_timeSTAMP = msTimer;//create timestamp
	    	TEST_ACTIVE_LED_OFF; //the test is over, so turn off test indicator LED
			input_index = 0; //reset input_index
		    updateARR(SIX_HZ_SPEED); //stop the timer and update the speed of the timer
			START_TIM2(); //start the timer
			gamestate = celebration_screen; //exit test screen and go to celebration screen (display win animation)
	    }

		//handle game
		switch(gamestate){
		case start_screen_display:
			TURN_ON_LED_BLOCK(&STATUS_INDICATOR_BLOCK); //turn on the fail and test LEDS
			TURN_ON_LED_BLOCK(&all_block);//turn on all colors
			system_timeSTAMP = msTimer; //create timestamp for leaving the start screen
			gamestate = leaving_start_screen;
			break;
		case leaving_start_screen://begin leaving the start screen
				if(msTimer - system_timeSTAMP >= START_SCREEN_TIME){//if start screen time is up
					TURN_OFF_LED_BLOCK(&STATUS_INDICATOR_BLOCK); //turn off the fail and test LEDS
					TURN_OFF_LED_BLOCK(&all_block); //turn off all colors
					system_timeSTAMP = msTimer; //create timestamp for next state
					gamestate = loading_pattern;
				}
				break;
		case loading_pattern: //prepare to display pattern
				if(msTimer - system_timeSTAMP >= PATTERN_RELOAD_TIME){ //if the time is up
					updateARR(TWO_HZ_SPEED);//stop timer and set ARR value to 2HZ
					START_TIM2(); //start timer
					system_timeSTAMP = 0; //reset
				    PATTERNS[selected_pattern]->current_step = 0; //reset pattern progress
					gamestate = displaying_pattern; //transition
				}
				break;
			case displaying_pattern:
				if (LED_updateFLAG == 1){ //if TIM2 has set the LED update FLAG....
					LED_updateFLAG = 0;//clear - will be set back to 1 again by TIM2
					//display current step in the pattern and progress to next step
					//stops timer and enters test state when finished
					DISPLAY_PATTERN(PATTERNS[selected_pattern]);
				}
				break;
			case test:
				//can either go to celebration_screen or fail_screen based on user input
				break;
			case celebration_screen: //the player has passed the test
				if (LED_updateFLAG == 1){//if TIM2 has set the LED update FLAG....
						LED_updateFLAG =0;//clear - will be set back to 1 again by TIM2
						QUICK_TOGGLE_LED_BLOCK(&all_block);//toggle all colors
				}
				if (msTimer - system_timeSTAMP>= CELEBRATION_DURATION){//if the win screen time is up...
					TURN_OFF_LED_BLOCK(&all_block); //turn off all colors
					system_timeSTAMP = msTimer; //create time stamp to leave next phase
					gamestate = wait_for_next_game; //transition

				}
				break;
			case fail_screen://the player pressed the wrong button, failing the test
				if (LED_updateFLAG == 1){//if TIM2 has set the LED update FLAG....
					LED_updateFLAG = 0;//clear - will be set back to 1 again by TIM2
					FAIL_LED_TOGGLE; //Toggle FAIL LED
				}
				if (msTimer - system_timeSTAMP>= FAIL_SCREEN_DURATION){//if TIM2 has set the LED update FLAG....
					TURN_OFF_LED_BLOCK(&all_block);//turn off all colors
					FAIL_LED_OFF;//turn off the FAIL LED
					system_timeSTAMP = msTimer; //create timestamp
					gamestate = loading_pattern; //load the pattern again
				}
				break;
			case wait_for_next_game: //cool down after a win
				if (msTimer - system_timeSTAMP>= NEXT_GAME_COOLDOWN_DURATION){//if the cool down time has passed...
			    	updateARR(TWO_HZ_SPEED);//stop timer and set ARR value to 2HZ
					gamestate = start_screen_display;//enter the start screen
				}
				break;


		}//end switch


		HANDLE_POST_DEBOUNCED_BUTTONS(msTimer);	//After the a button press is validated, carries out the appropriate actions


	}
}//end main


//================================================================================================
// EXTI4_IRQHandler()
// @parm: none
// @return: none
//		Initiates the debounce protocol for the BLUE button
//================================================================================================
void EXTI4_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 4)) {//if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 4);  // Clear interrupt flag
		  if (gamestate == test){ //only begin process if game is in test phase
		  INITIATE_DEBOUNCE_PROTOCOL(&BLUE_BUTTON, msTimer);
		  }
	}
}

//================================================================================================
// EXTI0_IRQHandler()
// @parm: none
// @return: none
//		Initiates the debounce protocol for the RED button
//================================================================================================
void EXTI0_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 0)) {//if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 0);  // Clear interrupt flag
		  if (gamestate == test){ //only begin process if game is in test phase
		  INITIATE_DEBOUNCE_PROTOCOL(&RED_BUTTON, msTimer);
		  }
	}
}
//================================================================================================
// EXTI2_IRQHandler()
// @parm: none
// @return: none
//		Initiates the debounce protocol for the YELLOW button
//================================================================================================
void EXTI2_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 2)) {//if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 2);  // Clear interrupt flag
		  if (gamestate == test){ //only begin process if game is in test phase
		  INITIATE_DEBOUNCE_PROTOCOL(&YELLOW_BUTTON, msTimer);
		  }
	}
}



//================================================================================================
// EXTI1_IRQHandler()
// @parm: none
// @return: none
//		Initiates the debounce protocol for the GREEN button
//================================================================================================
void EXTI1_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 1)) { //if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 1);  // Clear interrupt flag
		  if (gamestate == test){ //only begin process if game is in test phase
			  INITIATE_DEBOUNCE_PROTOCOL(&GREEN_BUTTON, msTimer);
		  }

	}
}



//================================================================================================
// EXTI15_10_IRQHandler()
// @parm: none
// @return: none
//		Initiates the debounce protocol for the SPECIAL button
//================================================================================================
void EXTI15_10_IRQHandler(void)
{
	if (EXTI->PR1 & (0x1 << 13)) { //if the interrupt flag is set....
		  EXTI->PR1 |= (0x1 << 13);  // Clear interrupt flag
		  INITIATE_DEBOUNCE_PROTOCOL(&SPECIAL_BUTTON, msTimer);

	}
}
//================================================================================================
// SysTick_Handler()
// @parm: none
// @return: none
// 		Increments msTimer variable and handles the debounce protocols for all buttons
//================================================================================================
void SysTick_Handler(void)
{
	  msTimer++; //goes up every 1ms
	  HANDLE_BUTTON_DEBOUNCE_PROTOCOLS(msTimer); //validates the buttons

}
//================================================================================================
// TIM2_IRQHandler()
// @parm: none
// @return: none
//		Sets the LED update Flag when the game is in these specific gamestates:
//			- displaying_pattern: Active pattern playback
//			- celebration screen: Win animations
//			- fail_screen: Loss/Fail feedback
//================================================================================================
void TIM2_IRQHandler(void)
{
	if (TIM2->SR & (1 << 0)) {
		TIM2->SR &= ~(1 << 0);//clear interrupt flag
		if (gamestate == displaying_pattern||gamestate == celebration_screen ||gamestate == fail_screen){
			LED_updateFLAG =1; //set update flag
		}
	}
}

//================================================================================================
// HANDLE_POST_DEBOUNCED_BUTTON()
// @parm: currentTIME_ms - Current system time in milliseconds.
// @return: none
// 		Calls HANDLE_DEBOUNCED_BUTTON() for all game buttons, executing any valid post-debounce
//		actions.
//================================================================================================
void HANDLE_POST_DEBOUNCED_BUTTONS(uint32_t currentTIME_ms){
	HANDLE_DEBOUNCED_BUTTON(&SPECIAL_BUTTON, currentTIME_ms);
	HANDLE_DEBOUNCED_BUTTON(&BLUE_BUTTON, currentTIME_ms);
	HANDLE_DEBOUNCED_BUTTON(&RED_BUTTON, currentTIME_ms);
	HANDLE_DEBOUNCED_BUTTON(&YELLOW_BUTTON,currentTIME_ms);
	HANDLE_DEBOUNCED_BUTTON(&GREEN_BUTTON,  currentTIME_ms);

}
//================================================================================================
// HANDLE_POST_DEBOUNCED_BUTTON()
// @parm: currentTIME_ms - Current system time in milliseconds.
// @return: none
// 		Runs the debounce protocol for all buttons.
//================================================================================================
void HANDLE_BUTTON_DEBOUNCE_PROTOCOLS(uint32_t currentTIME_ms){
	 DEBOUNCE_PROTOCOL(&BLUE_BUTTON, currentTIME_ms);
	 DEBOUNCE_PROTOCOL(&RED_BUTTON, currentTIME_ms);
	 DEBOUNCE_PROTOCOL(&YELLOW_BUTTON, currentTIME_ms);
	 DEBOUNCE_PROTOCOL(&GREEN_BUTTON,  currentTIME_ms);
	 DEBOUNCE_PROTOCOL(&SPECIAL_BUTTON,  currentTIME_ms);
}































