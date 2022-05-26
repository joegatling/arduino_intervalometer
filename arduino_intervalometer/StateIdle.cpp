#include "StateIdle.h"
#include "StateSetTimeInterval.h"
#include "Controller.h"

// 'Left Icons', 9x55px

// #define ICONS_WIDTH 9
// #define ICONS_HEIGHT 55

#define SETTINGS_Y_OFFSET 24


char StateIdle::__clockString[] = "\0";

char StateIdle::__delayString[] = "\0";
char StateIdle::__shutterString[] = "\0";
char StateIdle::__intervalString[] = "\0";
char StateIdle::__durationString[] = "\0";

StateIdle::StateIdle()
{
  strcpy(__clockString, "\0");

  _beginSelectable = new Selectable(SCREEN_WIDTH, SCREEN_HEIGHT, "GO");
  _beginSelectable->SetAlignment(Selectable::BOTTOM_RIGHT);
  _beginSelectable->SetTextScale(3);

  _setClockSelectable = new Selectable(SCREEN_WIDTH, 0, GetClockString);
  _setClockSelectable->SetAlignment(Selectable::TOP_RIGHT);

  _startSettingsSelectable = new Selectable(ICON_WIDTH + 2, SETTINGS_Y_OFFSET, GetDelayString);
  _startSettingsSelectable->SetAlignment(Selectable::TOP_LEFT);

  _shutterSettingsSelectable = new Selectable(ICON_WIDTH + 2, SETTINGS_Y_OFFSET + SELECTABLE_SPACING_1 * 1, GetShutterString);
  _shutterSettingsSelectable->SetAlignment(Selectable::TOP_LEFT);

  _intervalSettingsSelectable = new Selectable(ICON_WIDTH + 2, SETTINGS_Y_OFFSET + SELECTABLE_SPACING_1 * 2, GetIntervalString);
  _intervalSettingsSelectable->SetAlignment(Selectable::TOP_LEFT);

  _endSettingsSelectable = new Selectable(ICON_WIDTH + 2, SETTINGS_Y_OFFSET + SELECTABLE_SPACING_1 * 3, GetDurationString);
  _endSettingsSelectable->SetAlignment(Selectable::TOP_LEFT);

  Selectable::LinkInSequence(_setClockSelectable);
  Selectable::LinkInSequence(_startSettingsSelectable);
  Selectable::LinkInSequence(_shutterSettingsSelectable);
  Selectable::LinkInSequence(_intervalSettingsSelectable);
  Selectable::LinkInSequence(_endSettingsSelectable);
  Selectable::LinkInSequence(_beginSelectable);

  AddSelectable(_beginSelectable);
  AddSelectable(_setClockSelectable);
  AddSelectable(_setClockSelectable);
  AddSelectable(_shutterSettingsSelectable);
  AddSelectable(_intervalSettingsSelectable);
  AddSelectable(_startSettingsSelectable);
  AddSelectable(_endSettingsSelectable);

  SetCurrentSelectable(_beginSelectable);
}

void StateIdle::Enter()
{
  // if(GetCurrentSelectable() == _shutterSettingsSelectable)
  // {
  //   Controller::GetInstance()->GetConfig()->SetShutter(StateSetTimeInterval::GetInstance()->GetHours(),
  //                                                     StateSetTimeInterval::GetInstance()->GetMinutes(),
  //                                                     StateSetTimeInterval::GetInstance()->GetSeconds());
  // }
  // else if(GetCurrentSelectable() == _intervalSettingsSelectable)
  // {
  //   Controller::GetInstance()->GetConfig()->SetInterval(StateSetTimeInterval::GetInstance()->GetHours(),
  //                                                     StateSetTimeInterval::GetInstance()->GetMinutes(),
  //                                                     StateSetTimeInterval::GetInstance()->GetSeconds());
  // }  
  // else if(GetCurrentSelectable() == _endSettingsSelectable)
  // {
  //   if(Controller::GetInstance()->GetConfig()->GetSessionEndStyle() == IntervalInfo::TOTAL_DURATION)
  //   {
  //     Controller::GetInstance()->GetConfig()->SetDuration(StateSetTimeInterval::GetInstance()->GetHours(),
  //                                                       StateSetTimeInterval::GetInstance()->GetMinutes(),
  //                                                       StateSetTimeInterval::GetInstance()->GetSeconds());
  //   }
  // }

  _redrawRequired = true;
}

