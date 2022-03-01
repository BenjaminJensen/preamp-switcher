/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "esp_log.h"
#include "system.h"

/*******************************************************
 *                Types
 *******************************************************/


/*******************************************************
 *                Globals
 *******************************************************/
static const char* TAG = "SYS";
static system_params_t sys_params;

/*******************************************************
 *                Function Declarations
 *******************************************************/

/*******************************************************
 *                Function implementation
 *******************************************************/

const system_params_t* system_get_param(void) {
    
    sys_params.f0_cc = 1;
    sys_params.f0_cc = 2;
    sys_params.f0_cc = 3;
    sys_params.f0_cc = 4;
    sys_params.f0_cc = 5;
    sys_params.f0_cc = 6;
    sys_params.f0_cc = 7;
    sys_params.amp0_cc = 8;
    sys_params.amp1_cc = 9;
    sys_params.bypass_cc = 10;
    sys_params.midi_channel = 15;
    return &sys_params;
}