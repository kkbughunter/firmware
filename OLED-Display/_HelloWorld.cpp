/*
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

monitor_speed = 115200

lib_deps =
    adafruit/Adafruit SSD1306
    adafruit/Adafruit GFX Library
*/


#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RESET -1

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET);

void setup()
{
  Serial.begin(115200);

  // Initialize I2C
  Wire.begin(OLED_SDA, OLED_SCL);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED initialization failed!");
    while (true)
      ;
  }

  // Clear display
  display.clearDisplay();

  // Text settings
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  // Display Hello World
  display.setCursor(0, 20);
  display.println("Hello");
  display.println("World!");

  // Update OLED
  display.display();

  Serial.println("OLED initialized successfully!");
}

void loop()
{
  // Nothing needed
}
