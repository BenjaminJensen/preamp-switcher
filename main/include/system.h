#pragma once

/*******************************************************
 *                Macros
 *******************************************************/


/*******************************************************
 *                Type Definitions
 *******************************************************/
typedef struct {
    uint8_t f0_cc;
    uint8_t f1_cc;
    uint8_t f2_cc;
    uint8_t f3_cc;
    uint8_t f4_cc;
    uint8_t f5_cc;
    uint8_t f6_cc;
    uint8_t f7_cc;
    uint8_t amp0_cc;
    uint8_t amp1_cc;
    uint8_t bypass_cc;
    uint8_t midi_channel;
} system_params_t;

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

const system_params_t* system_get_param(void);