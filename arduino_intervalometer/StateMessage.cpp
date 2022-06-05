#include "StateMessage.h"
#include "Controller.h"
#include <cstring>

#define Y_OFFSET 24

StateMessage* StateMessage::instance = NULL;

StateMessage::StateMessage()
{
  SetCurrentSelectable(NULL);
  instance = this;
  _onComplete = NULL;
}

void StateMessage::Enter()
{
  Controller::GetInstance()->WakeUp(true);  

  Adafruit_SSD1306* display = Controller::GetInstance()->GetDisplay();

  display->clearDisplay();  
  display->setTextColor(SSD1306_WHITE);  

  display->setTextWrap(false);
  display->setTextSize(1);    

  display->drawLine(0, Y_OFFSET - 5, SCREEN_WIDTH, Y_OFFSET - 5, SSD1306_WHITE);

  display->setCursor(0,Y_OFFSET);  

  char* token = NULL;
  token = strtok(_messageText, " ");

  int16_t x1, y1;
  uint16_t width, height;
  
  while(token != NULL)
  {
    display->getTextBounds(token, display->getCursorX(), display->getCursorY(), &x1, &y1, &width, &height);

    if(x1 + width> SCREEN_WIDTH)
    {
      display->setCursor(0, display->getCursorY() + height);
    }

    display->print(token);
    display->print(" ");

    token = strtok(NULL, " ");
  }

  display->drawLine(0, display->getCursorY() + height + 4, SCREEN_WIDTH, display->getCursorY() + height + 4, SSD1306_WHITE);

  display->display();
}

void StateMessage::Update(bool forceRedraw)
{
  if(Controller::GetInstance()->GetMillisInCurrentState() > _messageDuration)
  {
    if(_onComplete != NULL)
    {
      _onComplete(true);
    }
  }
}

void StateMessage::Exit()
{}

void StateMessage::HandleEncoder(EncoderButton& eb)
{}

void StateMessage::HandleClick(EncoderButton& eb)
{}

void StateMessage::SetMessage(const char*  message, unsigned long duration)
{
  strcpy(_messageText, message);
  _messageDuration = duration;
}

void StateMessage::SetCompleteCallback(stateCompleteCallback onComplete)
{
  _onComplete = onComplete;
}

