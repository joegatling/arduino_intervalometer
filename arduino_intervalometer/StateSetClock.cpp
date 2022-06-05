#include "StateSetClock.h"
#include "Controller.h"

StateSetClock* StateSetClock::instance = NULL;

#define Y_OFFSET 24

StateSetClock::StateSetClock()
{
  StateSetClock::instance = this;

  _hourSelectable = new Selectable(0, Y_OFFSET, [](){ return StateSetClock::instance->GetClockHoursText(); });
  _hourSelectable->SetTextScale(2);

  _minuteSelectable = new Selectable(32, Y_OFFSET, [](){ return StateSetClock::instance->GetClockMinutesText(); });
  _minuteSelectable->SetTextScale(2);

  _amPmSelectable = new Selectable(42, Y_OFFSET + SELECTABLE_SPACING_2, [](){ return StateSetClock::GetInstance()->GetAmPmText(); });

  _timeFormatSelectable = new Selectable(ICON_WIDTH + 2, SCREEN_HEIGHT - SELECTABLE_SPACING_1*3, "24h");
  _timeFormatSelectable->SetAlignment(Selectable::BOTTOM_LEFT);

  _saveSelectable = new Selectable(0, SCREEN_HEIGHT - SELECTABLE_SPACING_1, "DONE");
  _saveSelectable->SetAlignment(Selectable::BOTTOM_LEFT);
  _cancelSelectable = new Selectable(0, SCREEN_HEIGHT, "CANCEL");
  _cancelSelectable->SetAlignment(Selectable::BOTTOM_LEFT);

  //Connect them all together
  Selectable::LinkInSequence(_hourSelectable);
  Selectable::LinkInSequence(_minuteSelectable);
  Selectable::LinkInSequence(_amPmSelectable);
  Selectable::LinkInSequence(_timeFormatSelectable);
  Selectable::LinkInSequence(_saveSelectable);
  Selectable::LinkInSequence(_cancelSelectable);
  Selectable::EndSequence();

  AddSelectable(_hourSelectable);
  AddSelectable(_minuteSelectable);
  AddSelectable(_saveSelectable);
  AddSelectable(_cancelSelectable);
  AddSelectable(_timeFormatSelectable);
  AddSelectable(_amPmSelectable);

  SetCurrentSelectable(_hourSelectable);
}

void StateSetClock::Enter()
{
  _amPmSelectable->SetEnabled(Controller::GetInstance()->Get24TimeFormat() == false);

  SetCurrentSelectable(_hourSelectable);

  _originalHours = _hours;
  _originalMinutes = _minutes;

}

void StateSetClock::HandleEncoder(EncoderButton& eb)
{  
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
        _minutes += 60;
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
        if(_onComplete != NULL)
        {
          _onComplete(false);
        }
      }
      else if(GetCurrentSelectable() == _cancelSelectable)
      {
        _hours = _originalHours;
        _minutes = _originalMinutes;

        if(_onComplete != NULL)
        {
          _onComplete(true);
        }
      }
      else if(GetCurrentSelectable() == _timeFormatSelectable)
      {
        Controller::GetInstance()->Set24TimeFormat(!Controller::GetInstance()->Get24TimeFormat());
        _amPmSelectable->SetEnabled(Controller::GetInstance()->Get24TimeFormat() == false);
        _timeFormatSelectable->Flash();
      }
      else if(GetCurrentSelectable() == _amPmSelectable)
      {
        _hours = (_hours + 12) % 24;
        _amPmSelectable->Flash();
      }      
      else
      {
        GetCurrentSelectable()->SetState(Selectable::SELECTED);
      }
    }
  }


}

void StateSetClock::Update(bool forceRedraw)
{

  if(UpdateAllSelectables() || forceRedraw)
  {
    Adafruit_SSD1306* display = Controller::GetInstance()->GetDisplay();

    // TODO: Custom clock setting logic 
    display->clearDisplay();

    display->setTextColor(SSD1306_WHITE);        // Draw white text

    display->setTextSize(1);             // Normal 1:1 pixel scale
    display->setCursor(2,3);             // Start at top-left corner
    display->println(_titleText);  

    display->setTextSize(2);             // Normal 1:1 pixel scale
    display->setCursor(24,_hourSelectable->GetPositionY()+2);             // Start at top-left corner
    display->println(F(":"));  

    DrawAllSelectables(display);

    if(_timeFormatSelectable->GetEnabled())
    {
      display->drawBitmap(0, _timeFormatSelectable->GetPositionY() - _timeFormatSelectable->GetHeight(), Controller::GetInstance()->Get24TimeFormat() ? icon_true : icon_false, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    }

    display->fillRect(0, 0, SCREEN_WIDTH, SELECTABLE_SPACING_1, SSD1306_INVERSE);
    
    display->display();

  }  
}

void StateSetClock::Exit()
{}

char* StateSetClock::GetClockHoursText()
{
  if(Controller::GetInstance()->Get24TimeFormat())
  {
    sprintf(_clockHoursText, "%02d", _hours);
  }
  else
  {
    if(_hours == 0 || _hours == 12)
    {
      sprintf(_clockHoursText, "12");
    }
    else
    {
      sprintf(_clockHoursText, "%2d", _hours % 12);
    }
  }
  return _clockHoursText;
}

char* StateSetClock::GetClockMinutesText()
{
  sprintf(_clockMinutesText, "%02d", _minutes);
  return _clockMinutesText;  
}

char* StateSetClock::GetAmPmText()
{
  if(_hours < 12)
  {
    sprintf(_amPmText, "AM");
  }
  else
  {
    sprintf(_amPmText, "PM");
  }
  return _amPmText;  
}


void StateSetClock::ShowTimeFormatOption(bool show)
{
  _timeFormatSelectable->SetEnabled(show);
}

void StateSetClock::SetTime(uint8_t hours, uint8_t minutes)
{
  _hours = hours;
  _minutes = minutes;
}

void StateSetClock::SetTitle(const char* title)
{
  _titleText = title;
}

void StateSetClock::SetCanCancel(bool canCancel)
{
  _cancelSelectable->SetEnabled(canCancel);
}

bool StateSetClock::GetCanCancel()
{
  return _cancelSelectable->GetEnabled();
}


void StateSetClock::SetCompleteCallback(stateCompleteCallback onComplete)
{
  _onComplete = onComplete;
}

