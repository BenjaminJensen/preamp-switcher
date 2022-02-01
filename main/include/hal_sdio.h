#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*******************************************************
 *                Macros
 *******************************************************/


/*******************************************************
 *                Type Definitions
 *******************************************************/


/*******************************************************
 *                Function Declarations
 *******************************************************/

/**
 * @brief Initializes the HAL layer for buttons
 *
 * @return ESP_OK on success
 */

esp_err_t hal_sdio_setup(void);


/**
 * @brief Initiate SDIO transfer
 *
 * @return ESP_OK on success
 */
uint32_t* sdio_start_transmission(TaskHandle_t*);