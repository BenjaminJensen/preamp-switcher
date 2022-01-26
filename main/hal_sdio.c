/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "esp_log.h"
#include "hal_sdio.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "hal_io_map.h"


/*******************************************************
 *                Defines
 *******************************************************/
// Timer 
#define TIMER_DIVIDER         (16)  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds
#define TIMER_RELOAD            (5000)
#define TIMER_NR    (TIMER_0)
#define TIMER_GROUP (TIMER_GROUP_0)

// Output pins
#define GPIO_CLK    IO_SDIO_CLK
#define GPIO_SDO    IO_SDIO_SDO
#define GPIO_LATCH  IO_SDIO_LATCH
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_CLK) | (1ULL<<GPIO_SDO) | (1ULL<<GPIO_LATCH))
// Input pins
#define GPIO_SDI     IO_SDIO_SDI
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_SDI) 

// SDIO
#define SDIO_BIT_CNT    (16)

/*******************************************************
 *                Types
 *******************************************************/
typedef struct {
    uint32_t timer_group;
    uint32_t timer_idx;
    uint32_t interval;
    bool auto_reload;
    TaskHandle_t* task_to_wake;
} timer_info_t;

/*******************************************************
 *                Globals
 *******************************************************/
static const char* TAG = "E_COL";
static volatile uint32_t cnt = 0;
static volatile uint16_t button_in_tmp = 0;
static volatile uint16_t relay_out_tmp = 0;

timer_info_t timer_handle = {0};

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
static bool IRAM_ATTR timer_group_isr_callback(void* args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // CLK update
    // toggle 

    timer_info_t *info = args;

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
        timer_pause(TIMER_GROUP, TIMER_NR);
    
        // Set flag when done
        configASSERT( timer_handle.task_to_wake != NULL );

        // Notify the task that the transmission is complete.
        vTaskNotifyGiveIndexedFromISR( timer_handle.task_to_wake, 
                                        0, 
                                        &xHigherPriorityTaskWoken );

        // There are no transmissions in progress, so no tasks to notify. 
        timer_handle.task_to_wake = NULL;

        // Sampling task wakes
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
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

    // Setup timer handle
    timer_handle.timer_group = TIMER_GROUP;
    timer_handle.timer_idx = TIMER_NR;
    timer_handle.auto_reload = true;
    timer_handle.interval = 0;

     /* Select and initialize basic parameters of the timer */
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = 1,
    }; // default clock source is APB
    timer_init(timer_handle.timer_group, timer_handle.timer_idx, &config);

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(timer_handle.timer_group, timer_handle.timer_idx, 0);

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(timer_handle.timer_group, 
                        timer_handle.timer_idx, 
                        TIMER_RELOAD);
    timer_enable_intr(timer_handle.timer_group, timer_handle.timer_idx);

   
    timer_isr_callback_add(timer_handle.timer_group, 
                            timer_handle.timer_idx, 
                            timer_group_isr_callback,
                             &timer_handle, 
                             0);

    

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
    ESP_LOGI(TAG, "Setup HAL layer SDIO");
    setup_gpio();
    setup_timer();

    return 0;
}

void sdio_start_transmission(TaskHandle_t* task_to_wake) {
    // Save task to wake when transmission is done
    timer_handle.task_to_wake = task_to_wake;

    cnt = 123;//timer_state_cnt;
    //Start timer
    timer_set_auto_reload(timer_handle.timer_group, timer_handle.timer_idx, 1);
    timer_start(timer_handle.timer_group, timer_handle.timer_idx);
}