# Example Description
This project includes example code for some MaTouch products.
- [**MaTouch 1.28" ToolSet_Controller**](https://www.makerfabs.com/matouch-1-28-toolset-controller.html)
- [**MaTouch_ESP32-S3 Parallel TFT with Touch 4.3"**](https://www.makerfabs.com/esp32-s3-parallel-tft-with-touch-4-3-inch.html)

This repository is suitable for the [**MaTouch 1.28" ToolSet_Controller**](https://www.makerfabs.com/matouch-1-28-toolset-controller.html).

## How to use example
#### MaTouch 1.28" ToolSet_Controller
Select the development board as **1-28-gc9a01-cst816s** .
![image-20251222183148326](https://easyimage.linwanrong.com/i/2025/12/22/uagknl-0.webp)
This example includes four routines：
> `main/firmware/1-28-gc9a01-cst816s/release.c`

![image-20251222183606880](https://easyimage.linwanrong.com/i/2025/12/22/ud6c66-0.webp)
- xuanniu_test：Example of a rotary encoder.
![aadb7e029fc8b3403fd312ede316a084-ezgif.com-optimize](https://easyimage.linwanrong.com/i/2025/12/23/fdclmu-0.gif)
- template_image_switch_lvgl: Use LVGL's built-in libturbo-jpeg decoder to load JPG images from the SD card.
![image_switch](https://easyimage.linwanrong.com/i/2025/12/22/vt59vi-0.gif)
- template_lvgl_demos_test: Examples included with LVGL.  
![qq_pic_merged_1766452938301](https://easyimage.linwanrong.com/i/2025/12/23/f9a5e0-0.webp)
- template_pcf8563_test: A simple PCF8563 RTC example.

#### MaTouch_ESP32-S3 Parallel TFT with Touch 4.3"
Select the development board as **4-3-unkonwn-gt911** .
![clipboard_2025-12-22_23-23](https://easyimage.linwanrong.com/i/2025/12/23/dzyu9t-0.webp)
This example includes two routines：
> `main/firmware/4-3-unkonwn-gt911/release.c`

![clipboard_2025-12-22_23-2](https://easyimage.linwanrong.com/i/2025/12/23/e10ojm-0.webp)
- template_image_switch_lvgl: Use LVGL's built-in libturbo-jpeg decoder to load JPG images from the SD card.
![ezgif.com-optimize](https://easyimage.linwanrong.com/i/2025/12/23/f641rz-0.gif)
- template_lvgl_demos_test: Examples included with LVGL.
![qq_pic_merged_1766453592919](https://easyimage.linwanrong.com/i/2025/12/23/ffq74n-0.webp)

## Example Note
If using `template_image_switch_lvgl` example, the following options must be enabled in menuconfig.
> The JPG images are stored in the "images" directory on the SD card.

![image-20251223090228410](https://easyimage.linwanrong.com/i/2025/12/23/ex8p32-0.webp)
![image-20251223090352046](https://easyimage.linwanrong.com/i/2025/12/23/exypsg-0.webp)

If using the `template_lvgl_demos_test` example, enable different LVGL demos in menuconfig.
![image-20251223090701857](https://easyimage.linwanrong.com/i/2025/12/23/f027t7-0.webp)