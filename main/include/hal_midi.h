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

/**
 * @brief Updates the button state machines
 *         The debounce state machine has 4 calls as the timing
 *          meaning than for every 4th call with same value state changes
 * @param but bitmask of current sampled button state
 * @return ESP_OK on success
 */
void hal_midi_setup(int midi_buf_size);

int hal_midi_get_bytes(uint8_t *, int, int);

int hal_midi_put_bytes(uint8_t *data, int len);
