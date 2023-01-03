
#include <Arduino_GFX_Library.h>

//PWM
// GPIO_2 output PWM

#define PWM_CHANNEL 1
#define PWM_FREQ 5000//Hz
#define pwm_resolution_bits 10
#define IO_PWM_PIN 2


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


void setup() {
  // put your setup code here, to run once:

  //Serial.begin(115200);
  //Serial.println("Hello, Backlight Controller");

  USBSerial.begin(115200);
  USBSerial.println("Hello, Backlight Controller");
  
  pinMode(IO_PWM_PIN, OUTPUT); 
  
  ledcSetup(PWM_CHANNEL, PWM_FREQ, pwm_resolution_bits);  //设置LEDC通道8频率为2.78K，分辨率为10位，即占空比可选0~1023
  ledcAttachPin(IO_PWM_PIN, PWM_CHANNEL); //设置LEDC通道8在IO2上输出
  
  ledcWrite(PWM_CHANNEL, 1023); //设置输出PWM

  // Init Display
  gfx->begin();
  gfx->fillScreen(WHITE);
  USBSerial.println("--WHITE--");
  delay(1000);
}

void loop() 
{

  // put your main code here, to run repeatedly:
  for(int i=0;i<1024;i+=20)
  {
    ledcWrite(PWM_CHANNEL, i);
    delay(50);
    USBSerial.println(i);
  }
  delay(2000);
  for(int i=1023;i>0;i-=20)
  {
    ledcWrite(PWM_CHANNEL, i);
    delay(50);
    USBSerial.println(i);    
  }
  delay(3000);
}
