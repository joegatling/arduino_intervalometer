#include "SubStateShutter.h"
#include "Controller.h"
#include "StateRunning.h"

#define SIMPLE_SHUTTER_MILLIS 250

char SubStateShutter::_displayString[] = "\0";

SubStateShutter::SubStateShutter()
{}

void SubStateShutter::Enter()
{
  //Trigger Shutter Pin
  Controller::GetInstance()->SetFocus(true);
  Controller::GetInstance()->SetShutter(true);

  auto config = Controller::GetInstance()->GetConfig();

  if(config->GetTotalShutterMillis() > 0)
  {
    RTCZero* rtc = Controller::GetInstance()->GetRTC();

    // Reset the date
    rtc->setDate(1,1,1);

    struct tm t;
    t.tm_sec = rtc->getSeconds() + config->GetShutterSeconds() - 1;
    t.tm_min = rtc->getMinutes() + config->GetShutterMinutes();
    t.tm_hour = rtc->getHours() + config->GetShutterHours();
    t.tm_mday = rtc->getDay();
    t.tm_mon = rtc->getMonth();
    t.tm_year = rtc->getYear();

    mktime(&t);

    rtc->setAlarmTime(t.tm_hour, t.tm_min, t.tm_sec);
    rtc->setAlarmDate(t.tm_mday, t.tm_mon, t.tm_year);
    rtc->enableAlarm(rtc->MATCH_YYMMDDHHMMSS);
    

    rtc->attachInterrupt(OnAlarm);
  }

  _isComplete = false;  

  Controller::GetInstance()->SetOverrideLedState(Controller::GetInstance()->GetDisplayState());

  _isBlinking = true;
}

void SubStateShutter::Update()
{
  if(_isBlinking && GetTimeInState() > 200)
  {
    _isBlinking = false;
    Controller::GetInstance()->ClearOverrideLedState();
  }

  auto config = Controller::GetInstance()->GetConfig();
  
  if(config->GetTotalShutterMillis() == 0)
  {
    _isComplete = (GetTimeInState() > SIMPLE_SHUTTER_MILLIS);
  }

  if(_isComplete)
  {
    if(config->GetExposureCount() == 0)
    {
      StateRunning::GetInstance()->SetSubState(StateRunning::INTERVAL);
    }
    else
    {
      unsigned int count = StateRunning::GetInstance()->IncrementExposureCount();

      if(count >= config->GetExposureCount())
      {
        StateRunning::GetInstance()->SetSubState(StateRunning::FINISHED);
      }
      else
      {
        StateRunning::GetInstance()->SetSubState(StateRunning::INTERVAL);
      }
    }
  }
}

void SubStateShutter::Exit()
{
  auto config = Controller::GetInstance()->GetConfig();
  config->GenerateShutterString(_displayString);

  //Untrigger Shutter Pin
  Controller::GetInstance()->SetShutter(false);
  Controller::GetInstance()->SetFocus(false);

  Controller::GetInstance()->ClearOverrideLedState();

  RTCZero* rtc = Controller::GetInstance()->GetRTC();
  rtc->detachInterrupt();
  rtc->disableAlarm();  
}

char* SubStateShutter::GetDisplayString(bool force)
{
  if(force || StateRunning::GetInstance()->GetCurrentSubState() == this)
  {
    auto config = Controller::GetInstance()->GetConfig();

    if(StateRunning::GetInstance()->GetCurrentSubState() != this)
    {
      config->GenerateShutterString(_displayString);
    }
    else
    {
      if(config->GetTotalShutterMillis())
      {
        if(GetTimeInState() > config->GetTotalShutterMillis())
        {
          sprintf(_displayString, "0s");
        }
        else
        { 
          long seconds = ceil((config->GetTotalShutterMillis() - GetTimeInState()) / 1000.0f);
      
          uint8_t minutes = (seconds / 60) % 60;
          uint8_t hours = (seconds / 3600);

          config->GenerateTimeString(_displayString, hours, minutes, (uint8_t)seconds % 60, true);    
        }
      }
    }
  }

  return _displayString; 
}

void SubStateShutter::OnAlarm()
{
  Controller::GetInstance()->WakeUp(false);

  RTCZero* rtc = Controller::GetInstance()->GetRTC();
  rtc->detachInterrupt();
  rtc->disableAlarm();

  SubStateShutter* s = (SubStateShutter*)StateRunning::GetInstance()->GetSubState(StateRunning::SHUTTER);
  s->SetIsComplete(true);
}
