/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "event_dispatcher.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "event_collector.h"

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

/*******************************************************
 *                Function Declarations
 *******************************************************/
static void event_dispatcher_task( void * pvParameters );

const static char* but_state_txt[4] = {"Press","Pressed","Release","Released"};
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
    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(event_dispatcher_task, "disp_task", 2048,0 , 1, &task_handle );
    if(xReturned != pdPASS) {
        ESP_LOGE(TAG, "Unable to create dispatcher task!");
    }
}

