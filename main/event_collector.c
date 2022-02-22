/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "event_collector.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

/*******************************************************
 *                Defines
 *******************************************************/
#define EVENT_QUEUE_SIZE    (32)

/*******************************************************
 *                Globals
 *******************************************************/
static QueueHandle_t event_queue = 0;

// Description of FreeRTOS queue
// https://www.freertos.org/xQueueCreateStatic.html
uint8_t queue_store[EVENT_QUEUE_SIZE * sizeof(event_t)];
static StaticQueue_t xStaticQueue;

static const char* TAG = "E_COL";

/*******************************************************
 *                Function Declarations
 *******************************************************/
static void event_send(event_t e);

/*******************************************************
 *                Private Function implementation
 *******************************************************/

static void event_send(event_t e) {
    if(event_queue != 0) {
        if( xQueueSendToBack( event_queue, &e, ( TickType_t ) 10 ) != pdPASS ) {
            // Failed to post the message, even after 10 ticks. 
            ESP_LOGE(TAG, "To pass event {%d,%d,%d,%d}!", e.type, e.data0, e.data1, e.data2);
        }
    }
}

/*******************************************************
 *                Function implementation
 *******************************************************/

void event_collector_init(void) {
    event_queue = xQueueCreateStatic(32, sizeof(event_t), queue_store, &xStaticQueue );
    if(event_queue == 0) {
        // Something wrong
        ESP_LOGE(TAG, "Unable to create event queue!");
    }
}

void event_send_button(uint8_t nr, enum button_state_e state) {
    event_t e;
    e.type = EVENT_BUTTON;
    e.data0 = nr;
    e.data1 = state;
    event_send(e);
}


void event_send_midi_cc(uint8_t nr, uint8_t val) {
    event_t e;
    e.type = EVENT_MIDI_CC;
    e.data0 = nr;
    e.data1 = val;
    event_send(e);
}

void event_send_midi_pc(uint8_t nr) {
    event_t e;
    e.type = EVENT_MIDI_PC;
    e.data0 = nr;
    event_send(e);
}

uint8_t get_event(event_t *e) {
    BaseType_t res = pdFALSE;
    if( event_queue != 0) {
        res = xQueueReceive( event_queue, e, ( TickType_t ) 10 );
    }

    return (res == pdFALSE ? 0 : 1);
}