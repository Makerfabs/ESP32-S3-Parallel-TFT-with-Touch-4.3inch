#include "TAMC_GT911.h"

#define TOUCH_SDA  17
#define TOUCH_SCL  18
#define TOUCH_INT  38
#define TOUCH_RST -1
#define TOUCH_WIDTH  800
#define TOUCH_HEIGHT 480

bool touch_swap_xy = false;
//int16_t touch_map_x1 = 320;
//int16_t touch_map_x2 = 800;
//int16_t touch_map_y1 = 200;
//int16_t touch_map_y2 = 480;

int16_t touch_map_x1 = 480;
int16_t touch_map_x2 = 0;
int16_t touch_map_y1 = 272;
int16_t touch_map_y2 = 0;

//TAMC_GT911 tp = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);
TAMC_GT911 tp = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, max(touch_map_x1, touch_map_x2), max(touch_map_y1, touch_map_y2));
//X: 320~800  Y:200~480
void TouchonInterrupt(void)
{
  //Serial.println("1!");
  tp.isTouched=true;
}
void setup() {
  Serial.begin(115200);
  Serial.println("TAMC_GT911 Example: Ready");
  //Wire.begin(TOUCH_SDA,TOUCH_SCL); //Join I2C bus
  tp.begin();
  tp.setRotation(ROTATION_NORMAL);
  attachInterrupt(TOUCH_INT, TouchonInterrupt, RISING);
  
}

void loop() {
  tp.read();
  if (tp.isTouched){
    for (int i=0; i<tp.touches; i++){
      Serial.print("Touch ");Serial.print(i+1);Serial.print(": ");;
      Serial.print("  x: ");Serial.print(tp.points[i].x);
      Serial.print("  y: ");Serial.print(tp.points[i].y);
      Serial.print("  size: ");Serial.println(tp.points[i].size);
      Serial.println(' ');
    }
    tp.isTouched=false;
  }
}
