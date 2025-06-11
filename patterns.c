
#include "patterns.h"
#include "leds.h"// For LED block definitions
#include "timers.h"
/**************************************************************************************************
* File: patterns.c
* Contains pattern declarations and pattern-related functions
*************************************************************************************************/

//================================================================================================
// bind_sequnece_to_pattern()
// @parm: *pattern - pointer to GAME_PATTERN struct
// @return: none
// 		 Maps color IDs in a pattern's sequence to their corresponding LED hardware blocks.
//================================================================================================
void bind_sequnece_to_pattern(struct GAME_PATTERN *pattern){
	//Iterates through each color ID in pattern->sequence
	for(uint32_t i = 0; i < pattern->length; i++){
		//Looks up the matching LED block pointer using colorID_to_LEDblock[]
		//Stores the LED block pointer in pattern->dataBLOCKS[]
		pattern->dataBLOCKS[i] = colorID_to_LEDblock[pattern->sequence[i]];
	}
}

//================================================================================================
// initialize_patterns()
// @parm: none
// @return: none
// 		 Configures all game patterns by binding their color sequences to physical LED blocks.
//================================================================================================
void initialize_patterns(void){
	for (uint32_t i = 0; i < PATTERN_COUNT; i++){
		bind_sequnece_to_pattern(PATTERNS[i]);
	}
}


//================================================================================================
// DISPLAY_PATTERN()
// @parm: *pattern - pointer to GAME_PATTERN struct
// @return: none
// 		 Toggles the LED block at the current step of the pattern sequence. Advances through the
//		 pattern by incrementing the step counter. When the full pattern has been displayed, it
//		 resets state, stops the timer, and enters the test state.
//================================================================================================
void DISPLAY_PATTERN (struct GAME_PATTERN *pattern){
	QUICK_TOGGLE_LED_BLOCK(pattern->dataBLOCKS[pattern->current_step/2]);
	pattern->current_step++;//increment through pattern
	if (pattern->current_step >= active_pattern_length*2 ){ //the pattern is finished
		pattern->current_step=0; //reset current_step
		STOP_TIM2_SAFELY(); //stop the timer
		TEST_ACTIVE_LED_ON; //turn on the green led to indicate test has started
		input_index = 0; //reset input_index
		gamestate=test; //enter test state
	}
}


//================================================================================================
// CANCEL_ACTIVE_PATTERN()
// @parm: *pattern - pointer to GAME_PATTERN struct
// @return: none
// 		 Cancels the active pattern by stopping the timer, turning off all LEDS and resetting the
//		 pattern progress
//================================================================================================
void CANCEL_ACTIVE_PATTERN(struct GAME_PATTERN *pattern){
	STOP_TIM2_SAFELY();
	TURN_OFF_LED_BLOCK(&all_block);
	pattern->current_step = 0;
	if(system_state == challenge_mode){//if system is in challenge_mode
		active_pattern_length = 1;
	}
	else{ //system is in learning mode
		active_pattern_length = pattern->length;
	}
}


//================================================================================================
// CHANGE_TO_NEXT_PATTERN()
// @parm: none
// @return: none
//		Called at the end of the game or when the SPECIAL button is tapped, changes to the next
//		available pattern. Also updates the pattern select indicator LED to reflect the new change.
//================================================================================================
void CHANGE_TO_NEXT_PATTERN(void){
	selected_pattern = (selected_pattern+1)%PATTERN_COUNT; //Change to the next pattern - wrap around when 3
	if (selected_pattern > 0){
		PATTERN_SELECT_LED_OFF(selected_pattern-1);
	}
	else{
		PATTERN_SELECT_LED_OFF(3);
	}
}




//patterns that the user can cycle through with the special button
struct GAME_PATTERN *PATTERNS[] = {&EASY, &MEDIUM, &HARD, &EXPERT};

//======patterns======================================================
struct GAME_PATTERN SUPER_EASY = {
    .sequence = {B, G, R, Y},
    .dataBLOCKS = {}, //filled in later by function
    .length = 4,
    .current_step = 0
};

struct GAME_PATTERN EASY = {
    .sequence = {B, G, B, G, R, Y, R, Y},
    .dataBLOCKS = {},
    .length = 8,
    .current_step = 0
};

struct GAME_PATTERN MEDIUM = {
    .sequence = {G, R, B, Y, G, Y, R, B},
    .dataBLOCKS =  {},
    .length = 8,
    .current_step = 0
};


struct GAME_PATTERN HARD = {
    .sequence =  {B, B, Y, G, R, G, Y, R},
    .dataBLOCKS = {},
    .length = 8,
    .current_step = 0
};

struct GAME_PATTERN EXPERT = {
		.sequence = {G, Y, R, B, Y, G, B, R},
		.dataBLOCKS =  {},
		.length = 11,
		.current_step = 0
};



