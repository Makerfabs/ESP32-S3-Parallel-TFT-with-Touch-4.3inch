//Confirm for V1.3

#include "config.h"

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

TAMC_GT911 tp = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);

USBHIDKeyboard Keyboard;

int pos[2] = {0, 0};

void setup()
{
    Serial.begin(115200);
    Serial.println("Keyboard begin");

    lcd_init();
    touch_init();
    Keyboard.begin();
    USB.begin();

    main_page();
}

void loop()
{
}

// Pages

void main_page()
{
    Button b[BUTTON_COUNT_M];

    String b_list[BUTTON_COUNT_M] = {
        "Keyboard 1",
        "Keyboard 2",
        "Keyboard 3"};

    gfx->fillScreen(COLOR_BACKGROUND);
    gfx->setTextColor(COLOR_TEXT);
    gfx->setTextSize(2);
    gfx->setCursor(10, 10);
    gfx->print("Makerfabs ESP32-S3 4.3\" Parallel lcd with Touch");
    gfx->setCursor(10, 42);
    gfx->print("Touch Keyboard Demo");

    // Button set
    for (int i = 0; i < BUTTON_COUNT_M; i++)
    {

        b[i].set(BUTTON_POS_X, BUTTON_POS_Y + 120 * i, 300, 100, "NULL", ENABLE);
        b[i].setText(b_list[i]);
        b[i].setValue(i);

        drawButton(b[i]);
    }

    while (1)
    {
        if (get_pos() == 1)
            for (int i = 0; i < BUTTON_COUNT_P1; i++)
            {
                int button_value = UNABLE;
                if ((button_value = b[i].checkTouch(pos[0], pos[1])) != UNABLE)
                {

                    Serial.printf("Pos is :%d,%d\n", pos[0], pos[1]);
                    Serial.printf("Value is :%d\n", button_value);
                    Serial.printf("Text is :");
                    Serial.println(b[i].getText());

                    drawButton_p(b[i]);
                    delay(BUTTON_DELAY);
                    drawButton(b[i]);

                    page_switch(button_value);

                    delay(200);
                }
            }
    }
}

void page1()
{
    Button b[BUTTON_COUNT_P1];

    String b_list[BUTTON_COUNT_P1] = {
        "Makerfabs",
        "Passward",
        "Now Time",
        "Ctrl + V",
        "Enter"};

    clean_button();

    // Button set
    for (int i = 0; i < BUTTON_COUNT_P1; i++)
    {

        b[i].set(BUTTON_POS_X, BUTTON_POS_Y + 80 * i, 300, 60, "NULL", ENABLE);
        b[i].setText(b_list[i]);
        b[i].setValue(i);

        drawButton(b[i]);
    }

    while (1)
    {
        if (get_pos() == 1)

            for (int i = 0; i < BUTTON_COUNT_P1; i++)
            {
                int button_value = UNABLE;
                if ((button_value = b[i].checkTouch(pos[0], pos[1])) != UNABLE)
                {

                    Serial.printf("Pos is :%d,%d\n", pos[0], pos[1]);
                    Serial.printf("Value is :%d\n", button_value);
                    Serial.printf("Text is :");
                    Serial.println(b[i].getText());

                    drawButton_p(b[i]);
                    delay(BUTTON_DELAY);
                    drawButton(b[i]);
                    key_input_1(button_value);
                    delay(200);
                }
            }
    }
}

void page2()
{
    Button b[BUTTON_COUNT_P2];

    String b_list[BUTTON_COUNT_P2] = {
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "0",
        "DEL",
        "ENTER"};

    clean_button();

    // Button set
    for (int i = 0; i < BUTTON_COUNT_P2; i++)
    {

        b[i].set(BUTTON_POS_X + i % 3 * 140, BUTTON_POS_Y + i / 3 * 90, 120, 80, "NULL", ENABLE);
        b[i].setText(b_list[i]);
        b[i].setValue(i);

        drawButton(b[i]);
    }

    while (1)
    {
        if (get_pos() == 1)

            for (int i = 0; i < BUTTON_COUNT_P2; i++)
            {
                int button_value = UNABLE;
                if ((button_value = b[i].checkTouch(pos[0], pos[1])) != UNABLE)
                {

                    Serial.printf("Pos is :%d,%d\n", pos[0], pos[1]);
                    Serial.printf("Value is :%d\n", button_value);
                    Serial.printf("Text is :");
                    Serial.println(b[i].getText());

                    drawButton_p(b[i]);
                    delay(BUTTON_DELAY);
                    drawButton(b[i]);
                    key_input_2(button_value);
                    delay(200);
                }
            }
    }
}

void page3()
{
    Button b[BUTTON_COUNT_P3];

    String b_list[BUTTON_COUNT_P3] = {
        "^",
        "v",
        "<",
        ">"};

    clean_button();

    b[0].set(320, 90, 160, 160, "NULL", ENABLE);
    b[1].set(320, 270, 160, 160, "NULL", ENABLE);
    b[2].set(120, 270, 160, 160, "NULL", ENABLE);
    b[3].set(520, 270, 160, 160, "NULL", ENABLE);

    // Button set
    for (int i = 0; i < BUTTON_COUNT_P3; i++)
    {
        b[i].setText(b_list[i]);
        b[i].setValue(i);
        b[i].setTextSize(8);
        drawButton(b[i]);
    }

    while (1)
    {
        if (get_pos() == 1)

            for (int i = 0; i < BUTTON_COUNT_P3; i++)
            {
                int button_value = UNABLE;
                if ((button_value = b[i].checkTouch(pos[0], pos[1])) != UNABLE)
                {

                    Serial.printf("Pos is :%d,%d\n", pos[0], pos[1]);
                    Serial.printf("Value is :%d\n", button_value);
                    Serial.printf("Text is :");
                    Serial.println(b[i].getText());

                    drawButton_p(b[i]);
                    delay(BUTTON_DELAY);
                    drawButton(b[i]);
                    key_input_3(button_value);
                    delay(200);
                }
            }
    }
}

