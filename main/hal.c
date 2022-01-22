/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "hal.h"
#include "hal_sdio.h"

/*******************************************************
 *                Function Declarations
 *******************************************************/
void button_update(uint16_t buttons);

/*******************************************************
 *                Function implementation
 *******************************************************/
esp_err_t hal_setup(void) {
    hal_sdio_setup();
    return 0;
}

volatile uint16_t relay_state = 0;

void timer_task(void) {
    uint16_t buttons;
    buttons = sdio_update(relay_state);
    button_update(buttons);

}
