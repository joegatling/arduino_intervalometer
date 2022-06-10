#include "StateIdle.h"
#include "StateSetTimeInterval.h"
#include "Controller.h"
#include "StateSetClock.h"

// 'Left Icons', 9x55px

// #define ICONS_WIDTH 9
// #define ICONS_HEIGHT 55

#define Y_OFFSET 24


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

  _setClockSelectable = new Selectable(SCREEN_WIDTH-1, 1, GetClockString);
  _setClockSelectable->SetAlignment(Selectable::TOP_RIGHT);

  _startSettingsSelectable = new Selectable(ICON_WIDTH + 2, Y_OFFSET, GetDelayString);
  _startSettingsSelectable->SetAlignment(Selectable::TOP_LEFT);

  _shutterSettingsSelectable = new Selectable(ICON_WIDTH + 2, Y_OFFSET + SELECTABLE_SPACING_1 * 1, GetShutterString);
  _shutterSettingsSelectable->SetAlignment(Selectable::TOP_LEFT);

  _intervalSettingsSelectable = new Selectable(ICON_WIDTH + 2, Y_OFFSET + SELECTABLE_SPACING_1 * 2, GetIntervalString);
  _intervalSettingsSelectable->SetAlignment(Selectable::TOP_LEFT);

  _endSettingsSelectable = new Selectable(ICON_WIDTH + 2, Y_OFFSET + SELECTABLE_SPACING_1 * 3, GetDurationString);
  _endSettingsSelectable->SetAlignment(Selectable::TOP_LEFT);

  Selectable::LinkInSequence(_setClockSelectable);
  Selectable::LinkInSequence(_startSettingsSelectable);
  Selectable::LinkInSequence(_shutterSettingsSelectable);
  Selectable::LinkInSequence(_intervalSettingsSelectable);
  Selectable::LinkInSequence(_endSettingsSelectable);
  Selectable::LinkInSequence(_beginSelectable);
  Selectable::EndSequence();

  AddSelectable(_beginSelectable);
  AddSelectable(_setClockSelectable);
  AddSelectable(_shutterSettingsSelectable);
  AddSelectable(_intervalSettingsSelectable);
  AddSelectable(_startSettingsSelectable);
  AddSelectable(_endSettingsSelectable);

  SetCurrentSelectable(_beginSelectable);
}

void StateIdle::Enter()
{
  _redrawRequired = true;
}

void StateIdle::Update(bool forceRedraw)
{
  _redrawRequired |= forceRedraw;

  const unsigned char* batteryIcon = Controller::GetInstance()->GetBatteryIconForCurrentVoltage();

  if(UpdateAllSelectables() || _redrawRequired || batteryIcon != _currentBatteryIcon)
  {
    _redrawRequired = false;
    Adafruit_SSD1306* display = Controller::GetInstance()->GetDisplay();

    display->clearDisplay();  
    display->setTextColor(SSD1306_WHITE);  

    display->setTextSize(1);    
    display->setCursor(0,0);    

    DrawAllSelectables(display);

    display->drawBitmap(0, Y_OFFSET, icon_start, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(0, Y_OFFSET + SELECTABLE_SPACING_1, icon_shutter, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(0, Y_OFFSET + SELECTABLE_SPACING_1*2, icon_interval, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(0, Y_OFFSET + SELECTABLE_SPACING_1*3, icon_count, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);

    _currentBatteryIcon = batteryIcon;
    display->drawBitmap(1,2,_currentBatteryIcon, BATTERY_ICON_WIDTH, BATTERY_ICON_HEIGHT, SSD1306_WHITE);
    // display->setCursor(0,Y_OFFSET + SELECTABLE_SPACING_1*4 + 5);
    // display->print(Controller::GetInstance()->GetBatteryVoltage());
    // display->print("v");

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
    StateSetClock::GetInstance()->SetCompleteCallback([](bool didCancel)
    {
      if(!didCancel)
      {
        Controller::GetInstance()->GetRTC()->setHours(StateSetClock::GetInstance()->GetHours());
        Controller::GetInstance()->GetRTC()->setMinutes(StateSetClock::GetInstance()->GetMinutes());
      }

      Controller::GetInstance()->SetState(Controller::IDLE);                                             
    });

    StateSetClock::GetInstance()->SetTitle("SET CLOCK");
    StateSetClock::GetInstance()->SetTime(Controller::GetInstance()->GetRTC()->getHours(), 
                                          Controller::GetInstance()->GetRTC()->getMinutes());
    StateSetClock::GetInstance()->SetCanCancel(true);

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
  else if(GetCurrentSelectable() == _startSettingsSelectable)
  {
    Controller::GetInstance()->SetState(Controller::SET_START_STYLE); 
  }
  else if(GetCurrentSelectable() == _beginSelectable)
  {
    Controller::GetInstance()->SetState(Controller::RUNNING);
  }
  
}

char* StateIdle::GetClockString()
{
  Controller::GetInstance()->GenerateTimeString(__clockString);
  return __clockString;  
}

char* StateIdle::GetDelayString()
{
  Controller::GetInstance()->GetConfig()->GenerateStartDelayString(__delayString);
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


