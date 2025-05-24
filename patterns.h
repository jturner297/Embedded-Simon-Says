
#ifndef PATTERNS_H_
#define PATTERNS_H_

#include "main.h"
/**************************************************************************************************
* File: patterns.h
* Header file for pattern.h
*************************************************************************************************/
//function prototypes
void bind_sequnece_to_pattern(struct GAME_PATTERN *pattern);
void initialize_patterns(void);
void DISPLAY_PATTERN (struct GAME_PATTERN *pattern);
void CANCEL_ACTIVE_PATTERN(struct GAME_PATTERN *pattern);




extern struct GAME_PATTERN SUPER_EASY_PATTERN;
extern struct GAME_PATTERN EASY_PATTERN;
extern struct GAME_PATTERN MEDIUM_PATTERN;
extern struct GAME_PATTERN HARD_PATTERN;
extern struct GAME_PATTERN IMPOSSIBLE_PATTERN;

// Array of available patterns
extern struct GAME_PATTERN* PATTERNS[];

#endif /* PATTERNS_H_ */
