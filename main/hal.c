/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "esp_log.h"
#include "hal.h"
#include "hal_sdio.h"
#include "hal_midi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "button.h"
#include "encoder.h"
#include "midi.h"
#include "system.h"
#include "event_collector.h"

/*******************************************************
 *                Defines
 *******************************************************/
#define MIDI_BUF_SIZE       (256)

/*******************************************************
 *                Global Declarations
 *******************************************************/
static TaskHandle_t task_to_wake = NULL;
TaskHandle_t *sampling_task_handle;
static const char* TAG = "HAL";
static uint16_t relay_out = 0;

/*******************************************************
 *                Function Declarations
 *******************************************************/
void button_update(uint16_t buttons);
static void input_sampling_task(void* args);
static void start_sampling( void );

static void midi_task(void *arg);
static void midi_driver_init(void);

/*******************************************************
 *                Function implementation
 *******************************************************/
esp_err_t hal_setup(void) {
    // Setup serial IO
    hal_sdio_setup();
    
    // Setup midi hal layer
    hal_midi_setup(MIDI_BUF_SIZE);
    midi_driver_init();

    // Create input task
    xTaskCreate( input_sampling_task, "HAL_SAMPL", 2046, 0, 5, sampling_task_handle);

    xTaskCreate(midi_task, "midi_task", 4096, NULL, 10, NULL);
    return 0;
}

void hal_relay_set(uint16_t state) {
    ESP_LOGI(TAG, "relay set(%x)", state);
    relay_out = state;
}

static void input_sampling_task(void* args) {
    uint32_t ulNotificationValue;
    TickType_t xLastWakeTime;
    // 20 ms sampling time
    const TickType_t xFrequency = pdMS_TO_TICKS( 20 );
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 1000 );

    uint8_t buttons = 0;
    uint8_t buttons_last = 0;
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
            buttons = sdio_get_buttons();
            uint8_t enc = (buttons >> 6) & 0x03;
            //encoder_update(enc);

            buttons &= 0x1F;

            button_update(buttons);
            if(buttons != buttons_last) {
                buttons_last = buttons;
                ESP_LOGI(TAG, "Buttons update {%x}", buttons  );
                //printf("Leading text "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
            }
        }
        else
        {
            // The call to ulTaskNotifyTake() timed out.
            // and the sampling never ended
            ESP_LOGW(TAG, "Sample timeout" );
        }

        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}
uint16_t relay_cnt = 0;
static void start_sampling( void ) {
    // Veryfy that handle
    if(task_to_wake == NULL ) {
        // Store handle, to wake up the transmission is done
        task_to_wake = xTaskGetCurrentTaskHandle();
        //ESP_LOGI(TAG, "Start sampling {%d}", 0  );
        /*
        relay_cnt++;
        if(relay_cnt > 25) {
            relay_cnt = 0;
            if(relay_out == 0) {
                relay_out = 0x8000;
            }
            else if(relay_out < 0x0080) {
                relay_out = 0x8000;
            }
            else {
                relay_out = relay_out >> 1;
            }
        }
        */
        sdio_start_transmission( &task_to_wake, relay_out);
    }
    else {
        //uint32_t * cnt_reg = (uint32_t *)(0x3FF5F004);
        /*
        uint32_t *reload_low = (uint32_t *)(0x3FF5F018);
        uint32_t *alarm_low = (uint32_t *)(0x3FF5F010);
        uint32_t *config_reg = (uint32_t *)(0x3FF5F000);
        */
        //ESP_LOGI(TAG, "reload: {%d}, alarm: {%d}, config: {%x}", *reload_low, *alarm_low, *config_reg);
        ESP_LOGW(TAG, "Unable to start new sampling, sampling already underway!");
    }    
}

/****************************************************************************
 * -------------------- MIDI Section -------------------------------------- *
 * *************************************************************************/

const system_params_t *sys_params;
/*
*
*/
static void midi_task(void *arg) {
    uint8_t data[MIDI_BUF_SIZE];

    ESP_LOGI(TAG, "MIDI Task started");
    
    while (1) {
        // Read data from the UART
        int len = hal_midi_get_bytes(data, MIDI_BUF_SIZE,  1 / portTICK_RATE_MS);
        

        if(len > 0) {
            //ESP_LOGI(TAG, "Received: %.*s", len, data);

            midi_process(data, len);
            // Write data back to the UART
            hal_midi_put_bytes(data, len);
        }
    }
}

static void midi_driver_handle_cc(uint8_t *data, int len) {

    if(len != 2) {
        ESP_LOGW(TAG, "MIDI CC Data length error (%d)", len);
    }
    else {
        ESP_LOGD(TAG, "MIDI CC received {%d, %d}", data[0], data[1]);
        event_send_midi_cc(data[0], data[1]);
    }    
}

static void midi_driver_init(void) {
    sys_params = system_get_param();
    midi_register_handler(midi_driver_handle_cc, CC);
}