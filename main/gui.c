/*******************************************************
 *                Includes
 *******************************************************/
#include "esp_system.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gui.h"
#include "lvgl.h"
#include "st7735.h"

/*******************************************************
 *                Defines
 *******************************************************/
#define LCD_H_RES (160)
#define LCD_V_RES (128)

#define TAG "GUI"
#define LVGL_TICK_PERIOD_MS    2

/*******************************************************
 *                Types
 *******************************************************/

/*******************************************************
 *                Globals
 *******************************************************/

/*******************************************************
 *                Local variables
 *******************************************************/
static lv_color_t *buf1 = NULL;
static lv_color_t *buf2 = NULL;
static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
static lv_disp_drv_t disp_drv;      // contains callback functions

/*******************************************************
 *                Function Declarations
 *******************************************************/
static void gui_test1(lv_obj_t *scr);
static void gui_test2(lv_obj_t *scr);
static void gui_task(void *pvParameter);
static void increase_lvgl_tick(void *arg);
static void gui_install_lv_tic_timer(void);

/*******************************************************
 *                Function implementation
 *******************************************************/

void gui_init() {
    /*
    // Init display driver
    st7735_init();

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    // alloc draw buffers used by LVGL
    // it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
    buf1 = heap_caps_malloc(LCD_H_RES * 40 * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    assert(buf1);
    buf2 = heap_caps_malloc(LCD_H_RES * 40 * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    assert(buf2);
    ESP_LOGI(TAG, "buf1@%p, buf2@%p", buf1, buf2);

    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LCD_H_RES * 40);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_H_RES;
    disp_drv.ver_res = LCD_V_RES;
    disp_drv.flush_cb = st7735_flush;
    disp_drv.draw_buf = &disp_buf;
    //disp_drv.user_data = panel_handle;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
    lv_obj_t *scr = lv_disp_get_scr_act(disp);

    //gui_install_lv_tic_timer();

    gui_test1(scr);
    */
    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
    //xTaskCreatePinnedToCore(gui_task, "gui", 4096*2, NULL, 10, NULL, 1);

}

static void increase_lvgl_tick(void *arg) {
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

static void gui_install_lv_tic_timer(void) {
    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));
}

static void gui_task(void *pvParameter) {


    int cnt = 0;
    int x = 0;

    while (1) {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        cnt++;
        if(cnt > 50) {
            cnt = 0;
            if(x == 0) {
                gui_test1(NULL);
                x = 1;
            }
            else {
                gui_test2(NULL);
                x = 0;
            }
        }

        lv_task_handler();
    }
}

/*****************************************
* Test code 
*****************************************/

static void gui_test2(lv_obj_t *scr) {

   /*Create an Arc*/
  lv_obj_t * arc = lv_arc_create(lv_scr_act());
  lv_arc_set_end_angle(arc, 200);
  lv_obj_set_size(arc, 100, 100);
  lv_obj_align(arc, LV_ALIGN_CENTER, 0, 0);
}

static void gui_test1(lv_obj_t *scr) {

   /*Create an Arc*/
  lv_obj_t * arc = lv_arc_create(lv_scr_act());
  lv_arc_set_end_angle(arc, 300);
  lv_obj_set_size(arc, 100, 100);
  lv_obj_align(arc, LV_ALIGN_CENTER, 0, 0);
}