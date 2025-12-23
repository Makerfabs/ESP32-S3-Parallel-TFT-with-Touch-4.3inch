#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/jpeg_decode.h"


#include "file_iterator.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "lv_demos.h"

#include "board.h"

#include "pcf8563.h"

static char *TAG = "template";



// @brief image switch
static lv_obj_t *img_obj = NULL;
static lv_image_dsc_t img_dsc;
static uint8_t *current_img_data = NULL; 
static file_iterator_instance_t *file_iterator = NULL; // 文件迭代器句柄
static int img_index = 0;
static int img_count = 0;

static jpeg_decoder_handle_t jpgd_handle;
static char img_dir_path[256];

// @brief pcf85063a 
static lv_obj_t *ui_time_label = NULL;
static lv_obj_t *ui_date_label = NULL;



/* =============================================================================================================
                                                image switch
    ============================================================================================================ */

static char* image_switch_get_path(int index)
{
    const char *image_name = file_iterator_get_name_from_index(file_iterator,index);

    static char image_path[256];
    // file_iterator_get_full_path_from_index(file_iterator,index,image_path,256);
    snprintf(image_path, sizeof(image_path), "A:/images/%s", image_name);

    return image_path;
}

static void image_switch_rb()
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if(dir != LV_DIR_LEFT && dir != LV_DIR_RIGHT) return; // only handle left/right

    int new_index = img_index + (dir == LV_DIR_LEFT ? 1 : -1);
    if(new_index < 0) new_index = img_count - 1;
    if(new_index >= img_count) new_index = 0;

    img_index = new_index;

    ESP_LOGI(TAG, "%s", image_switch_get_path(img_index));

    // Load image from SD card
    lvgl_port_lock(0);
    lv_img_set_src(img_obj, image_switch_get_path(img_index));
    lvgl_port_unlock();
}

// 使用 LVGL 自带的编解码
void template_image_switch_lvgl(const char *image_dir_path)
{
    file_iterator = file_iterator_new(image_dir_path);
    assert(file_iterator);

    img_count = file_iterator_get_count(file_iterator);
    if (img_count) 
        ESP_LOGI(TAG, "image count: %d", img_count);
    else {
        ESP_LOGW(TAG, "no image found");
        return ;
    }

    lv_obj_add_event(lv_scr_act(), image_switch_rb, LV_EVENT_GESTURE, NULL);

    for(int i = 0; i < img_count; i++) {
        ESP_LOGI(TAG, "%s", image_switch_get_path(i));
    }

    lvgl_port_lock(0);
    img_obj = lv_img_create(lv_scr_act());
    lv_obj_center(img_obj);
    lv_image_set_src(img_obj, image_switch_get_path(img_index));
    lvgl_port_unlock();
}


/* =============================================================================================================
                                                LVGL Demos
    ============================================================================================================ */

void template_lvgl_demos_test()
{
    lvgl_port_lock(0);

    lv_demo_widgets();

    lvgl_port_unlock();
}


/* =============================================================================================================
                                            rtc
    ============================================================================================================ */

#if CONFIG_PCF85063A_ENABLE
static void clock_update_timer_cb(lv_timer_t *timer)
{
    if (board_handle->pcf85063a == NULL || board_handle->pcf85063a->get_time_data == NULL) {
        ESP_LOGW(TAG, "board_handle->pcf85063a is NULL");
        return;
    }

    pcf85063a_datetime_t time_data;
    
    // 调用您的 BSP 驱动获取时间
    // 注意：这里是在 LVGL 任务上下文中调用的。
    // 因为您的驱动使用了互斥锁 (xSemaphoreTake)，所以它是线程安全的。
    esp_err_t ret = board_handle->pcf85063a->get_time_data(&time_data);
    if (ret == ESP_OK) {
        // 更新时间显示 (HH:MM:SS)
        // 请根据您 pcf85063a.h 中实际的结构体成员名称修改下面的 .hours, .minutes 等
        if (ui_time_label) {
            lv_label_set_text_fmt(ui_time_label, "%02d:%02d:%02d", 
                                  time_data.hour, 
                                  time_data.min, 
                                  time_data.sec);
        }

        // 更新日期显示 (YYYY-MM-DD)
        if (ui_date_label) {
            lv_label_set_text_fmt(ui_date_label, "20%02d-%02d-%02d", 
                                  time_data.year, 
                                  time_data.month, 
                                  time_data.day);
        }
    } else {
        ESP_LOGW(TAG, "Failed to read RTC time");
    }
}

void template_pcf85063a_test()
{
    // 1. 获取当前活动屏幕
    lv_obj_t *scr = lv_screen_active();

    // 2. 创建一个容器来居中内容
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, 240, 160); // 根据您的屏幕大小调整
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    // 3. 创建时间 Label (大字体)
    ui_time_label = lv_label_create(cont);
    // 如果您有启用的字体，可以解开下面这行的注释
    // lv_obj_set_style_text_font(ui_time_label, &lv_font_montserrat_28, 0); 
    lv_label_set_text(ui_time_label, "00:00:00");

    // 4. 创建日期 Label (小字体)
    ui_date_label = lv_label_create(cont);
    lv_label_set_text(ui_date_label, "YYYY-MM-DD");

    // 5. 创建一个 LVGL 定时器，每 1000ms (1秒) 触发一次
    lv_timer_create(clock_update_timer_cb, 1000, NULL);

    // 立即手动调用一次以避免第一秒显示 "00:00:00"
    clock_update_timer_cb(NULL);
}

#endif // CONFIG_PCF85063A_ENABLE


#if CONFIG_PCF8563_ENABLE
void template_pcf8563_test()
{
    struct tm t;
    char time_str[32];
    if (board_handle->pcf8563->get_time(board_handle->pcf8563, &t) == ESP_OK) {
        ESP_LOGI("APP", "Time: %02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
    } else {
        ESP_LOGW("APP", "Failed to read RTC time");

    }
}
#endif // CONFIG_PCF8563_ENABLE