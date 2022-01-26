/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "esp_log.h"
#include "hal.h"
#include "hal_sdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*******************************************************
 *                Global Declarations
 *******************************************************/
static TaskHandle_t task_to_wake = NULL;
TaskHandle_t *sampling_task_handle;
static const char* TAG = "HAL";

/*******************************************************
 *                Function Declarations
 *******************************************************/
void button_update(uint16_t buttons);
static void input_sampling_task(void* args);
static void start_sampling( void );

/*******************************************************
 *                Function implementation
 *******************************************************/
esp_err_t hal_setup(void) {
    // Setup serial IO
    hal_sdio_setup();

    // Create input task
    xTaskCreate( input_sampling_task, "HAL_SAMPL", 2046, 0, 5, sampling_task_handle);

    return 0;
}

static void input_sampling_task(void* args) {
    uint32_t ulNotificationValue;
    TickType_t xLastWakeTime;
    // 20 ms sampling time
    const TickType_t xFrequency = pdMS_TO_TICKS( 20 );
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 200 );

    // Initialise the xLastWakeTime variable with the current time.
     xLastWakeTime = xTaskGetTickCount();
    
    while(1) {
        
        // Start sampling
        start_sampling();

        // Wait for sampling to be done
        ulNotificationValue = ulTaskNotifyTakeIndexed( 0, pdTRUE, xMaxBlockTime );
        
        if( ulNotificationValue == 1 )
        {
            // The transmission ended as expected.
        }
        else
        {
            // The call to ulTaskNotifyTake() timed out.
            // and the sampling never ended
        }

        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

static void start_sampling( void ) {
    // Veryfy that handle
    if(task_to_wake == NULL ) {
        // Store handle, to wake up the transmission is done
        task_to_wake = xTaskGetCurrentTaskHandle();

        //sdio_start_transmission( &task_to_wake );
    }
    else {
        ESP_LOGE(TAG, "Unable to start sampling, sampling under way {%d}", 0 );
    }    
}
