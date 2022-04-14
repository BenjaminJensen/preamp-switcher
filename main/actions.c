/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "esp_log.h"
#include "actions.h"
#include "hal.h"

/*******************************************************
 *                Types
 *******************************************************/

/*******************************************************
 *                Globals
 *******************************************************/
static const char* TAG = "ACTIONS";

// Map a function to a relay output
static uint16_t func_relay_map[16] = { 0x8000,0x4000,0x2000,0x1000,
                                0x0800,0x0400,0x0200,0x0100,
                                0x0080,0x0040,0x0020,0x0010,
                                0x0008,0x0004,0x0002,0x0001
                                };

/*******************************************************
 *                Function Declarations
 *******************************************************/

/*******************************************************
 *                Function implementation
 *******************************************************/


/* 
* Relay related functions
*/
static uint16_t relay_state = 0;

void function_on(uint8_t nr) {
    if(nr < 16) {
        relay_state |= func_relay_map[nr];
        hal_relay_set(relay_state);
    }
    else {
        ESP_LOGW(TAG, "function_on: nr out of range (%d)!", nr);
    }
    
    // update_ui_function(nr, STATE_ON)
}

void function_off(uint8_t nr) {
    if(nr < 16) {
        relay_state &= ~(func_relay_map[nr]);
        hal_relay_set(relay_state);
    }
    else {
        ESP_LOGW(TAG, "function_off: nr out of range (%d)!", nr);
    }
    // update_ui_function(nr, STATE_OFF)
}
void amp_select(uint8_t nr) {

}

void amp_bypass(void) {

}
void function_toggle(uint8_t nr) {
    if(relay_state & func_relay_map[nr]) {
        function_off(nr);
    }
    else {
        function_on(nr);
    }
}

/* 
* Preset related functions
*/
/*
void preset_load(uint8_t nr) {
    // Update system mode to "preset"
    // Load from memory
    // Set current preset
    // Set relays
    // Send MIDI
    // Update UI
    // 
    // 
}
// Save current preset to location
void preset_save(uint8_t save_to) {
    // Set system mode to "preset"
    // write current settings to memory
}

void preset_preview(uint8_t nr) {
    // Set system state to "preview"
    // load shadow settings
    // update UI
}
*/