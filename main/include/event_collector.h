#pragma once

/*******************************************************
 *                Macros
 *******************************************************/


/*******************************************************
 *                Type Definitions
 *******************************************************/
enum button_state_e {
    BUTTON_PRESS_E,
    BUTTON_PRESSED_E,
    BUTTON_RELEASE_E,
    BUTTON_RELEASED_E
};

/*******************************************************
 *                Function Declarations
 *******************************************************/

/**
 * @brief Initializes the Event Collector
 *
 * @return ESP_OK on success
 */

void event_collector_init(void);

void event_button_send(uint8_t nr, enum button_state_e state);