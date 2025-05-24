

#ifndef LEDS_H_
#define LEDS_H_
#include "main.h"
/**************************************************************************************************
* File: leds.h
* Header file for leds.c
*************************************************************************************************/
void configure_LEDS (GPIO_TypeDef *port, uint32_t pins[], uint32_t number_of_pins, uint32_t port_clock_num);
void TURN_ON_LED_BLOCK (struct LEDblock *LEDgroup);
void TURN_OFF_LED_BLOCK (struct LEDblock *LEDgroup);
void QUICK_TOGGLE_LED_BLOCK (struct LEDblock *LEDgroup);

//all LED block
extern struct Light_Emitting_Diode all_leds[];
extern struct LEDblock all_block;

//blue LED block
extern struct Light_Emitting_Diode blue_leds[];
extern struct LEDblock blue_block;

//red LED block
extern struct Light_Emitting_Diode red_leds[];
extern struct LEDblock red_block;

//yellow LED block
extern struct Light_Emitting_Diode yellow_leds[];
extern struct LEDblock yellow_block;

//green LED block
extern struct Light_Emitting_Diode green_leds[];
extern struct LEDblock green_block;

//maps a color's ID to it's associated LED block
extern struct LEDblock* colorID_to_LEDblock[];


extern struct Light_Emitting_Diode DIFFICULTY_INDICATOR_LEDS[];
extern struct Light_Emitting_Diode STATUS_INDICATOR_LEDS[];
extern struct LEDblock STATUS_INDICATOR_BLOCK ;




#endif /* LEDS_H_ */
