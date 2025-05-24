#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "stm32l476xx.h"

/**
**************************************************************************************************
* File: main.h
* Defines the constants, enums, structures, and function prototypes
**************************************************************************************************
*/
//Macros
#define startSysTickTimer_MACRO (SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk)
#define SpecialButtonPressed ((!(GPIOC->IDR & (0x1 << 13))))
#define startTIM2_MACRO (TIM2->CR1 |= (1 << 0)) //start timer
#define stopTIM2_MACRO (TIM2->CR1 &= ~(1 << 0)) //stop timer

#define NUM_of_LEDS 16

#define GPIOApins_used 6 //number of GPIOA pins used for LEDs
#define GPIOBpins_used 10 //number of GPIOB pins used for LEDs
#define GPIOCpins_used 7 //number of GPIOC pins used for LEDs

#define SYS_CLK_FREQ 4000000// default frequency of the device = 4 MHZ
#define cntclk 1000

#define DEBOUNCE_DELAY 20//arbitrary value that provided an appropriate delay while still being responsive

#define PATTERN_COUNT 4 //number of patterns user can cycle through
#define MAX_PATTERN_SIZE 20 //max length of pattern sequence


#define LED_TOGGLE_TIME 350 //time an LED stays on when one of the 4 standard buttons is pressed
#define START_SCREEN_TIME 2000 //duration of the start screen state
#define PATTERN_RELOAD_TIME 500 //time it takes to start displaying pattern


#define FAIL_SCREEN_DURATION 1500 //time the fail screen state is active
#define CELEBRATION_DURATION 1500 //time the celebration screen state is active
#define NEXT_GAME_COOLDOWN_DURATION 1000

#define TEST_ACTIVE_LED_ON (GPIOC->ODR |= (0x1 << 10))
#define TEST_ACTIVE_LED_OFF (GPIOC->ODR &= ~(0x1 << 10))
#define TEST_ACTIVE_LED_TOGGLE (GPIOC->ODR ^= (0x1 << 10))

#define FAIL_LED_ON (GPIOC->ODR |= (0x1 << 12))
#define FAIL_LED_OFF (GPIOC->ODR &= ~(0x1 << 12))
#define FAIL_LED_TOGGLE (GPIOC->ODR ^= (0x1 << 12))

#define TWO_HZ_SPEED 2
#define FOUR_HZ_SPEED 4
#define SIX_HZ_SPEED 6

#define DIFFICULTY_INDICATOR_LED_ON(level) DIFFICULTY_INDICATOR_LEDS[level].port->ODR |= (0x1 << DIFFICULTY_INDICATOR_LEDS[level].pin)

//enums
enum AVAILIBLE_PATTERNS {PATTERN_1, PATTERN_2, PATTERN_3, PATTERN_4};
enum gamestates {start_screen_display, leaving_start_screen, loading_pattern, displaying_pattern, test, celebration_screen, fail_screen, wait_for_next_game}; // test, loading_win_pattern, win, mode_select, enter_mode_select};
enum identifications {B=0, R=1, Y=2, G=3, ALL, SPECIAL, UNASSIGNED};

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
	uint32_t is_active; //Unused, flag that tells when a block is active/on
	enum identifications ID; //gives an LEDblock an associated ID: B, G, Y, R....
};


struct GAME_PATTERN{
	uint32_t sequence[MAX_PATTERN_SIZE]; //Hard-coded array containing the sequence/order of the colors in the pattern
	struct LEDblock *dataBLOCKS[MAX_PATTERN_SIZE]; //array containing the LEDS that correspond to the colors in the sequence
        const uint32_t length; //length of the sequence
	uint32_t current_step; //tracks the progress of the sequence - this value will be incremented
};


//Function Prototypes
void HANDLE_POST_DEBOUNCED_BUTTONS(uint32_t currentTIME_ms);
void HANDLE_BUTTON_DEBOUNCE_PROTOCOLS(uint32_t currentTIME_ms);


extern enum AVAILIBLE_PATTERNS selected_pattern;
extern enum gamestates gamestate;
extern volatile uint32_t msTimer;
extern uint32_t system_timeSTAMP;
extern uint32_t input_index;




#endif /* MAIN_H */
