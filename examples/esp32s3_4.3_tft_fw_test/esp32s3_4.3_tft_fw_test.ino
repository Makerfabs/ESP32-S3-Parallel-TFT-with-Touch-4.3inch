//Confirm for V1.3

#include <Arduino_GFX_Library.h>
#include <SD.h>
#include <TAMC_GT911.h>
#include <Wire.h>

//jpeg
#include "JpegFunc.h"

// Date and time functions using a PCF8563 RTC connected via I2C and Wire lib
#include "RTClib.h"

RTC_PCF8563 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//TFT RTC SD
//V1.3 add RTC
//V1.4 Change BMP to JPEG
//

#define I2C_SDA_PIN 17
#define I2C_SCL_PIN 18

#define JPEG_FILENAME_LOGO "/logo.jpg"
#define JPEG_FILENAME_COVER "/cover.jpg"
#define JPEG_FILENAME_COVER_01 "/cover01.jpg"

#define JPEG_FILENAME_01 "/image01.jpg"
#define JPEG_FILENAME_02 "/image02.jpg"
#define JPEG_FILENAME_03 "/image03.jpg"
#define JPEG_FILENAME_04 "/image04.jpg"
#define JPEG_FILENAME_05 "/image05.jpg"

//microSD card  
#define SD_SCK  12
#define SD_MISO 13
#define SD_MOSI 11
#define SD_CS   10

#define TOUCH_INT -1
#define TOUCH_RST 38

#define TOUCH_ROTATION ROTATION_NORMAL

/**
#define TOUCH_MAP_X1 480
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 270
#define TOUCH_MAP_Y2 0
**/

#define TOUCH_MAP_X1 800
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 480
#define TOUCH_MAP_Y2 0


#define GFX_BL 2
#define TFT_BL GFX_BL

#define PWM_CHANNEL 1
#define PWM_FREQ 5000//Hz
#define pwm_resolution_bits 10


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

int touch_last_x = 0, touch_last_y = 0;

TAMC_GT911 ts = TAMC_GT911(I2C_SDA_PIN, I2C_SCL_PIN, TOUCH_INT, TOUCH_RST, max(TOUCH_MAP_X1, TOUCH_MAP_X2), max(TOUCH_MAP_Y1, TOUCH_MAP_Y2));

void TouchonInterrupt(void)
{
  //Serial.println("1!");
  ts.isTouched=true;
}


//int ColorArray[]={BLACK,NAVY,DARKGREEN,DARKCYAN,MAROON,PURPLE,OLIVE,LIGHTGREY,DARKGREY,BLUE,GREEN,CYAN,MAGENTA,YELLOW,WHITE,ORANGE,GREENYELLOW,PINK};
int ColorArray[]={BLACK,BLUE,GREEN,WHITE,RED,ORANGE};


void touch_init(void)
{
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    ts.begin();
    ts.setRotation(TOUCH_ROTATION);
}

bool touch_touched(void)
{
  #if 0
    ts.read();
    if (ts.isTouched)
    {
        touch_last_x = map(ts.points[0].x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, 480 - 1);
        touch_last_y = map(ts.points[0].y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, 480 - 1);

        // Serial.print("ox = ");
        // Serial.print(ts.points[0].x);
        // Serial.print(", oy = ");
        // Serial.print(ts.points[0].y);
        // Serial.print(",x = ");
        // Serial.print(touch_last_x);
        // Serial.print(", y = ");
        // Serial.print(touch_last_y);
        // Serial.println();

        return true;
    }
    else
    {
        return false;
    }
    #endif
        ts.read();
        if (ts.isTouched){
          touch_last_x = map(ts.points[0].x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, 800 - 1);
          touch_last_y = map(ts.points[0].y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, 480 - 1);

          
          for (int i=0; i<ts.touches; i++){
            Serial.print("Touch ");Serial.print(i+1);Serial.print(": ");;
            Serial.print("  x: ");Serial.print(ts.points[i].x);
            Serial.print("  y: ");Serial.print(ts.points[i].y);
            Serial.print("  size: ");Serial.println(ts.points[i].size);
            Serial.println(' ');
            break;
          }
          ts.isTouched=false;
          return true;
        }
        else
        {
          return false;
        }
}

