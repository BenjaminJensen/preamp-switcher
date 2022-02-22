/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "esp_log.h"
#include "midi.h"
#include "event_collector.h"



/*******************************************************
 *                Types
 *******************************************************/
typedef struct {
    uint8_t state;
    uint8_t status_byte;
    uint8_t channel;
    uint8_t bytes_to_receive;
    uint8_t bytes_received;
} midi_handle_t;

/*******************************************************
 *                Globals
 *******************************************************/
midi_handle_t m_handle;
static const char* TAG = "MIDI";
static midi_handler_fn midi_handler[7] = {0};

/*******************************************************
 *                Function Declarations
 *******************************************************/
static void midi_handle_data(uint8_t b, midi_handle_t *handle);

/*******************************************************
 *                Function implementation
 *******************************************************/

static uint8_t num_bytes[7] = {2,2,2,2,2,1,2};
/*
*
*/
void midi_process(uint8_t *data, int len) {
    midi_handle_t * handle = &m_handle;

    for(int i = 0; i < len; i++) {
        uint8_t b = data[i];
        
        if(b >= 0xF0) {
            // System message
        } else if(b & 0x80) {
            uint8_t chan = b & 0x0F;
            if(chan == handle->channel || handle->channel > 0x0F) {
                uint8_t type = (b >> 4) & 0x07;
                handle->status_byte = b;
                handle->bytes_to_receive = num_bytes[type];
                handle->bytes_received = 0;
            }
        }
        else if(handle->status_byte >= 0x80) {
            // Data message
            midi_handle_data(b, handle);
        }
    }
}

uint8_t midi_buffer[32];
/*
*
*/
static void midi_handle_data(uint8_t b, midi_handle_t *handle) {

    if(handle->bytes_received >= 32) {
        ESP_LOGW(TAG, "MIDI receive buffer full!");
    }
    else if(handle->status_byte != 0) { 
        midi_buffer[handle->bytes_received] = b;
        handle->bytes_received++;
        if(handle->bytes_to_receive == handle->bytes_received) {
            if(handle->status_byte >= 0xF0) {
                // System 
                ESP_LOGI(TAG, "System message Message: %x", handle->status_byte);
            } else if(handle->status_byte >= 0x80) {
                // Command message received
                uint8_t p_idx = (handle->status_byte >> 4) & 0x07;
                if(midi_handler[p_idx] != 0) {
                    // call handler function
                    midi_handler[p_idx](midi_buffer, num_bytes[p_idx]);
                }
                else {
                    ESP_LOGW(TAG, "No handler for Command Message: %x", handle->status_byte);
                }
                // reset
                handle->status_byte = 0;
            }
            else {
                // Something wrong
                ESP_LOGW(TAG, "Unknown status byte: %x", handle->status_byte);
            }
        }
    }
    else {
        ESP_LOGI(TAG, "Data received, no status byte set");
    }            
}

void midi_register_handler(midi_handler_fn handle, uint8_t status) {
    uint8_t p_idx = (status >> 4) & 0x07;
    midi_handler[p_idx] = handle;
}

/*
*
*
static void midi_handle_system(uint8_t b, midi_handle_t *handle) {
    switch(b) {
        case 0x80: // Note-off
            break;
        case 0x90: // Note-on
            break;
        case 0xA0: // Aftertouch
            break;
        case 0xB0: // Continous Control
            break;
        case 0xC0: // Patch Change
            break;
        case 0xD0: // Channel Pressure
            break;
        case 0xE0: // Pitch Bend
            break;
        case 0xF0: // Start of SYSEX
            break;
        case 0xF1: // MIDI Time Code Quarter Frame
            break;
        case 0xF2: // Song Position Pointer
            break;
        case 0xF3: // Song Select 
            break;
        case 0xF4: // Unused
            break;
        case 0xF5: // Unused
            break;
        case 0xF6: // Tune Request
            break;
        case 0xF7: // End of SYSEX
            break;
        case 0xF8: // Timing Clock
            break;
        case 0xF9: //  
            break;
        case 0xFA: // Start 
            break;
        case 0xFB: // Continue
            break;
        case 0xFC: // Stop 
            break;
        case 0xFD: // Unused
            break;
        case 0xFE: // Active Sensing
            break;
        case 0xFF: // System Reset
            break;
        default:
            ESP_LOGW(TAG, "Unknown System Message %d", b);
            break;
    }
}
*/