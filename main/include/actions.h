#pragma once

/*******************************************************
 *                Macros
 *******************************************************/


/*******************************************************
 *                Type Definitions
 *******************************************************/


/*******************************************************
 *                Function Declarations
 *******************************************************/

/* 
* Relay related functions
*/

void function_on(uint8_t nr);
void function_off(uint8_t nr) ;
void amp_select(uint8_t nr);
void amp_bypass(void);
void function_toggle(uint8_t nr);
/* 
* Preset related functions
*/
/*
void preset_load(uint8_t nr);
void preset_save(uint8_t save_to);
void preset_preview(uint8_t nr);
*/