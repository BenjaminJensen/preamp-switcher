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

void hal_midi_setup(int midi_buf_size) {
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

    ESP_ERROR_CHECK(uart_driver_install(MIDI_UART_PORT, midi_buf_size*2 , 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(MIDI_UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(MIDI_UART_PORT, 4, 5, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

}


int hal_midi_get_bytes(uint8_t *data, int max_buf_size, int wait) {
    int len = 0;
    len = uart_read_bytes(MIDI_UART_PORT, data, max_buf_size, wait);

    return len;
}

int hal_midi_put_bytes(uint8_t *data, int len) {
    return uart_write_bytes(MIDI_UART_PORT, data, len);
}