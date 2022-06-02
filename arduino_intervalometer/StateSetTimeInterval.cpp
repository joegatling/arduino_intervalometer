#include "StateSetTimeInterval.h"
#include "Controller.h"

StateSetTimeInterval* StateSetTimeInterval::instance = NULL;

StateSetTimeInterval::StateSetTimeInterval()
{
  _hourSelectable = new Selectable(0, SELECTABLE_SPACING_2, [](){ return StateSetTimeInterval::instance->GetHoursText(); });
  _hourSelectable->SetTextScale(2);

  _minuteSelectable = new Selectable(0, SELECTABLE_SPACING_2 * 2, [](){ return StateSetTimeInterval::instance->GetMinutesText(); });
  _minuteSelectable->SetTextScale(2);

  _secondsSelectable = new Selectable(0, SELECTABLE_SPACING_2 * 3, [](){ return StateSetTimeInterval::instance->GetSecondsText(); });
  _secondsSelectable->SetTextScale(2);

  _saveSelectable = new Selectable(0, SCREEN_HEIGHT - SELECTABLE_SPACING_1, "DONE");
  _saveSelectable->SetAlignment(Selectable::BOTTOM_LEFT);
  _cancelSelectable = new Selectable(0, SCREEN_HEIGHT, "CANCEL");
  _cancelSelectable->SetAlignment(Selectable::BOTTOM_LEFT);

  Selectable::LinkInSequence(_hourSelectable);
  Selectable::LinkInSequence(_minuteSelectable);
  Selectable::LinkInSequence(_secondsSelectable);
  Selectable::LinkInSequence(_saveSelectable);
  Selectable::LinkInSequence(_cancelSelectable);
  Selectable::EndSequence();

  AddSelectable(_hourSelectable);
  AddSelectable(_minuteSelectable);
  AddSelectable(_secondsSelectable);
  AddSelectable(_saveSelectable);
  AddSelectable(_cancelSelectable);

  SetCurrentSelectable(_secondsSelectable);

  StateSetTimeInterval::instance = this;
}

void StateSetTimeInterval::Enter()
{
  _originalHours = _hours;
  _originalMinutes = _minutes;
  _originalSeconds = _seconds;

  _redrawRequired = true;

  SetCurrentSelectable(_secondsSelectable);
}

void StateSetTimeInterval::HandleEncoder(EncoderButton& eb)
{  
  if(GetCurrentSelectable()->GetState() == Selectable::SELECTED)
  {
    if(GetCurrentSelectable() == _hourSelectable)
    {
      int8_t newHours = _hours + eb.increment();

      while(newHours < 0)
      {
        newHours += 100;
      }

      _hours = (uint8_t)newHours % 100;
    }
    else if(GetCurrentSelectable() == _minuteSelectable)
    {
      int8_t newMinutes = _minutes +  eb.increment();

      while(newMinutes < 0)
      {
        newMinutes += 60;
      }

      _minutes = (uint8_t)newMinutes % 60;
    }   
    else if(GetCurrentSelectable() == _secondsSelectable)
    {
      int8_t newSeconds = _seconds + eb.increment();

      while(newSeconds < 0)
      {
        newSeconds += 60;
      }

      _seconds = (uint8_t)newSeconds % 60;
    }       
  }
  else
  {
    IncrementCurrentSelectable(eb.increment());
  }
}

void StateSetTimeInterval::HandleClick(EncoderButton& eb)
{
  if(GetCurrentSelectable() != NULL)
  {
    if(GetCurrentSelectable()->GetState() == Selectable::SELECTED)
    {
      GetCurrentSelectable()->SetState(Selectable::FOCUSED);
    }
    else
    {
      if(GetCurrentSelectable() == _cancelSelectable)
      {        
        _hours = _originalHours;
        _minutes = _originalMinutes;
        _seconds = _originalSeconds;

        if(_onComplete != NULL)
        {
          _onComplete(true);
        }

        //Controller::GetInstance()->SetState(Controller::IDLE);
      }
      else if(GetCurrentSelectable() == _saveSelectable)
      {
        if(_onComplete != NULL)
        {
          _onComplete(false);
        }

//        Controller::GetInstance()->SetState(Controller::IDLE);
      }

      GetCurrentSelectable()->SetState(Selectable::SELECTED);
    }
  }


}

void StateSetTimeInterval::Update()
{
  if(UpdateAllSelectables() || _redrawRequired )
  {
    Adafruit_SSD1306* display = Controller::GetInstance()->GetDisplay();

    display->clearDisplay();

    display->setTextColor(SSD1306_WHITE);  

    display->setTextSize(1);             
    display->setCursor(2,3);             
    display->println(_titleText);  
    display->fillRect(0, 0, SCREEN_WIDTH, SELECTABLE_SPACING_1, SSD1306_INVERSE);

    display->setCursor(_hourSelectable->GetPositionX() + _hourSelectable->GetWidth() + 2, _hourSelectable->GetPositionY() + 8);
    display->println(F("HOUR"));  

    display->setCursor(_minuteSelectable->GetPositionX() + _minuteSelectable->GetWidth() + 2, _minuteSelectable->GetPositionY() + 8);
    display->println(F("MIN"));  

    display->setCursor(_secondsSelectable->GetPositionX() + _secondsSelectable->GetWidth() + 2, _secondsSelectable->GetPositionY() + 8);
    display->println(F("SEC"));  

    DrawAllSelectables(display);
    
    display->display();

    _redrawRequired = false;

  }  
}

void StateSetTimeInterval::Exit()
{}

void StateSetTimeInterval::SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  _hours = hours;
  _minutes = minutes;
  _seconds = seconds;
}

void StateSetTimeInterval::SetTitle(const char* title)
{
  _titleText = title;
}

void StateSetTimeInterval::SetCompleteCallback(stateCompleteCallback onComplete)
{
  _onComplete = onComplete;
}

char* StateSetTimeInterval::GetHoursText()
{
  sprintf(_hoursText, "%02d", _hours);
  return _hoursText;
}

char* StateSetTimeInterval::GetMinutesText()
{
  sprintf(_minutesText, "%02d", _minutes);
  return _minutesText;  
}

char* StateSetTimeInterval::GetSecondsText()
{
  sprintf(_secondsText, "%02d", _seconds);
  return _secondsText;  
}