#include "StateSetDuration.h"
#include "StateSetTimeInterval.h"
#include "Controller.h"

#define Y_OFFSET 24

char StateSetDuration::__durationString[] = "\0";

StateSetDuration::StateSetDuration()
{
  _exposureCountSelectable = new Selectable(0,0, "SHOTS");
  _totalDurationSelectable = new Selectable(0,0, "DURATION");
  _endTimeSelectable = new Selectable(0,0, "END TIME");
  _noEndSelectable = new Selectable(0,0, "NO END");

  _currentValueSelectable = new Selectable(0,0, GetDurationString );

  _saveSelectable = new Selectable(0, SCREEN_HEIGHT - SELECTABLE_SPACING_1, "SAVE");
  _saveSelectable->SetAlignment(Selectable::BOTTOM_LEFT);
  _cancelSelectable = new Selectable(0, SCREEN_HEIGHT, "CANCEL");
  _cancelSelectable->SetAlignment(Selectable::BOTTOM_LEFT);

  AddSelectable(_exposureCountSelectable);
  AddSelectable(_totalDurationSelectable);
  AddSelectable(_endTimeSelectable);
  AddSelectable(_noEndSelectable);
  AddSelectable(_currentValueSelectable);
  AddSelectable(_saveSelectable);
  AddSelectable(_cancelSelectable);

  SetCurrentSelectable(_currentValueSelectable);
}

void StateSetDuration::Enter()
{
  StateSetTimeInterval::GetInstance()->SetTitle("DURATION");
  StateSetTimeInterval::GetInstance()->SetTime(Controller::GetInstance()->GetConfig()->GetDurationHours(), 
                                                Controller::GetInstance()->GetConfig()->GetDurationMinutes(),  
                                                Controller::GetInstance()->GetConfig()->GetDurationSeconds());

  RearrangeSelectables();  
  _redrawRequired = true;
}

void StateSetDuration::Update(bool forceRedraw)
{
  _redrawRequired |= forceRedraw;
  if(UpdateAllSelectables() || _redrawRequired)
  {
    _redrawRequired = false;
    Adafruit_SSD1306* display = Controller::GetInstance()->GetDisplay();

    display->clearDisplay();  
    display->setTextColor(SSD1306_WHITE);  

    display->setTextSize(1);    
    display->setCursor(2,3);    
    display->println(F("END")); 

    DrawAllSelectables(display);

    auto config = Controller::GetInstance()->GetConfig();

    display->drawBitmap(0, _exposureCountSelectable->GetPositionY(), config->GetSessionEndStyle() == IntervalInfo::TOTAL_EXPOSURE_COUNT ? icon_true : icon_false, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(0, _totalDurationSelectable->GetPositionY(), config->GetSessionEndStyle() == IntervalInfo::TOTAL_DURATION ? icon_true : icon_false, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(0, _endTimeSelectable->GetPositionY(), config->GetSessionEndStyle() == IntervalInfo::DATE_TIME ? icon_true : icon_false, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(0, _noEndSelectable->GetPositionY(), config->GetSessionEndStyle() == IntervalInfo::NO_END ? icon_true : icon_false, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);

    display->fillRect(0, 0, SCREEN_WIDTH, SELECTABLE_SPACING_1, SSD1306_INVERSE);

    display->display();
  }
}

void StateSetDuration::Exit()
{}

void StateSetDuration::HandleEncoder(EncoderButton& eb)
{
  IncrementCurrentSelectable(eb.increment());
}

void StateSetDuration::HandleClick(EncoderButton& eb)
{
  if(GetCurrentSelectable() == _exposureCountSelectable)
  {
    Controller::GetInstance()->GetConfig()->SetSessionEndStyle(IntervalInfo::TOTAL_EXPOSURE_COUNT);
    RearrangeSelectables();
    //Controller::GetInstance()->SetState(Controller::SET_CLOCK);  
  }
  else if(GetCurrentSelectable() == _totalDurationSelectable)
  {
    Controller::GetInstance()->GetConfig()->SetSessionEndStyle(IntervalInfo::TOTAL_DURATION);
    RearrangeSelectables();
                                       
  }
  else if(GetCurrentSelectable() == _endTimeSelectable)
  {
    Controller::GetInstance()->GetConfig()->SetSessionEndStyle(IntervalInfo::DATE_TIME);
    RearrangeSelectables();
  } 
  else if(GetCurrentSelectable() == _noEndSelectable)
  {
    Controller::GetInstance()->GetConfig()->SetSessionEndStyle(IntervalInfo::NO_END);
    RearrangeSelectables();
  }     
  else
  {
    Controller::GetInstance()->SetState(Controller::IDLE);  
  }
}

void StateSetDuration::RearrangeSelectables()
{
  int offset = Y_OFFSET;
  IntervalInfo::SessionEndStyle endStyle = Controller::GetInstance()->GetConfig()->GetSessionEndStyle();
  Selectable::EndSequence();

  _exposureCountSelectable->SetPosition(ICON_WIDTH + 2, offset);
  Selectable::LinkInSequence(_exposureCountSelectable);
  offset += SELECTABLE_SPACING_1;

  if(endStyle == IntervalInfo::TOTAL_EXPOSURE_COUNT)
  {
    _currentValueSelectable->SetPosition(ICON_WIDTH + 2, offset);
    Selectable::LinkInSequence(_currentValueSelectable);
    offset += SELECTABLE_SPACING_1;
  } 

  _totalDurationSelectable->SetPosition(ICON_WIDTH + 2, offset);
  Selectable::LinkInSequence(_totalDurationSelectable);
  offset += SELECTABLE_SPACING_1;

  if(endStyle == IntervalInfo::TOTAL_DURATION)
  {
    _currentValueSelectable->SetPosition(ICON_WIDTH + 2, offset);
    Selectable::LinkInSequence(_currentValueSelectable);
    offset += SELECTABLE_SPACING_1;
  }   

  _endTimeSelectable->SetPosition(ICON_WIDTH + 2, offset);
  Selectable::LinkInSequence(_endTimeSelectable);
  offset += SELECTABLE_SPACING_1;

  if(endStyle == IntervalInfo::DATE_TIME)
  {
    _currentValueSelectable->SetPosition(ICON_WIDTH + 2, offset);
    Selectable::LinkInSequence(_currentValueSelectable);
    offset += SELECTABLE_SPACING_1;
  }   

  _noEndSelectable->SetPosition(ICON_WIDTH + 2, offset);
  Selectable::LinkInSequence(_noEndSelectable);

  _currentValueSelectable->SetEnabled(endStyle != IntervalInfo::NO_END);   

  Selectable::LinkInSequence(_saveSelectable);
  Selectable::LinkInSequence(_cancelSelectable);
  Selectable::EndSequence();  

  _redrawRequired = true;
}


char* StateSetDuration::GetDurationString()
{
  Controller::GetInstance()->GetConfig()->GenerateDurationString(__durationString);
  return __durationString;      
}

