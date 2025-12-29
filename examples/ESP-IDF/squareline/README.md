## LVGL design with SquareLine Studio

1. Click on Squareline to log in to your account, or click on "Sign up" if you don't have one.

2. Create a project.

2. Design Interface.

3. Export the UI code and select a folder to save it to.

   ![image-20251226163342808](https://easyimage.linwanrong.com/i/2025/12/26/r0j2gk-0.webp)

## Code Upload with IDF

- Clone the [ESP32-S3-Parallel-TFT-with-Touch-4.3inch](https://github.com/Makerfabs/ESP32-S3-Parallel-TFT-with-Touch-4.3inch/tree/main) repository and configure it according to the documentation.

  ```bash
  git clone https://github.com/Makerfabs/ESP32-S3-Parallel-TFT-with-Touch-4.3inch.git
  ```

- Copy the previously exported `ui` folder to the `components` directory and modify `ui/CmakeLists.txt`.

  > This example uses an interface I designed as an illustration.

  ```cmake
  idf_component_register(
      SRCS 
          "ui.c"
          "ui_helpers.c"
          "components/ui_comp.c"
          "components/ui_comp_card.c"
          "components/ui_comp_hook.c"
          "screens/ui_Screen1.c"
          "images/ui_img_wifi_png.c"
          "images/ui_img_home_png.c"
          "images/ui_img_1292609536.c"
          "images/ui_img_image_png.c"
          "images/ui_img_music_png.c"
          "images/ui_img_bell_png.c"
          "images/ui_img_settings_png.c"
      INCLUDE_DIRS 
          "."
          "components" 
          "screens"
          "images"
      REQUIRES 
          lvgl
  )
  ```

- Include the UI header file in `main/firmware/template.c`.

  ```c
  #include "ui.h"
  ```

  Modify the `template_lvgl_demos_test` function.

  ```c
  void template_lvgl_demos_test()
  {
      lvgl_port_lock(0);
  
      // lv_demo_widgets();
      
      // ui_init_styles();
      // create_main_ui();
  
      ui_init();
  
      lvgl_port_unlock();
  }
  ```

- Modify `main/release/4-3-unknown-gt911/release.c`

  ```c
  static void release_task(void *arg)
  {
      // extern void template_image_switch_lvgl(const char *path);
      // template_image_switch_lvgl("/sdcard/images");
  
      extern void template_lvgl_demos_test();
      template_lvgl_demos_test();
  
      // extern void template_dht11_test();
      // template_dht11_test();
  
      // extern void template_mpu6050_test();
      // template_mpu6050_test();
      
      vTaskDelete(NULL);
  }
  ```

After compiling and flashing the code at this stage, you should be able to see the interface designed in **Squareline Studio** on the screen.  Next, we will connect the **MPU6050** to the development board and display the sensor data on the designed interface.

- Add the **mpu6050** component. Modify `main/idf_components.yml` and add the following at the end:

  ```yml
  k0i05/esp_mpu6050: 1.2.7
  ```

- Modify `main/boards/board.h` and add the following at the end:

  ```c
  extern i2c_master_bus_handle_t _i2c_bus;
  ```

- The **MPU6050** sensor is connected to the **J2** interface on the development board. Modify the `main/firmware/template.c` file and add the `template_mpu6050_test` function.

  ```c
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
  ```

- Modify `main/release/4-3-unknown-gt911/release.c`

  ```c
  static void release_task(void *arg)
  {
      // extern void template_image_switch_lvgl(const char *path);
      // template_image_switch_lvgl("/sdcard/images");
  
      extern void template_lvgl_demos_test();
      template_lvgl_demos_test();
  
      // extern void template_dht11_test();
      // template_dht11_test();
  
      extern void template_mpu6050_test();
      template_mpu6050_test();
      
      vTaskDelete(NULL);
  }
  ```

- The compilation, flashing, and execution results are as follows:

  ![VID20251229143917-ezgif.com-optimize](https://easyimage.linwanrong.com/i/2025/12/29/p4knl0-0.gif)