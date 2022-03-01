/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "actions.h"

/*******************************************************
 *                Types
 *******************************************************/

/*******************************************************
 *                Globals
 *******************************************************/


/*******************************************************
 *                Function Declarations
 *******************************************************/

/*******************************************************
 *                Function implementation
 *******************************************************/


/* 
* Relay related functions
*/
static uint16_t relay_state;

void function_on(uint8_t nr) {
    relay_state |= (1 << nr);
    // relay_update(relay_state)
    // update_ui_function(nr, STATE_ON)
}
void function_off(uint8_t nr) {
    relay_state &= ~(1 << nr);
    // relay_update(relay_state)
    // update_ui_function(nr, STATE_OFF)
}
void amp_select(uint8_t nr) {

}

void amp_bypass(void) {

}
void function_toggle(uint8_t nr) {
    if(relay_state & (1 << nr)) {
        function_off(nr);
    }
    else {
        function_on(nr);
    }
}

/* 
* Preset related functions
*/

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
