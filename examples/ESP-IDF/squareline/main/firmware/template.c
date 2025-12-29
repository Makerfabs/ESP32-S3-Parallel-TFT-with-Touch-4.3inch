#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_check.h>
#include <driver/jpeg_decode.h>
#include <math.h>

#include "file_iterator.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "lv_demos.h"

#include "board.h"
#include "dht.h"
#include "ui.h"
#include "mpu6050.h"

#include "driver/i2c_master.h"


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


// @brief lvgl demos
#define COLOR_BG        lv_color_hex(0x0a0e27)
#define COLOR_CARD_BG   lv_color_hex(0x1a1f3a)
#define COLOR_PRIMARY   lv_color_hex(0x6366f1)
#define COLOR_SECONDARY lv_color_hex(0x8b5cf6)
#define COLOR_ACCENT    lv_color_hex(0xec4899)
#define COLOR_SUCCESS   lv_color_hex(0x10b981)
#define COLOR_WARNING   lv_color_hex(0xf59e0b)
#define COLOR_TEXT      lv_color_hex(0xe5e7eb)
#define COLOR_TEXT_SEC  lv_color_hex(0x9ca3af)

static lv_style_t style_screen;
static lv_style_t style_card;
static lv_style_t style_card_hover;
static lv_style_t style_title;
static lv_style_t style_button;
static lv_style_t style_button_pressed;

static lv_obj_t *lbl_temperature_value = NULL;
static lv_obj_t *lbl_humidity_value = NULL;

// 动画时间
#define ANIM_TIME 250

// @brief dht11



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

void ui_init_styles(void) {
    // 屏幕背景 - 深色渐变
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, COLOR_BG);
    lv_style_set_bg_grad_color(&style_screen, lv_color_hex(0x1a1f3a));
    lv_style_set_bg_grad_dir(&style_screen, LV_GRAD_DIR_VER);
    
    // 卡片 - 玻璃态效果
    lv_style_init(&style_card);
    lv_style_set_radius(&style_card, 20);
    lv_style_set_bg_color(&style_card, COLOR_CARD_BG);
    lv_style_set_bg_opa(&style_card, LV_OPA_70);
    lv_style_set_border_width(&style_card, 1);
    lv_style_set_border_color(&style_card, lv_color_hex(0x374151));
    lv_style_set_border_opa(&style_card, LV_OPA_40);
    lv_style_set_shadow_width(&style_card, 20);
    lv_style_set_shadow_color(&style_card, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&style_card, LV_OPA_40);
    lv_style_set_pad_all(&style_card, 20);
    
    // 标题
    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, COLOR_TEXT);
}

// 创建卡片（带淡入动画）
lv_obj_t* create_card(lv_obj_t *parent, int x, int y, int w, int h) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_size(card, w, h);
    lv_obj_set_pos(card, x, y);
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    
    return card;
}

// 创建状态卡片
lv_obj_t* create_status_card(lv_obj_t *parent, const char *title, 
                        const char *value, const char *unit,
                        lv_color_t color) {
    lv_obj_t *card = create_card(parent, 0, 0, 180, 140);
    
    // 顶部彩色条
    lv_obj_t *bar = lv_obj_create(card);
    lv_obj_set_size(bar, 140, 4);
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_radius(bar, 2, 0);
    lv_obj_set_style_bg_color(bar, color, 0);
    lv_obj_set_style_border_width(bar, 0, 0);
    lv_obj_set_style_shadow_width(bar, 0, 0);
    
    // 标题
    lv_obj_t *lbl_title = lv_label_create(card);
    lv_label_set_text(lbl_title, title);
    lv_obj_set_style_text_color(lbl_title, COLOR_TEXT_SEC, 0);
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 15);
    
    // 数值
    lv_obj_t *lbl_value = lv_label_create(card);
    lv_label_set_text(lbl_value, value);
    lv_obj_set_style_text_color(lbl_value, COLOR_TEXT, 0);
    lv_obj_align(lbl_value, LV_ALIGN_CENTER, 0, 5);
    
    // 单位
    lv_obj_t *lbl_unit = lv_label_create(card);
    lv_label_set_text(lbl_unit, unit);
    lv_obj_set_style_text_color(lbl_unit, color, 0);
    lv_obj_align_to(lbl_unit, lbl_value, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    return lbl_value;
}

