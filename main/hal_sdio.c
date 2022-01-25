/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "esp_log.h"
#include "hal_sdio.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "hal_io_map.h"


/*******************************************************
 *                Globals
 *******************************************************/
gptimer_handle_t gptimer = NULL;
static const char* TAG = "E_COL";
static volatile uint32_t cnt = 0;
static volatile uint16_t button_in_tmp = 0;
static volatile uint16_t relay_out_tmp = 0;

// Output pins
#define GPIO_CLK    IO_SDIO_CLK
#define GPIO_SDO    IO_SDIO_SDO
#define GPIO_LATCH  IO_SDIO_LATCH
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_CLK) | (1ULL<<GPIO_SDO) | (1ULL<<GPIO_LATCH))
// Input pins
#define GPIO_SDI     IO_SDIO_SDI
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_SDI) 

#define SDIO_BIT_CNT    (16)

/*******************************************************
 *                Function Declarations
 *******************************************************/
static bool setup_timer(void);
static bool setup_gpio(void);

/*******************************************************
 *                Function implementation
 *******************************************************/


/*
* 165 latch in on falling edge
* 595 latch out on rising edge
*
*
*/

/**
 * @brief timer_callback used for serial IO
 *
 * @return true on success
 */
static bool IRAM_ATTR timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    // CLK update
    // toggle 

    gpio_set_level(IO_SDIO_CLK, cnt & 0x1 );

    // initial call
    if(cnt == (SDIO_BIT_CNT *2)) {
        // Set latch low
        gpio_set_level(IO_SDIO_LATCH, 0 );
    
    } 
    else if(cnt < (SDIO_BIT_CNT *2)) {
        if( (cnt & 0x1) == 0) { // Rising edge of CLK
            // 595 shift in on rising edge
            // 165 shift on rising edge
            
        }
        else { // Faling edge of CLK
            // sample on falling edge
            button_in_tmp |= gpio_get_level(GPIO_SDI) & 0x0001;
            button_in_tmp = ( button_in_tmp << 1) & 0xFFFE;
            // send out on falling edge
            gpio_set_level(IO_SDIO_SDO, (relay_out_tmp & 0x0001 ) );
            relay_out_tmp = relay_out_tmp >> 1;
        }
    } 
    else if(cnt == 0) {
        // Set latch high
        gpio_set_level(IO_SDIO_LATCH, 1 );
        // stop timer immediately
        gptimer_stop(timer);
    
        // Set flag when done
    }
    
    cnt--;
    return 0;
}

/**
 * @brief Setup timer for serial IO
 *
 * @return true on success
 */
static bool setup_timer(void) {
    // setup timer
    ESP_LOGI(TAG, "Create timer handle");

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_APB,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    // Register callback
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, 0));

    return true;
}

/**
 * @brief Setup GPIO serial IO
 *
 * @return true on success
 */
static bool setup_gpio(void) {
    // Setup output
    gpio_config_t io_conf_o = {};

    io_conf_o.intr_type = GPIO_INTR_DISABLE;
    io_conf_o.mode = GPIO_MODE_OUTPUT;
    io_conf_o.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf_o.pull_down_en = 0;
    io_conf_o.pull_up_en = 0;
    gpio_config(&io_conf_o);
    
    // Setup input
    gpio_config_t io_conf_i = {};

    io_conf_i.intr_type = GPIO_INTR_DISABLE;
    io_conf_i.mode = GPIO_MODE_INPUT;
    io_conf_i.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf_i.pull_down_en = 0;
    io_conf_i.pull_up_en = 1;
    gpio_config(&io_conf_i);
    
    return true;
}

/**
 * @brief Setup hal layer for SDIO
 *
 * @return ESP_OK on success
 */
esp_err_t hal_sdio_setup(void) {
    setup_gpio();
    setup_timer();

    return 0;
}