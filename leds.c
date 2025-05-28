#include "leds.h"
/**************************************************************************************************
* File: leds.c
* Defines functions used for LED control and configuration.
*************************************************************************************************/
//functions
//================================================================================================
// configure_LEDS()
// @parm:   *port = GPIOx
// @parm:	pins[] = Array containing the port pin numbers used
// @parm:   number_of_pins = Number of port pins used
// @parm:	port_clock_num = The number associated with the port clock "0 for port A"
// @return: none
// 		Conveniently configures multiple LEDs at once. Also assists with board wire management.
//================================================================================================
void configure_LEDS (GPIO_TypeDef *port, uint32_t pins[], uint32_t number_of_pins, uint32_t port_clock_num)
{
	RCC->AHB2ENR |= (0x1 << port_clock_num);
	for (uint32_t i = 0; i < number_of_pins; i++){
		port->MODER &= ~(0x3 << (2 * pins[i]));
		port->MODER |= (0x1 << (2 * pins[i]));
		port->OTYPER &= ~(0x1 << pins[i]);
		port->OSPEEDR &= ~(0x3 << (2 * pins[i]));
		port->PUPDR &= ~(0x3 << (2 * pins[i]));
	}
}


//================================================================================================
// TOGGLE_SCOREBOARD()
// @parm:   none
// @return: none
// 		Toggles the players "score" on the 16 LEDS. Each pattern has a length of 8. Each 2 LEDS on
//		the scoreboard represent a level completion
//================================================================================================
void TOGGLE_SCOREBOARD(void){
	for (int32_t i = NUM_of_LEDS; i >= (int32_t)LEDS_remaining_off; i--) {
		all_leds[i].port->ODR ^= (0x1 << all_leds[i].pin);
	}
}



//================================================================================================
// TURN_ON_LED_BLOCK()
// @parm:   *block - pointer to LEDblock struct
// @return: none
// 		Turns on an LEDblock (Atomic Write Method)
//================================================================================================
void TURN_ON_LED_BLOCK(struct LEDblock *block) {
    // Pre-calculate all SET masks per port
    uint32_t portB_mask = 0;
    uint32_t portC_mask = 0;
    uint32_t portA_mask = 0;

    for(uint32_t i = 0; i < block->count; i++) {
        if(block->leds[i].port == GPIOA)
            portA_mask |= (1 << block->leds[i].pin);  // Add pin to GPIOA mask
        else if(block->leds[i].port == GPIOB)
            portB_mask |= (1 << block->leds[i].pin);  // Add pin to GPIOB mask
        else if(block->leds[i].port == GPIOC)
            portC_mask |= (1 << block->leds[i].pin);  // Add pin to GPIOC mask
    }

    // Atomic writes (one per port)
    if(portA_mask) GPIOA->BSRR = portA_mask;       // Set GPIOA pins
    if(portB_mask) GPIOB->BSRR = portB_mask;       // Set GPIOB pins
    if(portC_mask) GPIOC->BSRR = portC_mask;       // Set GPIOC pins
}
//================================================================================================
// TURN_OFF_LED_BLOCK()
// @parm:   *block - pointer to LEDblock struct
// @return: none
// 		Turns off an LEDblock (Atomic Write Method)
//================================================================================================
void TURN_OFF_LED_BLOCK(struct LEDblock *block) {
    // Pre-calculate all SET masks per port
    uint32_t portB_mask = 0;
    uint32_t portC_mask = 0;
    uint32_t portA_mask = 0;

    for(uint32_t i = 0; i < block->count; i++) {
        if(block->leds[i].port == GPIOA)
            portA_mask |= (1 << block->leds[i].pin);  // Add pin to GPIOA mask
        else if(block->leds[i].port == GPIOB)
            portB_mask |= (1 << block->leds[i].pin);  // Add pin to GPIOB mask
        else if(block->leds[i].port == GPIOC)
            portC_mask |= (1 << block->leds[i].pin);  // Add pin to GPIOC mask
    }

    // Atomic writes (one per port)
    if(portA_mask) GPIOA->BSRR = (portA_mask << 16);  // Reset GPIOA pins
       if(portB_mask) GPIOB->BSRR = (portB_mask << 16);  // Reset GPIOB pins
       if(portC_mask) GPIOC->BSRR = (portC_mask << 16);  // Reset GPIOC pins
}
//================================================================================================
// QUICK_TOGGLE_LED_BLOCK()
// @parm:   *block - pointer to LEDblock struct
// @return: none
// 		Toggles an LEDblock (Atomic Write Method)
//================================================================================================
void QUICK_TOGGLE_LED_BLOCK(struct LEDblock *block) {
    // Pre-calculate all toggle masks per port
    uint32_t portB_mask = 0;
    uint32_t portC_mask = 0;
    uint32_t portA_mask = 0;

    for(uint32_t i = 0; i < block->count; i++) {
        if(block->leds[i].port == GPIOA) portA_mask ^= (1 << block->leds[i].pin);
        else if(block->leds[i].port == GPIOB) portB_mask ^= (1 << block->leds[i].pin);
        else if(block->leds[i].port == GPIOC) portC_mask ^= (1 << block->leds[i].pin);
    }

    // Apply toggles atomically
    if(portA_mask) GPIOA->ODR ^= portA_mask;
    if(portB_mask) GPIOB->ODR ^= portB_mask;
    if(portC_mask) GPIOC->ODR ^= portC_mask;
}
//Structures

