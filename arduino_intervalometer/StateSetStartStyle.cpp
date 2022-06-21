#include "StateSetStartStyle.h"
#include "StateSetTimeInterval.h"
#include "StateSetClock.h"
#include "Controller.h"

#define Y_OFFSET 24

StateSetStartStyle::StateSetStartStyle()
{

  _timeSelectable = new Selectable(ICON_WIDTH + 2,Y_OFFSET, "AT TIME");
  _delaySelectable = new Selectable(ICON_WIDTH + 2,Y_OFFSET + SELECTABLE_SPACING_1, "DELAY");

  _cancelSelectable = new Selectable(0, SCREEN_HEIGHT, "CANCEL");
  _cancelSelectable->SetAlignment(Selectable::BOTTOM_LEFT);


  Selectable::LinkInSequence(_timeSelectable);
  Selectable::LinkInSequence(_delaySelectable);
  Selectable::LinkInSequence(_cancelSelectable);
  Selectable::EndSequence();


  AddSelectable(_timeSelectable);
  AddSelectable(_delaySelectable);
  AddSelectable(_cancelSelectable);

  SetCurrentSelectable(_timeSelectable);
}

void StateSetStartStyle::Enter()
{
  auto config = Controller::GetInstance()->GetConfig();
  
  GetCurrentSelectable()->SetState(Selectable::FOCUSED);
  if(config->GetSessionStartStyle() == IntervalInfo::AT_TIME)
  {
    SetCurrentSelectable(_timeSelectable);
  }
  else
  {
    SetCurrentSelectable(_delaySelectable);    
  }

  _redrawRequired = true;
}

void StateSetStartStyle::Update(bool forceRedraw)
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
    display->println(F("START")); 

    DrawAllSelectables(display);

    auto config = Controller::GetInstance()->GetConfig();

    //display->drawBitmap(0, _immediateSelectable->GetPositionY(), config->GetSessionStartStyle() == IntervalInfo::IMMEDIATELY ? icon_true : icon_false, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(0, _timeSelectable->GetPositionY(), config->GetSessionStartStyle() == IntervalInfo::AT_TIME ? icon_true : icon_false, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(0, _delaySelectable->GetPositionY(), config->GetSessionStartStyle() == IntervalInfo::AFTER_DELAY ? icon_true : icon_false, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);

    display->fillRect(0, 0, SCREEN_WIDTH, SELECTABLE_SPACING_1, SSD1306_INVERSE);

    display->display();
  }
}

void StateSetStartStyle::Exit()
{}

void StateSetStartStyle::HandleEncoder(EncoderButton& eb)
{
  IncrementCurrentSelectable(eb.increment());
}

void StateSetStartStyle::HandleClick(EncoderButton& eb)
{
  if(GetCurrentSelectable() == _timeSelectable)
  {
    StateSetClock::GetInstance()->SetCompleteCallback([](bool didCancel)
    {
      if(!didCancel)
      {
        Controller::GetInstance()->GetConfig()->SetSessionStartStyle(IntervalInfo::AT_TIME);

        Controller::GetInstance()->GetConfig()->SetStartTime(StateSetClock::GetInstance()->GetHours(),
                                                            StateSetClock::GetInstance()->GetMinutes());

        Controller::GetInstance()->SetState(Controller::IDLE, SHORT_FLASH_TIME); 
      }
      else
      {
        Controller::GetInstance()->SetState(Controller::SET_START_STYLE, SHORT_FLASH_TIME); 
      }
    });

    StateSetClock::GetInstance()->SetTitle("START TIME");
    StateSetClock::GetInstance()->SetTime(Controller::GetInstance()->GetConfig()->GetStartTimeHours(), 
                                          Controller::GetInstance()->GetConfig()->GetStartTimeMinutes());
    StateSetClock::GetInstance()->SetCanCancel(true);

    GetCurrentSelectable()->SetState(Selectable::SELECTED);                                                      
    Controller::GetInstance()->SetState(Controller::SET_CLOCK, SHORT_FLASH_TIME);  
  }
  else if(GetCurrentSelectable() == _delaySelectable)
  {

    StateSetTimeInterval::GetInstance()->SetCompleteCallback([](bool didCancel)
    {
      if(!didCancel)
      {    
        Controller::GetInstance()->GetConfig()->SetSessionStartStyle(IntervalInfo::AFTER_DELAY);
        Controller::GetInstance()->GetConfig()->SetStartDelay(StateSetTimeInterval::GetInstance()->GetHours(),
                                                            StateSetTimeInterval::GetInstance()->GetMinutes(),
                                                            StateSetTimeInterval::GetInstance()->GetSeconds());
        
        Controller::GetInstance()->SetState(Controller::IDLE, SHORT_FLASH_TIME);                                             
      }
      else
      {
        Controller::GetInstance()->SetState(Controller::SET_START_STYLE, SHORT_FLASH_TIME);                                             
      }

    });

    StateSetTimeInterval::GetInstance()->SetTitle("DELAY");
    StateSetTimeInterval::GetInstance()->SetTime(Controller::GetInstance()->GetConfig()->GetStartDelayHours(), 
                                                Controller::GetInstance()->GetConfig()->GetStartDelayMinutes(),  
                                                Controller::GetInstance()->GetConfig()->GetStartDelaySeconds());
    
    GetCurrentSelectable()->SetState(Selectable::SELECTED);                                                      
    Controller::GetInstance()->SetState(Controller::SET_TIME_INTERVAL, SHORT_FLASH_TIME);      
  } 
  else
  {
    GetCurrentSelectable()->SetState(Selectable::SELECTED);                                                      
    Controller::GetInstance()->SetState(Controller::IDLE, SHORT_FLASH_TIME);  
  }
}