void create_icon_btn(lv_obj_t *parent, const char *icon, lv_color_t color) {
    lv_obj_t *btn = lv_obj_create(parent);
    lv_obj_set_size(btn, 70, 70);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(btn, color, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_20, 0);
    lv_obj_set_style_border_width(btn, 2, 0);
    lv_obj_set_style_border_color(btn, color, 0);
    lv_obj_set_style_border_opa(btn, LV_OPA_50, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, icon);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_center(label);
}

void create_main_ui(void) {
    lv_obj_t *screen = lv_screen_active();
    lv_obj_add_style(screen, &style_screen, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    
    // ========== 顶部栏 ==========
    lv_obj_t *header = lv_obj_create(screen);
    lv_obj_set_size(header, 800, 60);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_shadow_width(header, 0, 0);
    
    lv_obj_t *lbl_time = lv_label_create(header);
    lv_label_set_text(lbl_time, "14:23");
    lv_obj_add_style(lbl_time, &style_title, 0);
    lv_obj_align(lbl_time, LV_ALIGN_LEFT_MID, 20, 0);
    
    lv_obj_t *lbl_title = lv_label_create(header);
    lv_label_set_text(lbl_title, "Smart Home");
    lv_obj_set_style_text_color(lbl_title, COLOR_TEXT, 0);
    lv_obj_align(lbl_title, LV_ALIGN_CENTER, 0, 0);
    
    lv_obj_t *lbl_wifi = lv_label_create(header);
    lv_label_set_text(lbl_wifi, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(lbl_wifi, COLOR_SUCCESS, 0);
    lv_obj_align(lbl_wifi, LV_ALIGN_RIGHT_MID, -20, 0);
    
    // ========== 状态卡片区域 ==========
    lv_obj_t *status_row = lv_obj_create(screen);
    lv_obj_set_size(status_row, 760, 150);
    lv_obj_set_pos(status_row, 20, 75);
    lv_obj_set_style_bg_opa(status_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(status_row, 0, 0);
    lv_obj_set_style_pad_all(status_row, 0, 0);
    lv_obj_set_style_pad_column(status_row, 10, 0);
    lv_obj_clear_flag(status_row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(status_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_row, LV_FLEX_ALIGN_SPACE_BETWEEN, 
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    
    lbl_temperature_value = create_status_card(status_row, "Temperature", "24", "°C", COLOR_WARNING);
    lbl_humidity_value = create_status_card(status_row, "Humidity", "65", "%", COLOR_PRIMARY);
    create_status_card(status_row, "Air Quality", "Good", "", COLOR_SUCCESS);
    create_status_card(status_row, "PM2.5", "12", "ug/m3", COLOR_ACCENT);
    
    // ========== 设备控制卡片 ==========
    lv_obj_t *ctrl_card = create_card(screen, 20, 240, 760, 220);
    
    lv_obj_t *ctrl_title = lv_label_create(ctrl_card);
    lv_label_set_text(ctrl_title, "Device Control");
    lv_obj_set_style_text_color(ctrl_title, COLOR_TEXT, 0);
    lv_obj_align(ctrl_title, LV_ALIGN_TOP_LEFT, 0, 0);
    
    // 设备图标容器
    lv_obj_t *icon_row = lv_obj_create(ctrl_card);
    lv_obj_set_size(icon_row, 720, 120);
    lv_obj_align(icon_row, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_bg_opa(icon_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(icon_row, 0, 0);
    lv_obj_set_style_pad_all(icon_row, 10, 0);
    lv_obj_set_style_pad_column(icon_row, 15, 0);
    lv_obj_clear_flag(icon_row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(icon_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(icon_row, LV_FLEX_ALIGN_SPACE_EVENLY, 
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    create_icon_btn(icon_row, LV_SYMBOL_HOME, COLOR_PRIMARY);
    create_icon_btn(icon_row, LV_SYMBOL_CHARGE, COLOR_SUCCESS);
    create_icon_btn(icon_row, LV_SYMBOL_IMAGE, COLOR_WARNING);
    create_icon_btn(icon_row, LV_SYMBOL_AUDIO, COLOR_ACCENT);
    create_icon_btn(icon_row, LV_SYMBOL_BELL, COLOR_SECONDARY);
    create_icon_btn(icon_row, LV_SYMBOL_SETTINGS, COLOR_TEXT_SEC);
}



void template_lvgl_demos_test()
{
    lvgl_port_lock(0);

    // lv_demo_widgets();
    // ui_init_styles();

    // create_main_ui();

    // UI created using Squareline Studio
    ui_init();

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


/* =============================================================================================================
                                                sensor
    1. dht11                                         
    ============================================================================================================ */
void template_dht11_test()
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT_OD,
        .pin_bit_mask = (1ULL << GPIO_NUM_20),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&io_conf);

    int16_t humidity = 0;
    int16_t temperature = 0;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(2000));

        if (dht_read_data(DHT_TYPE_DHT11, GPIO_NUM_20, &humidity, &temperature) == ESP_OK) {
            lvgl_port_lock(0);

            lv_obj_t * label_temp = ui_comp_get_child(ui_card, UI_COMP_CARD_LBLTEMP);
            lv_label_set_text_fmt(label_temp, "%d.%d", temperature / 10, temperature % 10);
            lv_obj_t * label_humi = ui_comp_get_child(ui_card1, UI_COMP_CARD_LBLTEMP);
            lv_label_set_text_fmt(label_humi, "%d.%d", humidity / 10, humidity % 10);


            // if (lbl_temperature_value) {
            //     lv_label_set_text_fmt(lbl_temperature_value, "%d.%d", temperature / 10, temperature % 10);
            // }
            // if (lbl_humidity_value) {
            //     lv_label_set_text_fmt(lbl_humidity_value, "%d.%d", humidity / 10, humidity % 10);
            // }
            lvgl_port_unlock();
            // ESP_LOGI("APP", "Humidity: %d.%d%% Temperature: %d.%d C", 
            //          humidity / 10, humidity % 10, 
            //          temperature / 10, temperature % 10);
        } else {
            ESP_LOGW("APP", "Failed to read DHT sensor");
        }
    }

}





void template_mpu6050_test()
{
    mpu6050_config_t dev_cfg          = I2C_MPU6050_CONFIG_DEFAULT;
    dev_cfg.accel_full_scale_range    = MPU6050_ACCEL_FS_RANGE_2G;
    mpu6050_handle_t dev_hdl;
    //
    // init device
    mpu6050_init(_i2c_bus, &dev_cfg, &dev_hdl);

    uint8_t                                 sample_rate_divider_reg;
    mpu6050_config_register_t               config_reg;
    mpu6050_gyro_config_register_t          gyro_config_reg;
    mpu6050_accel_config_register_t         accel_config_reg;
    mpu6050_interrupt_enable_register_t     irq_enable_reg;
    mpu6050_power_management1_register_t    power_management1_reg;
    mpu6050_power_management2_register_t    power_management2_reg;
    mpu6050_who_am_i_register_t             who_am_i_reg;

    /* attempt to read device sample rate divider register */
    mpu6050_get_sample_rate_divider_register(dev_hdl, &sample_rate_divider_reg);

    /* attempt to read device configuration register */
    mpu6050_get_config_register(dev_hdl, &config_reg);

    /* attempt to read device gyroscope configuration register */
    mpu6050_get_gyro_config_register(dev_hdl, &gyro_config_reg);

    /* attempt to read device accelerometer configuration register */
    mpu6050_get_accel_config_register(dev_hdl, &accel_config_reg);

    /* attempt to read device interrupt enable register */
    mpu6050_get_interrupt_enable_register(dev_hdl, &irq_enable_reg);

    /* attempt to read device power management 1 register */
    mpu6050_get_power_management1_register(dev_hdl, &power_management1_reg);

    /* attempt to read device power management 2 register */
    mpu6050_get_power_management2_register(dev_hdl, &power_management2_reg);

    /* attempt to read device who am i register */
    mpu6050_get_who_am_i_register(dev_hdl, &who_am_i_reg);

        // show registers
    ESP_LOGI(TAG, "Sample Rate Divider Register:         0x%02x (%s)", sample_rate_divider_reg, uint8_to_binary(sample_rate_divider_reg));
    ESP_LOGI(TAG, "Configuration Register:               0x%02x (%s)", config_reg.reg, uint8_to_binary(config_reg.reg));
    ESP_LOGI(TAG, "Gyroscope Configuration Register:     0x%02x (%s)", gyro_config_reg.reg, uint8_to_binary(gyro_config_reg.reg));
    ESP_LOGI(TAG, "Accelerometer Configuration Register: 0x%02x (%s)", accel_config_reg.reg, uint8_to_binary(accel_config_reg.reg));
    ESP_LOGI(TAG, "Interrupt Enable Register:            0x%02x (%s)", irq_enable_reg.reg, uint8_to_binary(irq_enable_reg.reg));
    ESP_LOGI(TAG, "Power Management 1 Register:          0x%02x (%s)", power_management1_reg.reg, uint8_to_binary(power_management1_reg.reg));
    ESP_LOGI(TAG, "Power Management 2 Register:          0x%02x (%s)", power_management2_reg.reg, uint8_to_binary(power_management2_reg.reg));
    ESP_LOGI(TAG, "Who am I Register:                    0x%02x (%s)", who_am_i_reg.reg, uint8_to_binary(who_am_i_reg.reg));

    while (1) {
        // handle sensor
        float temperature;
        mpu6050_gyro_data_axes_t gyro_data;
        mpu6050_accel_data_axes_t accel_data;
        esp_err_t result = mpu6050_get_motion(dev_hdl, &gyro_data, &accel_data, &temperature);
        if(result != ESP_OK) {
            ESP_LOGE(TAG, "mpu6050 device read failed (%s)", esp_err_to_name(result));
        } else {
            /* pitch and roll */
            float pitch = atanf(accel_data.x_axis / sqrtf(powf(accel_data.y_axis, 2.0f) + powf(accel_data.z_axis, 2.0f)));
            float roll  = atanf(accel_data.y_axis / sqrtf(powf(accel_data.x_axis, 2.0f) + powf(accel_data.z_axis, 2.0f)));

            // ESP_LOGI(TAG, "Accelerometer X-Axis: %fg", accel_data.x_axis);
            // ESP_LOGI(TAG, "Accelerometer Y-Axis: %fg", accel_data.y_axis);
            // ESP_LOGI(TAG, "Accelerometer Z-Axis: %fg", accel_data.z_axis);
            // ESP_LOGI(TAG, "Gyroscope X-Axis:     %f°/sec", gyro_data.x_axis);
            // ESP_LOGI(TAG, "Gyroscope Y-Axis:     %f°/sec", gyro_data.y_axis);
            // ESP_LOGI(TAG, "Gyroscope Z-Axis:     %f°/sec", gyro_data.z_axis);
            // ESP_LOGI(TAG, "Temperature:          %f°C", temperature);
            // ESP_LOGI(TAG, "Pitch Angle:          %f°", pitch);
            // ESP_LOGI(TAG, "Roll Angle:           %f°", roll);

            lvgl_port_lock(0);
            lv_obj_t * label_temp = ui_comp_get_child(ui_card, UI_COMP_CARD_LBLTEMP);
            lv_obj_t * label_accel_x = ui_comp_get_child(ui_card1, UI_COMP_CARD_LBLTEMP);
            lv_obj_t * label_accel_y = ui_comp_get_child(ui_card2, UI_COMP_CARD_LBLTEMP);
            lv_obj_t * label_accel_z = ui_comp_get_child(ui_card3, UI_COMP_CARD_LBLTEMP);
            lv_label_set_text_fmt(label_temp, "%d", (int)temperature);
            lv_label_set_text_fmt(label_accel_x, "%.2f", accel_data.x_axis);
            lv_label_set_text_fmt(label_accel_y, "%.2f", accel_data.y_axis);
            lv_label_set_text_fmt(label_accel_z, "%.2f", accel_data.z_axis);
            lvgl_port_unlock();


        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}