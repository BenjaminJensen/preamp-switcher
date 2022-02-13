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
    uint8_t bytes_left;
    uint8_t data[2];
} midi_handle_t;

/*******************************************************
 *                Globals
 *******************************************************/
midi_handle_t m_handle;
static const char* TAG = "MIDI";

/*******************************************************
 *                Function Declarations
 *******************************************************/
static void midi_handle_command(uint8_t b, midi_handle_t *handle);
static void midi_handle_system(uint8_t b, midi_handle_t *handle);
static void midi_handle_data(uint8_t b, midi_handle_t *handle);

/*******************************************************
 *                Function implementation
 *******************************************************/

/*
*
*/
void midi_process(uint8_t *data, int len) {
    midi_handle_t * handle = &m_handle;

    for(int i = 0; i < len; i++) {
        uint8_t b = data[i];
        
        if(b & 0x80) {
            uint8_t chan = b & 0x0F;
            if(chan == handle->channel || handle->channel > 0x0F) { // 0x0F = OMNI
                // System message
                if(b >= 0xF0) {
                    midi_handle_system(b, handle);
                }
                else {
                    // Command message
                    midi_handle_command(b, handle);
                }
            }
        }
        else {
            // Data message
            midi_handle_data(b, handle);
        }
    }
}

/*
*
*/
static void midi_handle_command(uint8_t b, midi_handle_t *handle) {
    uint8_t num_bytes[8] = {2,2,2,2,2,1,2,0};

    uint8_t type = (b >> 4) & 0x07;
    handle->bytes_left = num_bytes[type];
}

/*
*
*/
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

/*
*
*/
static void midi_handle_data(uint8_t b, midi_handle_t *handle) {
    handle->bytes_left--;
    handle->data[handle->bytes_left] = b;
    
    if(handle->bytes_left == 0) {
        switch(handle->status_byte) {
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
}