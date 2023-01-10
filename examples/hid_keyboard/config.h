#include <USB.h>
#include <USBHIDKeyboard.h>
#include <SPI.h>
#include <Arduino_GFX_Library.h>

#include "TAMC_GT911.h"
#include "Button.h"

#define TOUCH_SDA 17
#define TOUCH_SCL 18
#define TOUCH_INT -1
#define TOUCH_RST 38
#define TOUCH_WIDTH 800
#define TOUCH_HEIGHT 480

#define TFT_BL 2

#define TOUCH_ROTATION ROTATION_INVERTED


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
#define BUTTON_COUNT_P3 4
