/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "button.h"
#include "event_collector.h"

#define BUTTON_PUSH     (0xF0)
#define BUTTON_PUSHED   (0xF8)
#define BUTTON_RELEASE  (0x0F)
#define BUTTON_RELEASED (0x1F)
#define MAX_BUTTONS     (0x05)

/*******************************************************
 *                Types
 *******************************************************/
typedef struct {
    uint8_t state;
    uint8_t state_cnt;
    uint8_t nr;
} button_state_t;

/*******************************************************
 *                Globals
 *******************************************************/
volatile button_state_t buttons[MAX_BUTTONS] = {0};

/*******************************************************
 *                Function Declarations
 *******************************************************/

/*******************************************************
 *                Function implementation
 *******************************************************/
void button_update(uint16_t but) {
    uint8_t new_state;

    // Run through buttons
    for(int i = 0; i < MAX_BUTTONS; i++ ) {
        buttons[i].nr = i;
        // Decide on new state
        if(but & (1 << i)) {
            new_state = (buttons[i].state >> 1) | 0x80;
        }
        else {
            new_state = (buttons[i].state >> 1) & 0x7F;
        }
        // update state?
        if(new_state != buttons[i].state) {
            if(new_state == BUTTON_PUSH) {
                // if button is pushed
                event_send_button(buttons[i].nr, BUTTON_PRESS_E);
            }
            else if(new_state == BUTTON_PUSHED) {
                // If buttos have been pushed for some time
                event_send_button(buttons[i].nr, BUTTON_PRESSED_E);
            }
            else if(new_state  == BUTTON_RELEASED) {
                // If button have been released
                event_send_button(buttons[i].nr, BUTTON_RELEASED_E);
            }
            else if(new_state == BUTTON_RELEASE) {
                // Button released
                event_send_button(buttons[i].nr, BUTTON_RELEASE_E);
            }
            else {
                // Something wrong
            }
            // Update state
            buttons[i].state = new_state;
            buttons[i].state_cnt++;
        }
    }
}