void StateIdle::Update()
{
  if(UpdateAllSelectables() || _redrawRequired)
  {
    _redrawRequired = false;
    Adafruit_SSD1306* display = Controller::GetInstance()->GetDisplay();

    display->clearDisplay();  
    display->setTextColor(SSD1306_WHITE);  

    display->setTextSize(1);    
    display->setCursor(0,0);    

    DrawAllSelectables(display);

    display->drawBitmap(0, SETTINGS_Y_OFFSET, icon_start, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(0, SETTINGS_Y_OFFSET + SELECTABLE_SPACING_1, icon_shutter, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(0, SETTINGS_Y_OFFSET + SELECTABLE_SPACING_1*2, icon_interval, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(0, SETTINGS_Y_OFFSET + SELECTABLE_SPACING_1*3, icon_count, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);

    display->fillRect(0, 0, SCREEN_WIDTH, SELECTABLE_SPACING_1, SSD1306_INVERSE);

    display->display();
  }
}

void StateIdle::Exit()
{}

void StateIdle::HandleEncoder(EncoderButton& eb)
{
  if(_endSettingsSelectable->GetState() == Selectable::SELECTED)
  {
    int16_t increment = eb.increment()*eb.increment()*eb.increment();
    // if(eb.increment() < 0)
    // {
    //   increment = -increment;      
    // }
    Controller::GetInstance()->GetConfig()->SetExposureCount(Controller::GetInstance()->GetConfig()->GetExposureCount() + increment);
  }
  else
  {
    IncrementCurrentSelectable(eb.increment());
  }
}

void StateIdle::HandleClick(EncoderButton& eb)
{
  if(GetCurrentSelectable() == _setClockSelectable)
  {
    Controller::GetInstance()->SetState(Controller::SET_CLOCK);  
  }
  else if(GetCurrentSelectable() == _shutterSettingsSelectable)
  {
    StateSetTimeInterval::GetInstance()->SetCompleteCallback([](bool didCancel)
    {
      if(!didCancel)
      {
        Controller::GetInstance()->GetConfig()->SetShutter(StateSetTimeInterval::GetInstance()->GetHours(),
                                                          StateSetTimeInterval::GetInstance()->GetMinutes(),
                                                          StateSetTimeInterval::GetInstance()->GetSeconds());        
      }
      
      Controller::GetInstance()->SetState(Controller::IDLE);                                             
    });
    
    StateSetTimeInterval::GetInstance()->SetTitle("SHUTTER");
    StateSetTimeInterval::GetInstance()->SetTime(Controller::GetInstance()->GetConfig()->GetShutterHours(), 
                                                Controller::GetInstance()->GetConfig()->GetShutterMinutes(),  
                                                Controller::GetInstance()->GetConfig()->GetShutterSeconds());
                                                          
    Controller::GetInstance()->SetState(Controller::SET_TIME_INTERVAL);                                           
  }
  else if(GetCurrentSelectable() == _intervalSettingsSelectable)
  {
    StateSetTimeInterval::GetInstance()->SetCompleteCallback([](bool didCancel)
    {
      if(!didCancel)
      {    
        Controller::GetInstance()->GetConfig()->SetInterval(StateSetTimeInterval::GetInstance()->GetHours(),
                                                          StateSetTimeInterval::GetInstance()->GetMinutes(),
                                                          StateSetTimeInterval::GetInstance()->GetSeconds());
      }

      Controller::GetInstance()->SetState(Controller::IDLE);                                             
    });

    StateSetTimeInterval::GetInstance()->SetTitle("INTERVAL");
    StateSetTimeInterval::GetInstance()->SetTime(Controller::GetInstance()->GetConfig()->GetIntervalHours(), 
                                                Controller::GetInstance()->GetConfig()->GetIntervalMinutes(),  
                                                Controller::GetInstance()->GetConfig()->GetIntervalSeconds());

    Controller::GetInstance()->SetState(Controller::SET_TIME_INTERVAL);                                           
  }  
  else if(GetCurrentSelectable() == _endSettingsSelectable)
  {
    if(_endSettingsSelectable->GetState() == Selectable::SELECTED)
    {
      _endSettingsSelectable->SetState(Selectable::FOCUSED);  
      //_endSettingsSelectable->SetTextScale(1);
    }
    else
    {
      _endSettingsSelectable->SetState(Selectable::SELECTED);
      //_endSettingsSelectable->SetTextScale(2);
    }
  }
}

char* StateIdle::GetClockString()
{
  sprintf(__clockString, "%02d:%02d", Controller::GetInstance()->GetRTC()->getHours(), Controller::GetInstance()->GetRTC()->getMinutes());
  return __clockString;  
}

char* StateIdle::GetDelayString()
{
  Controller::GetInstance()->GetConfig()->GenerateInitialDelayString(__delayString);
  return __delayString;    
}

char* StateIdle::GetShutterString()
{
  Controller::GetInstance()->GetConfig()->GenerateShutterString(__shutterString);
  return __shutterString;      
}

char* StateIdle::GetIntervalString()
{
  Controller::GetInstance()->GetConfig()->GenerateIntervalString(__intervalString);
  return __intervalString;    
}

char* StateIdle::GetDurationString()
{
  Controller::GetInstance()->GetConfig()->GenerateDurationString(__durationString);
  return __durationString;      
}


