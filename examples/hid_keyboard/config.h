#include <USB.h>
#include <USBHIDKeyboard.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>

#include "TAMC_GT911.h"
#include "Button.h"

#define I2C_SDA_PIN 17
#define I2C_SCL_PIN 18
#define TOUCH_INT 38
#define TOUCH_RST -1
#define TFT_BL 2

#define TOUCH_ROTATION ROTATION_NORMAL
#define TOUCH_MAP_X1 0
#define TOUCH_MAP_X2 480
#define TOUCH_MAP_Y1 270
#define TOUCH_MAP_Y2 0

#define COLOR_BACKGROUND BLACK
#define COLOR_BUTTON BLACK
#define COLOR_BUTTON_P 0x4BAF
#define COLOR_TEXT WHITE
#define COLOR_LINE WHITE
#define COLOR_SHADOW 0x4BAF

#define BUTTON_POS_X 10
#define BUTTON_POS_Y 90

#define BUTTON_DELAY 150

#define BUTTON_COUNT_M 3
#define BUTTON_COUNT_P1 5
#define BUTTON_COUNT_P2 12
#define BUTTON_COUNT_P3 5
