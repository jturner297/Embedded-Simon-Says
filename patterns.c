
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
	bind_sequnece_to_pattern(&SUPER_EASY_PATTERN);
	bind_sequnece_to_pattern(&EASY_PATTERN);
	bind_sequnece_to_pattern(&MEDIUM_PATTERN);
	bind_sequnece_to_pattern(&HARD_PATTERN);
	bind_sequnece_to_pattern(&IMPOSSIBLE_PATTERN);
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
	if (pattern->current_step >= pattern->length*2 ){ //the pattern is finished
		pattern->current_step=0; //reset current_step
		STOP_TIM2_SAFELY(); //stop the timer
		TEST_ACTIVE_LED_ON; //turn on the green led to indicate test has started
		input_index = 0; //reset input_index
		gamestate=test; //enter test state
	}
}


//================================================================================================
// CANCEL_ACTIVE_PATTERN()
// @parm:  *pattern - pointer to GAME_PATTERN struct
// @return: none
// 		 Cancels the active pattern by stopping the timer, turning off all LEDS and resetting the
//		 pattern progress
//================================================================================================
void CANCEL_ACTIVE_PATTERN(struct GAME_PATTERN *pattern){
	STOP_TIM2_SAFELY();
	TURN_OFF_LED_BLOCK(&all_block);
	pattern->current_step = 0;
}


//patterns that the user can cycle through with the special button
struct GAME_PATTERN *PATTERNS[] = {&SUPER_EASY_PATTERN, &EASY_PATTERN, &MEDIUM_PATTERN, &HARD_PATTERN};

//------patterns-----------------------------
struct GAME_PATTERN SUPER_EASY_PATTERN = {
    .sequence = {B, G, R, Y},
    .dataBLOCKS = {}, //filled in later by function
    .length = 4,
    .current_step = 0
};

struct GAME_PATTERN EASY_PATTERN = {
    .sequence = {R, G, Y, B, G},
    .dataBLOCKS = {},
    .length = 5,
    .current_step = 0
};

struct GAME_PATTERN MEDIUM_PATTERN = {
    .sequence = {Y, B, R, Y, R, G},
    .dataBLOCKS =  {},
    .length = 6,
    .current_step = 0
};


struct GAME_PATTERN HARD_PATTERN = {
    .sequence = {G, G, Y, B, G, R, B, G },
    .dataBLOCKS = {},
    .length = 8,
    .current_step = 0
};

struct GAME_PATTERN IMPOSSIBLE_PATTERN = {
	.sequence = {R, B, G, B, Y, G, R, G, Y, B, G},
	.dataBLOCKS =  {},
	.length = 11,
	.current_step = 0
};


