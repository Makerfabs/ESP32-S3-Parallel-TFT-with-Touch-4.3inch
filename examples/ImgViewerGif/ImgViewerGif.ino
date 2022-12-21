

#include <Arduino_GFX_Library.h>
#include <SD.h>
#include "GifClass.h"

#define SD_SCK 12
#define SD_MISO 13
#define SD_MOSI 11
#define SD_CS 10

#define TFT_BL 2

// #define GIF_FILENAME "/ezgif.com-resize.gif"
// #define GIF_FILENAME "/ezgif.com-optimize.gif"
#define GIF_FILENAME "/test1.gif"

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
    GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
    40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
    5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
    8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */
);

// Uncomment for ST7262 IPS LCD 800x480
Arduino_RPi_DPI_RGBPanel *gfx = new Arduino_RPi_DPI_RGBPanel(
    bus,
    800 /* width */, 0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
    480 /* height */, 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
    1 /* pclk_active_neg */, 16000000 /* prefer_speed */, true /* auto_flush */);

static GifClass gifClass;

void setup()
{
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  Serial.begin(115200);
  Serial.println("Animated GIF Image Viewer");

  // Init Display
  gfx->begin();
  gfx->fillScreen(BLACK);

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
  if (!SD.begin(SD_CS))
  {
    Serial.println(F("ERROR: File System Mount Failed!"));
    gfx->println(F("ERROR: File System Mount Failed!"));
  }
}

void loop()
{

  File gifFile = SD.open(GIF_FILENAME, "r");
  if (!gifFile || gifFile.isDirectory())
  {
    Serial.println(F("ERROR: open gifFile Failed!"));
    gfx->println(F("ERROR: open gifFile Failed!"));
  }
  else
  {
    // read GIF file header
    gd_GIF *gif = gifClass.gd_open_gif(&gifFile);
    if (!gif)
    {
      Serial.println(F("gd_open_gif() failed!"));
    }
    else
    {
      uint8_t *buf = (uint8_t *)malloc(gif->width * gif->height);
      if (!buf)
      {
        Serial.println(F("buf malloc failed!"));
      }
      else
      {
        // int16_t x = (gfx->width() - gif->width) / 2;
        // int16_t y = (gfx->height() - gif->height) / 2;

        int16_t x = 16;
        int16_t y = 16;

        Serial.println(F("GIF video start"));
        int32_t t_fstart, t_delay = 0, t_real_delay, delay_until;
        int32_t res = 1;
        int32_t duration = 0, remain = 0;
        while (res > 0)
        {
          t_fstart = millis();
          t_delay = gif->gce.delay * 10;
          res = gifClass.gd_get_frame(gif, buf);
          if (res < 0)
          {
            Serial.println(F("ERROR: gd_get_frame() failed!"));
            break;
          }
          else if (res > 0)
          {
            // gfx->fillRect(x, y, gif->width, gif->height, BLACK);

            // gfx->drawIndexedBitmap(x, y, buf, gif->palette->colors, gif->width, gif->height);

            for (int i = 0; i < 12; i++)
              for (int j = 0; j < 7; j++)
                gfx->drawIndexedBitmap(x + 64 * i, y + 64 * j, buf, gif->palette->colors, gif->width, gif->height);

            t_real_delay = t_delay - (millis() - t_fstart);
            duration += t_delay;
            remain += t_real_delay;
            delay_until = millis() + t_real_delay;
            while (millis() < delay_until)
            {
              delay(1);
            }
          }
        }
        Serial.println(F("GIF video end"));
        Serial.print(F("duration: "));
        Serial.print(duration);
        Serial.print(F(", remain: "));
        Serial.print(remain);
        Serial.print(F(" ("));
        Serial.print(100.0 * remain / duration);
        Serial.println(F("%)"));

        gifClass.gd_close_gif(gif);
        free(buf);
      }
    }
  }
}