/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "esp_log.h"
#include "preset.h"
#include <string.h>
// ---- mem ----
#include "esp_spiffs.h"

/*******************************************************
 *                Types
 *******************************************************/


/*******************************************************
 *                Globals
 *******************************************************/
static const char* TAG = "PRESET";

static preset_t current_preset;

/*******************************************************
 *                Function Declarations
 *******************************************************/
static int preset_load_mem(uint16_t nr, preset_t* p);
static int preset_save_mem(uint16_t nr, preset_t* p);

/*******************************************************
 *                Function implementation
 *******************************************************/

const preset_t* preset_load(uint16_t nr) {
    // Load preset from FLASH
    preset_load_mem(nr, &current_preset);

    // Save preset in RAM
    // Set relay state
    // Update display
    // Update system state
    ESP_LOGI(TAG, "Preset loaded: %s", current_preset.name);
    return &current_preset;
}

const preset_t* preset_save(uint16_t nr) {
    // Load preset from FLASH
    strcpy(current_preset.name, "Preset 1");
    preset_save_mem(nr, &current_preset);

    // Save preset in RAM
    // Set relay state
    // Update display
    // Update system state

    return &current_preset;
}

/*
* Initialize SPIFFS file system
*/
esp_err_t mem_init() {
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
    }
    return ret;
}

/*
* Save preset to SPIFFS file system
*/
static int preset_save_mem(uint16_t nr, preset_t* p) {
    FILE* f = fopen("/spiffs/presets.bin", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

    size_t read = fwrite(p, sizeof(p), 1, f);
    fclose(f);

    if(read != 1) {
        ESP_LOGW(TAG, "preset_save_mem: unable to save preset!");
    }
    else {
        ESP_LOGI(TAG, "preset %d saved", nr);
    }

    return read;
}

/*
* Load preset from SPIFFS file system
*/
static int preset_load_mem(uint16_t nr, preset_t* p) {
    FILE* f = fopen("/spiffs/presets.bin", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }

    size_t read = fread(p, sizeof(p), 1, f);
    fclose(f);

    if(read != 1) {
        ESP_LOGW(TAG, "preset_load_mem: unable to read preset!");
    }
    else {
        ESP_LOGI(TAG, "preset %d loaded", nr);
    }

    return read;
}