#include <Wire.h>
#include "TAMC_GT911.h"

#define TOUCH_GT911
#define TOUCH_GT911_SCL 18
#define TOUCH_GT911_SDA 17
#define TOUCH_GT911_INT -1
#define TOUCH_GT911_RST 38
#define TOUCH_GT911_ROTATION ROTATION_NORMAL

int16_t touch_last_x = 0;
int16_t touch_last_y = 0;

TAMC_GT911 ts = TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, 800, 480);

void touch_init()
{
  Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
  ts.begin();
  ts.setRotation(TOUCH_GT911_ROTATION);
}

bool touch_has_signal()
{

  return true;
}

bool touch_touched()
{

  ts.read();
  if (ts.isTouched)
  {
    int16_t touch_raw_x = ts.points[0].x;
    int16_t touch_raw_y = ts.points[0].y;

    touch_last_x = map(touch_raw_x, 800, 324, 0, 800);
    touch_last_y = map(touch_raw_y, 470, 214, 0, 480);

    Serial.println(touch_raw_x);
    Serial.println(touch_raw_y);
    Serial.println(touch_last_x);
    Serial.println(touch_last_y);
    Serial.println("..........................");

    return true;
  }

  return false;
}

bool touch_released()
{

  return true;
}
