#include <stdint.h>
#include <stm32l476xx.h>
#include <stddef.h> // For NULL definition
#include "main.h"
#include "input.h"
#include "leds.h"
#include "timers.h"
#include "patterns.h"
/***************************************************************************************************
* @file main.c
* @brief Main program body
* @author: Justin Turner
* @version 2.0
* ------------------------------------------------------------------------------------------------
*  Embedded Simon Says
**************************************************************************************************
*/
enum system_states system_state = practice_mode; //sets the default system operating mode
enum gamestates gamestate = start_screen_display;//sets starting gamestate
enum AVAILIBLE_PATTERNS selected_pattern = PATTERN_1; //sets the default selected pattern

volatile uint32_t msTimer = 0; //value used in the SysTick
volatile uint32_t LED_updateFLAG  =0;//value used in TIM2handler to control led animation and updates

volatile uint32_t inactivity_timeout; //value used in challenge mode, if the player is inactive during the test for too long
//they automatically fail the test/lose the game

uint32_t system_timeSTAMP = 0; //used to create timestamps for game events and leaving states
uint32_t input_index = 0; //used during the test state. Increments with each successful button input
uint32_t active_pattern_length = 1; //controls what steps in the pattern/sequence is shown

int32_t LEDS_remaining_off = NUM_of_LEDS; //in challenge mode, the player's goal is to turn on all 16 LEDS in the celebration screen,
//by successfully completing the whole pattern. Every round decreases this value by 2, there are 8 total rounds.


