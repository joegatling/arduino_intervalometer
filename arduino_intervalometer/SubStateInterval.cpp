#include "SubStateInterval.h"
#include "Controller.h"
#include "StateRunning.h"

char SubStateInterval::_displayString[] = "\0";

SubStateInterval::SubStateInterval()
{}

void SubStateInterval::Enter()
{
  auto config = Controller::GetInstance()->GetConfig();
  RTCZero* rtc = Controller::GetInstance()->GetRTC();

  // Reset the date
  rtc->setDate(1,1,1);

  struct tm t;
  t.tm_sec = rtc->getSeconds() + config->GetIntervalSeconds() - 1;
  t.tm_min = rtc->getMinutes() + config->GetIntervalMinutes();
  t.tm_hour = rtc->getHours() + config->GetIntervalHours();
  t.tm_mday = rtc->getDay();
  t.tm_mon = rtc->getMonth();
  t.tm_year = rtc->getYear();

  mktime(&t);

  rtc->setAlarmTime(t.tm_hour, t.tm_min, t.tm_sec);
  rtc->setAlarmDate(t.tm_mday, t.tm_mon, t.tm_year);
  rtc->enableAlarm(rtc->MATCH_YYMMDDHHMMSS);
  
  rtc->attachInterrupt(OnAlarm);
  

  _isComplete = false;    
}

void SubStateInterval::Update()
{
  if(_isComplete)
  {
    StateRunning::GetInstance()->SetSubState(StateRunning::SHUTTER);
  }
}

void SubStateInterval::Exit()
{
  auto config = Controller::GetInstance()->GetConfig();
  config->GenerateIntervalString(_displayString);

  RTCZero* rtc = Controller::GetInstance()->GetRTC();
  rtc->detachInterrupt();
  rtc->disableAlarm();  
}

char* SubStateInterval::GetDisplayString(bool force)
{

  if(force || StateRunning::GetInstance()->GetCurrentSubState() == this)
  {    
    auto config = Controller::GetInstance()->GetConfig();
    if(StateRunning::GetInstance()->GetCurrentSubState() != this)
    {
      config->GenerateIntervalString(_displayString);
    }
    else
    {
      if(GetTimeInState() > config->GetTotalIntervalMillis())
      {
        sprintf(_displayString, "0s");
      }
      else
      { 
        long seconds = ceil((config->GetTotalIntervalMillis() - GetTimeInState()) / 1000.0f);
    
        uint8_t minutes = (seconds / 60) % 60;
        uint8_t hours = (seconds / 3600);

        config->GenerateTimeString(_displayString, hours, minutes, (uint8_t)seconds % 60, true);    
      }
    }
  }

  return _displayString; 
}

void SubStateInterval::OnAlarm()
{
  Controller::GetInstance()->WakeUp(false);

  RTCZero* rtc = Controller::GetInstance()->GetRTC();
  rtc->detachInterrupt();
  rtc->disableAlarm();

  SubStateInterval* s = (SubStateInterval*)StateRunning::GetInstance()->GetSubState(StateRunning::INTERVAL);
  s->SetIsComplete(true);
}