// Hardware init

void lcd_init()
{
    // Pin init
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    // lcd init
    gfx->begin();
}

void touch_init()
{
    pinMode(TOUCH_RST, OUTPUT);
    digitalWrite(TOUCH_RST, LOW);
    delay(500);
    digitalWrite(TOUCH_RST, HIGH);
    delay(500);

    tp.begin();
    tp.setRotation(TOUCH_ROTATION);
}

int get_pos()
{
    tp.read();

    if (tp.isTouched && pos[0] != tp.points[0].x && pos[1] != tp.points[0].y)
    {
        pos[0] = tp.points[0].x;
        pos[1] = tp.points[0].y;

        Serial.print("ox = ");
        Serial.print(tp.points[0].x);
        Serial.print(", oy = ");
        Serial.print(tp.points[0].y);
        Serial.print(",x = ");
        Serial.print(pos[0]);
        Serial.print(", y = ");
        Serial.print(pos[1]);
        Serial.println();

        tp.isTouched = false;

        return 1;
    }
    else
    {
        pos[0] = -1;
        pos[1] = -1;
        return 0;
    }
}

// Draw button and shadow

void drawButton(Button b)
{
    int b_x;
    int b_y;
    int b_w;
    int b_h;
    int shadow_len = 4;
    String text;
    int textSize;

    b.getFoDraw(&b_x, &b_y, &b_w, &b_h, &text, &textSize);

    gfx->fillRect(b_x, b_y, b_w, b_h, COLOR_BUTTON);
    gfx->drawRect(b_x, b_y, b_w, b_h, COLOR_LINE);
    gfx->setCursor(b_x + 20, b_y + 20);
    gfx->setTextColor(COLOR_TEXT);
    gfx->setTextSize(textSize);
    gfx->print(text);

    // Add button shadow
    if (b.getValue() != UNABLE)
    {
        gfx->fillRect(b_x + shadow_len, b_y + b_h, b_w, shadow_len, COLOR_SHADOW);
        gfx->fillRect(b_x + b_w, b_y + shadow_len, shadow_len, b_h, COLOR_SHADOW);
    }
}

void drawButton_p(Button b)
{
    int b_x;
    int b_y;
    int b_w;
    int b_h;
    int shadow_len = 4;
    String text;
    int textSize;

    b.getFoDraw(&b_x, &b_y, &b_w, &b_h, &text, &textSize);

    gfx->fillRect(b_x, b_y, b_w + shadow_len, b_h + shadow_len, COLOR_BACKGROUND);

    gfx->fillRect(b_x + shadow_len, b_y + shadow_len, b_w, b_h, COLOR_BUTTON_P);
    gfx->drawRect(b_x + shadow_len, b_y + shadow_len, b_w, b_h, COLOR_LINE);
    gfx->setCursor(b_x + 20, b_y + 20);
    gfx->setTextColor(COLOR_TEXT);
    gfx->setTextSize(textSize);
    gfx->print(text);
}

void clean_button()
{
    gfx->fillRect(BUTTON_POS_X, BUTTON_POS_Y, 479 - BUTTON_POS_X, 799 - BUTTON_POS_Y, COLOR_BACKGROUND);
}

// Button Command
void page_switch(int page)
{
    switch (page)
    {
    case 0:
        page1();
        break;
    case 1:
        page2();
        break;
    case 2:
        page3();
        break;

    defualt:
        break;
    }
    delay(100);
}

void key_input_1(int value)
{
    switch (value)
    {
    case 0:
        Keyboard.print("Makerfabs");
        break;
    case 1:

        Keyboard.print("Password");
        break;
    case 2:
        Keyboard.print("Time");
        break;
    case 3:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press('V');
        break;
    case 4:
        Keyboard.press(KEY_RETURN);
        break;
    defualt:
        break;
    }
    delay(100);
    Keyboard.releaseAll();
}

void key_input_2(int value)
{
    if (value < 9)
    {
        Keyboard.write('1' + value);
    }
    else if (value == 9)
        Keyboard.write('0');
    else if (value == 10)
        Keyboard.write(KEY_BACKSPACE);
    else if (value == 11)
        Keyboard.write(KEY_RETURN);

    delay(100);
    Keyboard.releaseAll();
}

void key_input_3(int value)
{
    switch (value)
    {
    case 0:
        Keyboard.press(KEY_UP_ARROW);
        break;
    case 1:

        Keyboard.press(KEY_DOWN_ARROW);
        break;
    case 2:
        Keyboard.press(KEY_LEFT_ARROW);
        break;
    case 3:
        Keyboard.press(KEY_RIGHT_ARROW);
        break;
    defualt:
        break;
    }
    delay(100);
    Keyboard.releaseAll();
}
