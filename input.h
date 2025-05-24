
#ifndef INPUT_H_
#define INPUT_H_

#include "main.h"
/**************************************************************************************************
* File: input.h
* Header file for input.c
*************************************************************************************************/


//create button structs
extern struct button BLUE_BUTTON;
extern struct button RED_BUTTON;
extern struct button YELLOW_BUTTON;
extern struct button GREEN_BUTTON;
extern struct button SPECIAL_BUTTON;

//function prototypes
void configure_external_switches(void);
void configure_Board_Button (void);
void INITIATE_DEBOUNCE_PROTOCOL(struct button *button, uint32_t currentTIME_ms);
void DEBOUNCE_PROTOCOL(struct button *button, uint32_t currentTIME_ms);
void HANDLE_DEBOUNCED_BUTTON(struct button *button,  uint32_t currentTIME_ms);
void SPECIAL_BUTTON_ACTIONS(struct button *button);
void BUTTON_ACTIONS(struct button *button, struct GAME_PATTERN *pattern, uint32_t currentTIME_ms);//regular button pressed

#endif /* INPUT_H_ */