int main(void)
{
	//--initialize hardware------------------------------------------------------------------------
	uint32_t GPIOA_pins[] = {9,7,6,12,11,15, 5}; //LEDs connected to port A's pins
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
	STOP_TIM2();

	//---------------------------------------------------------------------------------------------
	// This main loop continuously drives game transitions/progress by evaluating the current
	// gamestate and executing the appropriate logic for each phase. Depending on the state, it
	// activates or deactivates LEDs, processes button interactions, and manages pattern playback.
	//---------------------------------------------------------------------------------------------
	while (1)
	{
		//handle game
		switch(gamestate){
			case start_screen_display://new game
				if (system_state == challenge_mode){ //if the system is in challenge mode....
					LEDS_remaining_off = NUM_of_LEDS;//sets the number of LEDS the player must turn off in challenge mode
				}
				TURN_ON_LED_BLOCK(&STATUS_INDICATOR_BLOCK); //turn on the fail and test LEDS
				TURN_ON_LED_BLOCK(&all_block);//turn on all colors

				gamestate = leaving_start_screen;
				system_timeSTAMP = msTimer;  //create timestamp for leaving next state "leaving_start_screen"
				break;
			case leaving_start_screen://begin leaving the start screen
				if(msTimer - system_timeSTAMP >= START_SCREEN_TIME){//if start screen time is up
					TURN_OFF_LED_BLOCK(&STATUS_INDICATOR_BLOCK); //turn off the fail and test LEDS
					TURN_OFF_LED_BLOCK(&all_block); //turn off all colors

					gamestate = loading_pattern;
					system_timeSTAMP = msTimer; //create timestamp for leaving next state "loading_pattern"
				}
				break;
			case loading_pattern: //prepare to display pattern
				if(msTimer - system_timeSTAMP >= PATTERN_RELOAD_TIME){ //if the time is up
					RECONFIG_AND_RESTART_TIMER(TWO_HZ_SPEED);//restart the timer at 2HZ (for pattern playback)

				    PATTERNS[selected_pattern]->current_step = 0; //reset pattern progress
					system_timeSTAMP = 0; //reset, it is not needed for the next state
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
			case test://the player attempts to replicate the pattern
				if (input_index == active_pattern_length ){ //if the player passes a round...
					active_pattern_length++; //increases how much of the pattern is shown

					GAME_OR_ROUND_OVER();//creates timestamp, turns test active LED off, and resets the input_index
					if (system_state == challenge_mode){ //if the system in challenge mode.....

						//increases the amount of LEDS shown on final score board
						if(PATTERNS[selected_pattern]->length == 8){
							LEDS_remaining_off -=2;
						}
						else if(PATTERNS[selected_pattern]->length == 4){//supports sequences with lengths of 4 (For Debugging Only)
							LEDS_remaining_off -=4;
						}

					}
					//if there is no more of the pattern to show...
					if (active_pattern_length > PATTERNS[selected_pattern]->length){//test is over

						//the player has passed the entire test, do these actions....
						RECONFIG_AND_RESTART_TIMER(SIX_HZ_SPEED); //restart the timer at 6HZ (for celebration animation)
						gamestate = celebration_screen; //exit test screen and go to celebration screen (display game over animation)

					}
					else{//test is not finished
						gamestate = loading_pattern;
					}
			    }
				if (system_state == challenge_mode && inactivity_timeout > TIME_10_SEC){
					RECONFIG_AND_RESTART_TIMER(FOUR_HZ_SPEED); //restart the timer at 4HZ (for fail LED animation)
						GAME_OR_ROUND_OVER();
						gamestate = fail_screen;
				}
				break;
			case fail_screen://the player pressed the wrong button, failing
				if (LED_updateFLAG == 1){//if TIM2 has set the LED update FLAG....
					LED_updateFLAG = 0;//clear - will be set back to 1 again by TIM2
					FAIL_LED_TOGGLE; //Toggle fail LED
				}
				if (msTimer - system_timeSTAMP>= FAIL_SCREEN_DURATION){//if the fail screen duration is up...
					TURN_OFF_LED_BLOCK(&all_block);//turn off all colors
					FAIL_LED_OFF;//turn off the fail LED

					if (system_state == challenge_mode){//if they failed while in challenge mode....
						//they do not get a second chance
						RECONFIG_AND_RESTART_TIMER(SIX_HZ_SPEED); //restart the timer at 6HZ (for celebration animation)
						gamestate = celebration_screen; //exit fail screen and go to celebration screen (display game over animation)
					}
					else{//the system is in practice mode, so they get to try again endlessly
						gamestate = loading_pattern;
					}
					system_timeSTAMP = msTimer; //create timestamp for leaving the next state "loading_pattern" or "celebration_screen"
				}
				break;
			case celebration_screen: //the player has passed the test or it has ended due to them failing(challenge mode)
				if (LED_updateFLAG == 1){//if TIM2 has set the LED update FLAG....
					LED_updateFLAG =0;//clear - will be set back to 1 again by TIM2

					if(system_state == challenge_mode){//if the system is in challenge mode....
						TOGGLE_SCOREBOARD();//toggle the score board
					}
					else{// the system is in practice mode.....
						QUICK_TOGGLE_LED_BLOCK(&all_block);//toggle all colors
					}
				}
				if (msTimer - system_timeSTAMP>= CELEBRATION_DURATION){//if the win screen time is up...

					//preparing for next game
					TURN_OFF_LED_BLOCK(&all_block); //turn off all colors
					STOP_TIM2();
					CHANGE_TO_NEXT_PATTERN();
					active_pattern_length = 1; //reset back to 1 for the next game

					gamestate = wait_for_next_game; //transition
					system_timeSTAMP = msTimer; //create time stamp to leave next phase "wait_for_next_game"
				}
				break;
			case wait_for_next_game: //cool down after a win and change pattern
				if (msTimer - system_timeSTAMP>= NEXT_GAME_COOLDOWN_DURATION){//if the cool down time has passed...
					gamestate = start_screen_display;//enter the start screen
				}
				break;
			case system_mode_switch://special button held
				TURN_ON_LED_BLOCK(&SELECTED_PATTERN_INDICATOR_BLOCK); //turn on all LEDS in the selected pattern indicator block
				TURN_OFF_LED_BLOCK(&STATUS_INDICATOR_BLOCK); //turn on the fail and test LEDS

				if (msTimer - system_timeSTAMP>= SYSTEM_MODE_SWITCH_DURATION){//if the system mode change time is up...
					TURN_OFF_LED_BLOCK(&SELECTED_PATTERN_INDICATOR_BLOCK);
					selected_pattern = PATTERN_1; // reset back to pattern 1
					system_state^=1; //toggle system state

					gamestate = start_screen_display; //transition
					system_timeSTAMP = msTimer; //create time stamp to leave next phase "start_screen_display"
				}
				break;


		}//end switch

		if(system_state == challenge_mode){ //if the system is in challenge mode....
			SPECIAL_LED_ON;//turn special LED (PC13) on
		}
		else{ //the system in practice mode....
			SPECIAL_LED_OFF;//turn special LED (PC13) off
		}

		if(gamestate!= system_mode_switch){//if game has not detected a system state change....
			//handle selected pattern indicator (white LEDS)
			if (gamestate != wait_for_next_game ){ //if the gamestate is not in "wait_for_next_game"
				PATTERN_SELECT_LED_ON(selected_pattern); //Turn on the selected pattern indicator
			}
			else{// the gamestate is "wait_for_next_game"
				PATTERN_SELECT_LED_OFF(selected_pattern); //Turn off the selected pattern indicator
			}
		}

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
	  if (gamestate == test && system_state == challenge_mode){
		  inactivity_timeout++;
	  }
	  else{
		  inactivity_timeout = 0;
	  }

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
// GAME_OR_ROUND_OVER()
// @parm: none
// @return: none
// 		Called after the ending of a round or test. Creates timestamp, turns test active LED
//		off, and resets the input_index
//================================================================================================
void GAME_OR_ROUND_OVER (void){
	system_timeSTAMP = msTimer;//create timestamp for leaving the next state
	TEST_ACTIVE_LED_OFF; //turn off test indicator LED
	input_index = 0; //reset input_index
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































