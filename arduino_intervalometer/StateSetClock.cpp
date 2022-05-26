#include "StateSetClock.h"
#include "Controller.h"

StateSetClock* StateSetClock::instance = NULL;

StateSetClock::StateSetClock()
{
  _hourSelectable = new Selectable(0, 14, [](){ return StateSetClock::instance->GetClockHoursText(); });
  _hourSelectable->SetTextScale(2);

  _minuteSelectable = new Selectable(32, 14, [](){ return StateSetClock::instance->GetClockMinutesText(); });
  _minuteSelectable->SetTextScale(2);

  _saveSelectable = new Selectable(0, SCREEN_HEIGHT - SELECTABLE_SPACING_1, "DONE");
  _saveSelectable->SetAlignment(Selectable::BOTTOM_LEFT);
  _cancelSelectable = new Selectable(0, SCREEN_HEIGHT, "CANCEL");
  _cancelSelectable->SetAlignment(Selectable::BOTTOM_LEFT);

  //Connect them all together
  _hourSelectable->SetLinkedSelectables(NULL, _minuteSelectable);
  _minuteSelectable->SetLinkedSelectables(_hourSelectable, _saveSelectable);
  _saveSelectable->SetLinkedSelectables(_minuteSelectable, _cancelSelectable);
  _cancelSelectable->SetLinkedSelectables(_saveSelectable, NULL);

  AddSelectable(_hourSelectable);
  AddSelectable(_minuteSelectable);
  AddSelectable(_saveSelectable);
  AddSelectable(_cancelSelectable);

  SetCurrentSelectable(_hourSelectable);
}

void StateSetClock::Enter()
{
  StateSetClock::instance = this;

  SetCurrentSelectable(_hourSelectable);

  _hours = Controller::GetInstance()->GetRTC()->getHours();
  _minutes = Controller::GetInstance()->GetRTC()->getMinutes(); 
}

void StateSetClock::HandleEncoder(EncoderButton& eb)
{  
  auto rtc = Controller::GetInstance()->GetRTC();

  if(GetCurrentSelectable()->GetState() == Selectable::SELECTED)
  {
    if(GetCurrentSelectable() == _hourSelectable)
    {
      _hours += eb.increment();

      while(_hours < 0)
      {
        _hours += 24;
      }

      _hours = _hours % 24;
    }
    else if(GetCurrentSelectable() == _minuteSelectable)
    {
      _minutes += eb.increment();

      while(_minutes < 0)
      {
        _minutes += 24;
      }

      _minutes = _minutes % 60;
    }   
  }
  else
  {
    IncrementCurrentSelectable(eb.increment());
  }
}

void StateSetClock::HandleClick(EncoderButton& eb)
{
  if(GetCurrentSelectable() != NULL)
  {
    if(GetCurrentSelectable()->GetState() == Selectable::SELECTED)
    {
      GetCurrentSelectable()->SetState(Selectable::FOCUSED);
    }
    else
    {
      if(GetCurrentSelectable() == _saveSelectable)
      {
        Controller::GetInstance()->GetRTC()->setHours(_hours);
        Controller::GetInstance()->GetRTC()->setMinutes(_minutes);

        Controller::GetInstance()->SetState(Controller::IDLE);
      }
      else if(GetCurrentSelectable() == _cancelSelectable)
      {
        Controller::GetInstance()->SetState(Controller::IDLE);
      }

      GetCurrentSelectable()->SetState(Selectable::SELECTED);
    }
  }


}

void StateSetClock::Update()
{
  if(UpdateAllSelectables())
  {
    Adafruit_SSD1306* display = Controller::GetInstance()->GetDisplay();

    // TODO: Custom clock setting logic 
    display->clearDisplay();

    display->setTextColor(SSD1306_WHITE);        // Draw white text

    display->setTextSize(1);             // Normal 1:1 pixel scale
    display->setCursor(2,0);             // Start at top-left corner
    display->println(F("CLOCK"));  

    display->setTextSize(2);             // Normal 1:1 pixel scale
    display->setCursor(24,16);             // Start at top-left corner
    display->println(F(":"));  

    DrawAllSelectables(display);
    
    display->display();

  }  
}

void StateSetClock::Exit()
{}

char* StateSetClock::GetClockHoursText()
{
  sprintf(_clockHoursText, "%02d", _hours);
  return _clockHoursText;
}

char* StateSetClock::GetClockMinutesText()
{
  sprintf(_clockMinutesText, "%02d", _minutes);
  return _clockMinutesText;  
}
