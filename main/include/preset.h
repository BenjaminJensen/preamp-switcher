#pragma once

/*******************************************************
 *                Macros
 *******************************************************/


/*******************************************************
 *                Type Definitions
 *******************************************************/

typedef struct {
    uint16_t relay;
    uint16_t relay_mask;
    char name[16];
} preset_but_t;

typedef struct {
    uint16_t relay;
    char name[32];
    preset_but_t buttons [4];
} preset_t;

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

const preset_t* preset_load(uint16_t nr);
const preset_t* preset_save(uint16_t nr);
esp_err_t mem_init(void);