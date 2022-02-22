/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "event_dispatcher.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "event_collector.h"
#include "system.h"

/*******************************************************
 *                Defines
 *******************************************************/


/*******************************************************
 *                Type Definitions
 *******************************************************/


/*******************************************************
 *                Globals
 *******************************************************/
static const char* TAG = "E_DIS";
static TaskHandle_t task_handle = NULL;

const static char* but_state_txt[4] = {"Press","Pressed","Release","Released"};

const system_params_t *sys_params;

/*******************************************************
 *                Function Declarations
 *******************************************************/
static void event_dispatcher_task( void * pvParameters );

void event_handler_button(uint8_t nr, uint8_t state);
void event_handler_midi_cc(uint8_t nr, uint8_t val);

/*******************************************************
 *                Private Function implementation
 *******************************************************/
static void event_dispatcher_task( void * pvParameters ) {
    
    event_t e;

    // Task main loop
    while(1) {
        if( get_event(&e) != 0) {
            if(e.type == EVENT_BUTTON) {
                // Event received
                ESP_LOGI(TAG, "Event received {Button, nr(%d),%s}!", e.data0, but_state_txt[e.data1]);
                event_handler_button(e.data0, e.data1);
            }
            else  if(e.type == EVENT_MIDI_CC) {
                // Event received
                ESP_LOGD(TAG, "Event received {MIDI CC, nr(%d), value(%d)!", e.data0, e.data1);
                event_handler_midi_cc(e.data0, e.data1);
            }
            else {
                ESP_LOGI(TAG, "Event received {%d,%d,%d,%d}!", e.type, e.data0, e.data1, e.data2);
            }
        }
    }
}

/*******************************************************
 *                Function implementation
 *******************************************************/

void event_dispatcher_init(void) {
    BaseType_t xReturned;
    ESP_LOGI(TAG, "Event Dispatcher init.");
    sys_params = system_get_param();

    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(event_dispatcher_task, "disp_task", 2048,0 , 1, &task_handle );
    if(xReturned != pdPASS) {
        ESP_LOGE(TAG, "Unable to create dispatcher task!");
    }
}


static void handle_button_press_preset(uint8_t nr) {
    ESP_LOGI(TAG, "Button Press Handler(%d)", nr);

    // toggle function

}


/* 
* Relay related functions
*/
static uint16_t relay_state;

void function_on(uint8_t nr) {
    relay_state |= (1 << nr);
    // relay_update(relay_state)
    // update_ui_function(nr, STATE_ON)
}
void function_off(uint8_t nr) {
    relay_state &= ~(1 << nr);
    // relay_update(relay_state)
    // update_ui_function(nr, STATE_OFF)
}
void amp_select(uint8_t nr) {

}

void amp_bypass(void) {

}
void function_toggle(uint8_t nr) {
    if(relay_state & (1 << nr)) {
        function_off(nr);
    }
    else {
        function_on(nr);
    }
}

/* 
* Preset related functions
*/

void preset_load(uint8_t nr) {
    // Update system mode to "preset"
    // Load from memory
    // Set current preset
    // Set relays
    // Send MIDI
    // Update UI
    // 
    // 
}
// Save current preset to location
void preset_save(uint8_t save_to) {
    // Set system mode to "preset"
    // write current settings to memory
}

void preset_preview(uint8_t nr) {
    // Set system state to "preview"
    // load shadow settings
    // update UI
}


#define MAX_BUTTONS (5)
#define MAX_STATES (4) // {PRESET, PREVIEW, EDIT}
typedef void (*button_handler_fn)(uint8_t);



static button_handler_fn button_fn_table_preset[MAX_BUTTONS][MAX_STATES] = {0};

void event_handler_button(uint8_t nr, uint8_t state) {
    button_handler_fn (*table)[MAX_BUTTONS][MAX_STATES];
    table = &button_fn_table_preset;
    button_fn_table_preset[0][0] = handle_button_press_preset;
    if(nr < MAX_BUTTONS && state < MAX_STATES) {
        // NULL pointer check
        if(table[nr][state] != 0) {
            (*table)[nr][state](nr);
        }
    }
    else {
        ESP_LOGW(TAG, "event_handler_button(%d, %d) fn==0!", nr, state);
    }
}

void event_handler_midi_cc(uint8_t nr, uint8_t val) {
    uint8_t func_nr = 0xFF;
    
    if(nr == sys_params->f0_cc) {
        func_nr = 0;
    }
    else if(nr == sys_params->f1_cc) {
        func_nr = 1;
    }
    else if(nr == sys_params->f2_cc) {
        func_nr = 2;
    }
    else if(nr == sys_params->f3_cc) {
        func_nr = 3;
    }
    else if(nr == sys_params->f4_cc) {
        func_nr = 4;
    }
    else if(nr == sys_params->f5_cc) {
        func_nr = 5;
    }
    else if(nr == sys_params->f6_cc) {
        func_nr = 6;
    }
    else if(nr == sys_params->f7_cc) {
        func_nr = 7;
    }
    else if(nr == sys_params->amp0_cc) {
        amp_select(0);
    }
    else if(nr == sys_params->amp1_cc) {
        amp_select(1);
    }
    else if(nr == sys_params->bypass_cc) {
        amp_bypass();
    }
    else {
        ESP_LOGI(TAG, "Unmapped CC (%d, %d)", nr, val);
    }

    // valid CC
    if(func_nr < 0xFF) {
        if(val > 0x3F) {
            function_on(func_nr);
        }
        else {
            function_off(func_nr);
        }
    }
}