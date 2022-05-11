#include <ctime>
#include <RTCZero.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EncoderButton.h>

#include "Controller.h"

void setup() 
{
  Controller::Initialize(Controller::SET_CLOCK);

  Adafruit_SSD1306* display = Controller::GetInstance()->GetDisplay();
  display->println("Setup Complete");
  display->display();
  delay(200);
}

void loop() 
{

  // Adafruit_SSD1306* display = Controller::GetInstance()->GetDisplay();
  // display->clearDisplay();
  // display->setCursor(0,0);             // Start at top-left corner

  // display->setTextColor(SSD1306_WHITE);        // Draw white text

  // display->setTextSize(1);             // Normal 1:1 pixel scale
  // display->println(millis());
  // display->display();

  // delay(100);

  Controller::GetInstance()->Update();
}
