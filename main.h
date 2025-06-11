#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "stm32l476xx.h"
#include <stddef.h>// For NULL definition

/**
**************************************************************************************************
* File: main.h
* Defines the constants, enums, structures, and function prototypes
**************************************************************************************************
*/
//Macros

/******************************
*			Timers
*******************************/
#define startSysTickTimer_MACRO (SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk)
#define startTIM2_MACRO (TIM2->CR1 |= (1 << 0)) //start timer
#define stopTIM2_MACRO (TIM2->CR1 &= ~(1 << 0)) //stop timer

#define SYS_CLK_FREQ 4000000// default frequency of the device = 4 MHZ
#define cntclk 1000

#define TWO_HZ_SPEED 2
#define THREE_HZ_SPEED 3
#define FOUR_HZ_SPEED 4
#define SIX_HZ_SPEED 6

#define SYSTICK_1MS ((SYS_CLK_FREQ / 1000) - 1)


/******************************
*			Buttons
*******************************/
#define DEBOUNCE_DELAY 20//arbitrary value that provided an appropriate delay while still being responsive
#define SpecialButtonPressed ((!(GPIOC->IDR & (0x1 << 13))))

/******************************
*			 LEDS
*******************************/
#define NUM_of_LEDS 16
#define GPIOApins_used 7 //number of GPIOA pins used for LEDs
#define GPIOBpins_used 10 //number of GPIOB pins used for LEDs
#define GPIOCpins_used 7 //number of GPIOC pins used for LEDs

#define TEST_ACTIVE_LED_ON (GPIOC->ODR |= (0x1 << 10))
#define TEST_ACTIVE_LED_OFF (GPIOC->ODR &= ~(0x1 << 10))
#define TEST_ACTIVE_LED_TOGGLE (GPIOC->ODR ^= (0x1 << 10))

#define FAIL_LED_ON (GPIOC->ODR |= (0x1 << 12))
#define FAIL_LED_OFF (GPIOC->ODR &= ~(0x1 << 12))
#define FAIL_LED_TOGGLE (GPIOC->ODR ^= (0x1 << 12))

#define PATTERN_SELECT_LED_ON(level) SELECTED_PATTERN_INDICATOR_LEDS[level].port->ODR |= (0x1 << SELECTED_PATTERN_INDICATOR_LEDS[level].pin)
#define PATTERN_SELECT_LED_OFF(level) SELECTED_PATTERN_INDICATOR_LEDS[level].port->ODR &= ~(0x1 << SELECTED_PATTERN_INDICATOR_LEDS[level].pin)

#define SPECIAL_LED_ON (GPIOA->ODR |= (0x1 << 5))
#define SPECIAL_LED_OFF (GPIOA->ODR &= ~(0x1 << 5))


/******************************
*		Time keeping
*******************************/
#define START_SCREEN_TIME 2000 //duration of the start screen state
#define PATTERN_RELOAD_TIME 800 //time it takes to start displaying pattern

#define SYSTEM_MODE_SWITCH_DURATION 1000
#define FAIL_SCREEN_DURATION 1500 //time the fail screen state is active
#define CELEBRATION_DURATION 3500 //time the celebration screen state is active
#define NEXT_GAME_COOLDOWN_DURATION 1000
#define LED_TOGGLE_TIME 250 //time an LED stays on when one of the 4 standard buttons is pressed.

#define TIME_500MS 500
#define TIME_10_SEC 10000
/******************************
*		GAME related
*******************************/
#define PATTERN_COUNT 4 //number of patterns user can cycle through
#define MAX_PATTERN_SIZE 20 //max length of pattern sequence





//enums
enum AVAILIBLE_PATTERNS {PATTERN_1, PATTERN_2, PATTERN_3, PATTERN_4};

enum gamestates {start_screen_display, leaving_start_screen, loading_pattern, displaying_pattern, test,
	fail_screen, celebration_screen, wait_for_next_game , system_mode_switch};

enum identifications {B=0, R=1, Y=2, G=3, SPECIAL, ALL, UNASSIGNED_ID};
enum system_states {learning_mode, challenge_mode};

//Structures
struct Light_Emitting_Diode{
	GPIO_TypeDef *port; //GPIOx
	uint32_t pin; //Pin number
};

struct button{
	volatile uint32_t press_pending; //flag pending button press
	volatile uint32_t debounce_counter; //counter for debouncing
	volatile uint32_t press_ready; //flag for valid button press
	volatile uint32_t LED_TOGGLE_TIMESTAMP; //used to simulate LED toggle behavior
	struct LEDblock  *led_block; //each standard button has an associated LED block
	enum identifications ID; //gives the button an associated ID: B, G, Y, R, SPECIAL
};




struct LEDblock{
	struct Light_Emitting_Diode* leds; //Array containing the hardware info for the LEDblock
	const uint32_t count; //indicates the number of individual LEDS that make up a LED block
	enum identifications ID; //gives an LEDblock an associated ID: B, G, Y, R....
};


struct GAME_PATTERN{
	uint32_t sequence[MAX_PATTERN_SIZE]; //Hard-coded array containing the sequence/order of the colors in the pattern
	struct LEDblock *dataBLOCKS[MAX_PATTERN_SIZE]; //array containing the LEDS that correspond to the colors in the sequence
    const uint32_t length; //length of the sequence
	uint32_t current_step; //tracks the progress of the sequence - this value will be incremented

};


//Function Prototypes
void GAME_OR_ROUND_OVER (void);
void HANDLE_POST_DEBOUNCED_BUTTONS(uint32_t currentTIME_ms);
void HANDLE_BUTTON_DEBOUNCE_PROTOCOLS(uint32_t currentTIME_ms);


//extern variables
extern enum system_states system_state;
extern enum AVAILIBLE_PATTERNS selected_pattern;
extern enum gamestates gamestate;
extern volatile uint32_t msTimer;
extern uint32_t system_timeSTAMP;
extern uint32_t input_index;
extern uint32_t active_pattern_length;
extern int32_t LEDS_remaining_off;

extern volatile uint32_t inactivity_timeout;

#endif /* MAIN_H */
