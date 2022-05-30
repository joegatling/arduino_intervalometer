#include "StateRunning.h"
#include "Controller.h"

StateRunning::StateRunning()
{
  strcpy(_clockString, "\0");
}

void StateRunning::Enter()
{}

void StateRunning::Update()
{
  Adafruit_SSD1306* display = Controller::GetInstance()->GetDisplay();

  display->clearDisplay();  
  display->setTextColor(SSD1306_WHITE);        // Draw white text

  display->setTextSize(1);             // Normal 1:1 pixel scale
  display->setCursor(0,0);             // Start at top-left corner
  
  sprintf(_clockString, "%02d:%02d", Controller::GetInstance()->GetRTC()->getHours(), Controller::GetInstance()->GetRTC()->getMinutes());
  
  int16_t  x1, y1;
  uint16_t width, height;

  display->getTextBounds(_clockString, 0, 0, &x1, &y1, &width, &height);

  display->setCursor(SCREEN_WIDTH - width, 0);
  display->print(_clockString);
  display->display();
}

void StateRunning::Exit()
{}

void StateRunning::HandleEncoder(EncoderButton& eb)
{}

void StateRunning::HandleClick(EncoderButton& eb)
{
  Controller::GetInstance()->SetState(Controller::SET_CLOCK);  
}