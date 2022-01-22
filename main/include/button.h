#pragma once

/*******************************************************
 *                Macros
 *******************************************************/


/*******************************************************
 *                Type Definitions
 *******************************************************/
enum event_types_e {EVENT_BUTTON, EVENT_ENCODER, EVENT_MIDI};

/*******************************************************
 *                Function Declarations
 *******************************************************/

/**
 * @brief Updates the button state machines
 *         The debounce state machine has 4 calls as the timing
 *          meaning than for every 4th call with same value state changes
 * @param but bitmask of current sampled button state
 * @return ESP_OK on success
 */

void button_update(uint16_t but);