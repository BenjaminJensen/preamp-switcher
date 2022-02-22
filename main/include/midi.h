#pragma once

/*******************************************************
 *                Macros
 *******************************************************/


/*******************************************************
 *                Type Definitions
 *******************************************************/
typedef void (*midi_handler_fn)(uint8_t*, int);

enum midi_status_e {PC = 0xC0, CC = 0xB0};

/*******************************************************
 *                Function Declarations
 *******************************************************/

/**
 * @brief Processes a stream of midi data ´
 *          and calls handlers for received messages
 * @param data pointer to received data for processing
 * @param len length of data array
 * @return none
 */
void midi_process(uint8_t *data, int len);
void midi_register_handler(midi_handler_fn handle, uint8_t status);

