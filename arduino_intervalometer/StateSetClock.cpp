#include "StateSetClock.h"
#include "Controller.h"

StateSetClock* StateSetClock::instance = NULL;

StateSetClock::StateSetClock()
{
  _hourSelectable = new Selectable(0, 14, [](){ return StateSetClock::instance->GetClockHoursText(); });
  _hourSelectable->SetTextScale(2);

  _minuteSelectable = new Selectable(32, 14, [](){ return StateSetClock::instance->GetClockMinutesText(); });
  _minuteSelectable->SetTextScale(2);

  _saveSelectable = new Selectable(0, 36, "DONE");
  _cancelSelectable = new Selectable(0, 46, "CANCEL");

  //Connect them all together
  _hourSelectable->SetLinkedSelectables(NULL, _minuteSelectable);
  _minuteSelectable->SetLinkedSelectables(_hourSelectable, _saveSelectable);
  _saveSelectable->SetLinkedSelectables(_minuteSelectable, _cancelSelectable);
  _cancelSelectable->SetLinkedSelectables(_saveSelectable, NULL);

  _currentSelectable = _hourSelectable;
  _currentSelectable->SetState(Selectable::FOCUSED);
}

void StateSetClock::Enter()
{
  StateSetClock::instance = this;

  if(_currentSelectable != _hourSelectable)
  {
    _currentSelectable->SetState(Selectable::NONE);
    _currentSelectable = _hourSelectable;
    _currentSelectable->SetState(Selectable::FOCUSED);
  }

  _hours = Controller::GetInstance()->GetRTC()->getHours();
  _minutes = Controller::GetInstance()->GetRTC()->getMinutes();

  Controller::GetInstance()->GetKnob()->setEncoderHandler([](EncoderButton& eb) 
  { 
    StateSetClock::instance->HandleEncoder(eb);
  });

  Controller::GetInstance()->GetKnob()->setClickHandler([](EncoderButton& eb) 
  {
    StateSetClock::instance->HandleClick(eb);
  });  

  _redrawRequired = true; 
}

void StateSetClock::HandleEncoder(EncoderButton& eb)
{  
  auto rtc = Controller::GetInstance()->GetRTC();

  if(_currentSelectable->GetState() == Selectable::SELECTED)
  {
    if(_currentSelectable == _hourSelectable)
    {
      _hours += eb.increment();

      while(_hours < 0)
      {
        _hours += 24;
      }

      _hours = _hours % 24;
    }
    else if(_currentSelectable == _minuteSelectable)
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
    if(eb.increment() > 0)
    {
      if(_currentSelectable->GetNextSelectable() != NULL)
      {
        _currentSelectable->SetState(Selectable::NONE);
        _currentSelectable = _currentSelectable->GetNextSelectable();
        _currentSelectable->SetState(Selectable::FOCUSED);
    
      }
    }
    else if(eb.increment() < 0)
    {
      if(_currentSelectable->GetPreviousSelectable() != NULL)
      {
        _currentSelectable->SetState(Selectable::NONE);
        _currentSelectable = _currentSelectable->GetPreviousSelectable();
        _currentSelectable->SetState(Selectable::FOCUSED);
    
      }      
    }

  }
  
  _redrawRequired = true;
}

void StateSetClock::HandleClick(EncoderButton& eb)
{
  if(_currentSelectable != NULL)
  {
    if(_currentSelectable->GetState() == Selectable::SELECTED)
    {
      _currentSelectable->SetState(Selectable::FOCUSED);
    }
    else
    {
      if(_currentSelectable == _saveSelectable)
      {
        Controller::GetInstance()->GetRTC()->setHours(_hours);
        Controller::GetInstance()->GetRTC()->setMinutes(_minutes);

        Controller::GetInstance()->SetState(Controller::RUNNING);
      }
      else if(_currentSelectable == _cancelSelectable)
      {
        Controller::GetInstance()->SetState(Controller::RUNNING);
      }
      _currentSelectable->SetState(Selectable::SELECTED);
    }
  
    _redrawRequired = true;
  }


}

void StateSetClock::Update()
{
  if(_redrawRequired)
  {
    _redrawRequired = false;    
    Adafruit_SSD1306* display = Controller::GetInstance()->GetDisplay();

    // TODO: Custom clock setting logic 
    display->clearDisplay();

    display->setTextColor(SSD1306_WHITE);        // Draw white text

    display->setTextSize(1);             // Normal 1:1 pixel scale
    display->setCursor(2,0);             // Start at top-left corner
    display->println(F("Set Clock"));  

    display->setTextSize(2);             // Normal 1:1 pixel scale
    display->setCursor(24,16);             // Start at top-left corner
    display->println(F(":"));  


    _hourSelectable->Update(display);
    _minuteSelectable->Update(display);
    _saveSelectable->Update(display);
    _cancelSelectable->Update(display);

    display->display();

  }  
}

void StateSetClock::Exit()
{


  Controller::GetInstance()->GetKnob()->setClickHandler(NULL);
  Controller::GetInstance()->GetKnob()->setEncoderHandler(NULL);  
}

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
