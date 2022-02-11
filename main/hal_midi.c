/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "esp_log.h"
#include "hal_midi.h"
#include "hal_io_map.h"
#include "driver/uart.h"
#include "driver/gpio.h"

/*******************************************************
 *                Types
 *******************************************************/

/*******************************************************
 *                Defines
 *******************************************************/
#define MIDI_UART_PORT      (UART_NUM_1) 
#define BUFFER_SIZE         (32)
#define MIDI_UART_BAUDRATE  (31250)
#define MIDI_BUF_SIZE       (256)

/*******************************************************
 *                Globals
 *******************************************************/
static const char* TAG = "HAL_MIDI";

/*******************************************************
 *                Function Declarations
 *******************************************************/
static void midi_task(void *arg);

/*******************************************************
 *                Function implementation
 *******************************************************/

void hal_midi_setup() {
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = MIDI_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_driver_install(MIDI_UART_PORT, MIDI_BUF_SIZE*2 , 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(MIDI_UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(MIDI_UART_PORT, 4, 5, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    xTaskCreate(midi_task, "midi_task", 4096, NULL, 10, NULL);
}

/*
*
*/
static void midi_task(void *arg) {
    uint8_t data[MIDI_BUF_SIZE];

    ESP_LOGI(TAG, "MIDI Task started");
    
    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(MIDI_UART_PORT, data, MIDI_BUF_SIZE, 1 / portTICK_RATE_MS);

        if(len > 0) {
            ESP_LOGI(TAG, "Received: %.*s", len, data);
            // Write data back to the UART
            uart_write_bytes(MIDI_UART_PORT, data, len);
        }
    }
}