void pcf8563_init(void)
{
    if (! rtc.begin()) 
    {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    
    //while (1) delay(10);
    return;
  }

  if (rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  // When the RTC was stopped and stays connected to the battery, it has
  // to be restarted by clearing the STOP bit. Let's do this to ensure
  // the RTC is running.
  rtc.start();  
}

void RTCShow(int num)
{
    DateTime now = rtc.now();
/**
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");

    // calculate a date which is 7 days, 12 hours and 30 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));

    Serial.print(" now + 7d + 12h + 30m + 6s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
**/
    gfx->setTextSize(3);
//    if(num) gfx->setTextColor(BLACK);
//    else gfx->setTextColor(WHITE);
    gfx->setCursor(100, 10);
    gfx->print("Time: ");    
    gfx->print(String(now.year()));
    gfx->print("/");
    //dtostrf(now.month(),2,0,myS);
    gfx->print(String(now.month()));
    gfx->print("/");
    gfx->print(String(now.day()));
    gfx->print("(");
    gfx->print(daysOfTheWeek[now.dayOfTheWeek()]);
    gfx->print(") ");
    gfx->print(String(now.hour()));
    gfx->print(":");
    gfx->print(String(now.minute()));
    gfx->print(":");
    gfx->print(String(now.second()));
    gfx->print("");
}





// pixel drawing callback
static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

void setup()
{

    Serial.begin(115200);
      
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    pinMode(TOUCH_RST, OUTPUT);
    delay(100);
    digitalWrite(TOUCH_RST, LOW);
    delay(1000);
    digitalWrite(TOUCH_RST, HIGH);
    delay(1000);

    ledcSetup(PWM_CHANNEL, PWM_FREQ, pwm_resolution_bits);  
    ledcAttachPin(TFT_BL, PWM_CHANNEL); 
  
    ledcWrite(PWM_CHANNEL, 1023); //output PWM

    //attachInterrupt(TOUCH_INT, TouchonInterrupt, RISING);
    //while (!Serial);
    Serial.println("JPEG Image Viewer");

    digitalWrite(TOUCH_RST, LOW);
    delay(1000);
    digitalWrite(TOUCH_RST, HIGH);
    delay(1000);
    touch_init();
    delay(300);
    pcf8563_init();
    
    // Init Display
    gfx->begin();
    gfx->fillScreen(WHITE);
    gfx->setTextSize(4);
    gfx->setTextColor(BLACK);
    gfx->setCursor(300, 50);
    gfx->println(F("Makerfabs"));
    gfx->setCursor(100, 100);
    gfx->println(F("4.3inch TFT with Touch "));

    //ledcWrite(PWM_CHANNEL, 10);
    delay(1000);
    gfx->setCursor(0, 20);
    //gfx->println(F("RED"));
    gfx->fillScreen(RED);
    Serial.println("--RED--");
    delay(1000);

    //ledcWrite(PWM_CHANNEL, 205);
    //gfx->println(F("GREEN"));
    gfx->fillScreen(GREEN);
    Serial.println("--GREEN--");
    delay(1000);

    //ledcWrite(PWM_CHANNEL, 512);
    //gfx->println(F("BLUE"));
    gfx->fillScreen(BLUE);
    Serial.println("--BLUE--");
    delay(1000);

    //ledcWrite(PWM_CHANNEL, 715);
    //gfx->println(F("WHITE"));
    gfx->fillScreen(WHITE);
    Serial.println("--WHITE--");
    delay(1000);

    ledcWrite(PWM_CHANNEL, 0);
    gfx->fillScreen(BLACK);
    Serial.println("--BLACK--");
    delay(1000);
    ledcWrite(PWM_CHANNEL, 1023); 
#if 1
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    if (!SD.begin(SD_CS))
    {
        Serial.println(F("ERROR: SD Mount Failed!"));
        //while(1)
        {
           gfx->fillScreen(WHITE);
           gfx->setTextSize(3);
           gfx->setTextColor(RED);
           gfx->setCursor(50, 180);
           gfx->println(F("ERROR: SD Mount Failed!"));
           delay(3000);
         }
    }
    else
    {
        unsigned long start = millis();
        jpegDraw(JPEG_FILENAME_LOGO, jpegDrawCallback, true /* useBigEndian */,
             0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
        Serial.printf("Time used: %lums\n", millis() - start);
    }

    delay(1000);
    Serial.println("ESP32S3 4.3inch LCD V1.1");

    gfx->fillScreen(WHITE);
    gfx->fillRect(200, 160, 260, 160, BLACK);

    gfx->setTextSize(4);
    gfx->setTextColor(WHITE);
    gfx->setCursor(200+20, 180);
    gfx->println("3 * POINTS ");
    gfx->setCursor(200+20, 220);
    gfx->println("TOUCH TO");
    gfx->setCursor(200+20, 260);
    gfx->println("CONTINUE");
    
    jpegDraw(JPEG_FILENAME_COVER_01, jpegDrawCallback, true /* useBigEndian */,
             0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
    delay(500);
    
    while (1)
    {
        ts.read();
        if (ts.isTouched){
          for (int i=0; i<ts.touches; i++){
            //gfx->fillScreen(WHITE);
            int temp=random(0,6);
            gfx->fillScreen(ColorArray[temp]);
            
            gfx->setTextSize(4);
            //if(num) gfx->setTextColor(BLACK);
            //else 
            if(temp==4)
              gfx->setTextColor(WHITE);
            else
              gfx->setTextColor(RED);
            gfx->setCursor(320, 400);
            gfx->print("X: ");
            gfx->println(String(ts.points[i].x));
            gfx->setCursor(320, 440);
            gfx->print("Y: ");
            gfx->println(String(ts.points[i].y));
            
            RTCShow(0);
            
            Serial.print("Touch ");Serial.print(i+1);Serial.print(": ");;
            Serial.print("  x: ");Serial.print(ts.points[i].x);
            Serial.print("  y: ");Serial.print(ts.points[i].y);
            Serial.print("  size: ");Serial.println(ts.points[i].size);
            Serial.println(' ');  
            
          }
          
          ts.isTouched=false;
          if(ts.touches>2) break;
        }
        delay(100);
    }
#endif


}

void loop()
{
    /**
    gfx->fillScreen(GREEN);
    Serial.println("GREEN");
    RTCShow(0);
    delay(1000);
    gfx->fillScreen(BLUE);
    Serial.println("BLUE");
    RTCShow(0);
    delay(1000);
    gfx->fillScreen(WHITE);
    Serial.println("WHITE");
    RTCShow(1);
    delay(1000);
    gfx->fillScreen(YELLOW);
    Serial.println("YELLOW");
    RTCShow(1);
    delay(1000);
    ***/

  int w = gfx->width();
  int h = gfx->height();
#if 0
  unsigned long start = millis();
  jpegDraw(JPEG_FILENAME_01, jpegDrawCallback, true /* useBigEndian */,
           random(w * 2) - w /* x */,
           random(h * 2) - h /* y */,
           w /* widthLimit */, h /* heightLimit */);
  Serial.printf("Time used: %lums\n", millis() - start);
  RTCShow(random(0,2));
  delay(1000);
#endif
  
  unsigned long  start = millis();
  jpegDraw(JPEG_FILENAME_01, jpegDrawCallback, true /* useBigEndian */,
             0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
  Serial.printf("Time used: %lums\n", millis() - start);
  delay(1000);
  start = millis();
  jpegDraw(JPEG_FILENAME_02, jpegDrawCallback, true /* useBigEndian */,
             0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
  Serial.printf("Time used: %lums\n", millis() - start);
  delay(1000);
  start = millis();
  jpegDraw(JPEG_FILENAME_03, jpegDrawCallback, true /* useBigEndian */,
             0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
  Serial.printf("Time used: %lums\n", millis() - start);
  delay(1000);
  start = millis();
  jpegDraw(JPEG_FILENAME_04, jpegDrawCallback, true /* useBigEndian */,
             0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
  Serial.printf("Time used: %lums\n", millis() - start);
  delay(1000);
  jpegDraw(JPEG_FILENAME_05, jpegDrawCallback, true /* useBigEndian */,
             0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
  Serial.printf("Time used: %lums\n", millis() - start);
  delay(1000);
    
}
