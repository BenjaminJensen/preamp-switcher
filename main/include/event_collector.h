#pragma once

/*******************************************************
 *                Macros
 *******************************************************/


/*******************************************************
 *                Type Definitions
 *******************************************************/
enum button_state_e {
    BUTTON_PRESS_E = 0,
    BUTTON_PRESSED_E = 1,
    BUTTON_RELEASE_E = 2,
    BUTTON_RELEASED_E = 3
};

enum event_types_e {EVENT_BUTTON = 0, EVENT_ENCODER = 1, EVENT_MIDI_PC = 2, EVENT_MIDI_CC = 3};

typedef struct {
    uint8_t type;
    uint8_t data0;
    uint8_t data1;
    uint8_t data2;
} event_t;

/*******************************************************
 *                Function Declarations
 *******************************************************/

/**
 * @brief Initializes the Event Collector
 *
 * @return ESP_OK on success
 */

void event_collector_init(void);

/**
 * @brief Send a button event to the event system
 * @param nr the button number
 * @param state the state of the button "button_state_e"
 * @return ESP_OK on success
 */
void event_send_button(uint8_t nr, enum button_state_e state);


void event_send_midi_cc(uint8_t nr, uint8_t val);

void event_send_midi_pc(uint8_t nr);

/**
 * @brief Get an event from the system
 * @param e OUT pointer to a generic event struct
 * @return 1 if an event was cobyed to the data structure supplied
 */
uint8_t get_event(event_t *e);