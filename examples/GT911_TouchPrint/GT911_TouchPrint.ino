//Confirm for V1.3

#include "TAMC_GT911.h"

#define TOUCH_SDA 17
#define TOUCH_SCL 18
#define TOUCH_INT -1
#define TOUCH_RST 38
#define TOUCH_WIDTH 800
#define TOUCH_HEIGHT 480

TAMC_GT911 tp = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);

void setup()
{
  Serial.begin(115200);
  Serial.println("TAMC_GT911 Example: Ready");

  pinMode(TOUCH_RST, OUTPUT);
  digitalWrite(TOUCH_RST, LOW);
  delay(500);
  digitalWrite(TOUCH_RST, HIGH);
  delay(500);

  tp.begin();
  tp.setRotation(ROTATION_INVERTED);
}

void loop()
{
  tp.read();
  if (tp.isTouched)
  {
    for (int i = 0; i < tp.touches; i++)
    {
      Serial.print("Touch ");
      Serial.print(i + 1);
      Serial.print(": ");
      ;
      Serial.print("  x: ");
      Serial.print(tp.points[i].x);
      Serial.print("  y: ");
      Serial.print(tp.points[i].y);
      Serial.print("  size: ");
      Serial.println(tp.points[i].size);
      Serial.println(' ');
    }
    tp.isTouched = false;
  }
}
