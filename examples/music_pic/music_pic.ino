#include <SPI.h>
#include <FS.h>
#include <Wire.h>
#include <SD.h>
#include <WiFi.h>

#include "LGFX_ESP32S3_RGB_MakerfabsParallelTFTwithTouch43.h"
#include "Audio.h"

// microSD card
#define SD_SCK 12
#define SD_MISO 13
#define SD_MOSI 11
#define SD_CS 10

// I2S
#define I2S_DOUT 19
#define I2S_BCLK 20
#define I2S_LRCK 2

// 準備したクラスのインスタンスを作成します。
LGFX display;
Audio audio;

String ssid = "Makerfabs";
String password = "20160704";

#define IMG_LIMIT 20

String img_list[IMG_LIMIT];
int img_num = 0;
int img_index = 0;

void setup(void)
{
    Serial.begin(115200);

    // SPIバスとパネルの初期化を実行すると使用可能になります。
    display.init();
    touch_set();
    display.fillScreen(TFT_RED);

    SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    if (!SD.begin(SD_CS))
    {
        Serial.println(F("ERROR: SD Mount Failed!"));
    }

    img_num = get_img_list(SD, "/", img_list, IMG_LIMIT);

    // display.drawJpgFile(SD, "/test.jpg");

    // WiFi.begin(ssid.c_str(), password.c_str());

    // int connect_count = 0;
    // while (WiFi.status() != WL_CONNECTED)
    // {
    //     vTaskDelay(500);
    //     Serial.print(".");
    //     connect_count++;
    //     if (connect_count > 20)
    //     {
    //         Serial.println("Wifi error");
    //     }
    // }

    audio.setPinout(I2S_BCLK, I2S_LRCK, I2S_DOUT);
    audio.setVolume(21);
    audio.connecttoFS(SD, "/ChildhoodMemory.mp3");
    // audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3"); //  128k mp3
    // audio.connecttohost("http://www.wdr.de/wdrlive/media/einslive.m3u");

    xTaskCreatePinnedToCore(Task_TFT, "Task_TFT", 20480, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task_Touch, "Task_Touch", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task_Audio, "Task_Audio", 10240, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
}

void loop(void)
{
}

void Task_TFT(void *pvParameters) // This is a task.
{
    while (1) // A Task shall never return or exit.
    {
        // display.fillRect(0, 0, 400, 480, TFT_YELLOW);
        // display.fillRect(400, 0, 400, 480, TFT_RED);
        // vTaskDelay(2000);
        // display.fillRect(0, 0, 400, 480, TFT_RED);
        // display.fillRect(400, 0, 400, 480, TFT_WHITE);
        // vTaskDelay(2000);
        // display.fillRect(0, 0, 400, 480, TFT_WHITE);
        // display.fillRect(400, 0, 400, 480, TFT_BLUE);
        // vTaskDelay(2000);

        // display.drawJpgFile(SD, "/test.jpg");
        // display.fillRect(480, 0, 320, 480, TFT_YELLOW);
        // vTaskDelay(2000);

        // display.drawJpgFile(SD, "/test.jpg", 320, 0);
        // display.fillRect(0, 0, 320, 480, TFT_YELLOW);
        // vTaskDelay(2000);

        img_next();
        vTaskDelay(2000);
    }
}

void Task_Audio(void *pvParameters) // This is a task.
{
    while (1)
    {
        audio.loop();
        vTaskDelay(1);
    }
}

void Task_Touch(void *pvParameters) // This is a task.
{
    while (1)
    {
        int32_t x, y;
        if (display.getTouch(&x, &y))
        {
            Serial.print(x);
            Serial.print(",");
            Serial.println(y);
        }
        vTaskDelay(100);
    }
}

//************************************

void touch_set()
{
    display.setTextSize((std::max(display.width(), display.height()) + 255) >> 8);

    // タッチが使用可能な場合のキャリブレーションを行います。（省略可）
    if (display.touch())
    {
        if (display.width() < display.height())
            display.setRotation(display.getRotation() ^ 1);

        // 画面に案内文章を描画します。
        display.setTextDatum(textdatum_t::middle_center);
        display.drawString("touch the arrow marker.", display.width() >> 1, display.height() >> 1);
        display.setTextDatum(textdatum_t::top_left);

        // タッチを使用する場合、キャリブレーションを行います。画面の四隅に表示される矢印の先端を順にタッチしてください。
        std::uint16_t fg = TFT_WHITE;
        std::uint16_t bg = TFT_BLACK;
        if (display.isEPD())
            std::swap(fg, bg);
        display.calibrateTouch(nullptr, fg, bg, std::max(display.width(), display.height()) >> 3);
    }
}

int get_img_list(fs::FS &fs, const char *dirname, String *list, int length)
{
    Serial.printf("Listing directory: %s\n", dirname);
    int i = 0;

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return i;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return i;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
        }
        else
        {
            String temp = "/";
            temp += file.name();
            if (temp.endsWith(".jpg"))
            {
                list[i] = temp;
                i++;
            }
            if (i >= IMG_LIMIT)
                return i;
        }
        file = root.openNextFile();
    }
    return i;
}

void img_next()
{
    img_index++;
    if (img_index >= img_num)
    {
        img_index = 0;
    }
    display.drawJpgFile(SD, img_list[img_index], 0, 0);
}