//all LED block
struct Light_Emitting_Diode all_leds[] = { //declaring the array of LEDS
		 {GPIOC, 8},  {GPIOC, 9}, {GPIOC, 6}, {GPIOB, 8},
		 {GPIOC, 5}, {GPIOB, 9},  {GPIOA, 12}, {GPIOA, 11},
		 {GPIOA, 6}, {GPIOB, 12}, {GPIOA, 7}, {GPIOB, 11},
		 {GPIOB, 6},  {GPIOC, 7}, {GPIOB, 2}, {GPIOA, 9},
};




struct LEDblock all_block = {all_leds, 16,  ALL};


//blue LED block
struct Light_Emitting_Diode blue_leds[] = { //declaring the array of LEDS
		 {GPIOB, 6},  {GPIOC, 7},
		 {GPIOB, 2}, {GPIOA, 9},
};
struct LEDblock blue_block = {blue_leds, 4, B};


//red LED block
struct Light_Emitting_Diode red_leds[] = { //declaring the array of LEDS
		 {GPIOA, 6}, {GPIOB, 12},
		 {GPIOA, 7}, {GPIOB, 11},
};
struct LEDblock red_block = {red_leds, 4, R};


//yellow LED block
struct Light_Emitting_Diode yellow_leds[] = { //declaring the array of LEDS
		 {GPIOC, 5}, {GPIOB, 9},
		 {GPIOA, 12}, {GPIOA, 11},
};
struct LEDblock yellow_block = {yellow_leds, 4, Y};


//green LED block
struct Light_Emitting_Diode green_leds[] = { //declaring the array of LEDS
		 {GPIOC, 8},  {GPIOC, 9},
		 {GPIOC, 6}, {GPIOB, 8},
};
struct LEDblock green_block = {green_leds, 4, G};


//maps a color's ID to it's associated LED block
struct LEDblock* colorID_to_LEDblock[]={
	    [B]   = &blue_block,
		[R] = &red_block,
	    [Y] = &yellow_block,
		[G]  = &green_block
};


struct Light_Emitting_Diode STATUS_INDICATOR_LEDS[] = { //declaring the array of LEDS
		 {GPIOC, 12}, {GPIOC, 10},
};
struct LEDblock STATUS_INDICATOR_BLOCK = {STATUS_INDICATOR_LEDS, 2, UNASSIGNED_ID};

struct Light_Emitting_Diode SELECTED_PATTERN_INDICATOR_LEDS[] = { //declaring the array of LEDS
		 {GPIOB, 5}, {GPIOB, 13},
		 {GPIOB, 4}, {GPIOB, 14},
};

struct LEDblock SELECTED_PATTERN_INDICATOR_BLOCK = {SELECTED_PATTERN_INDICATOR_LEDS, 4, UNASSIGNED_ID};

struct Light_Emitting_Diode unused_leds[] = { {NULL, 0} };//unused for filling in functions/struct declarations
struct LEDblock unused_block = { //unused for filling in functions/struct declarations
    .leds = unused_leds,
    .count = 0,
    .ID = UNASSIGNED_ID
};


