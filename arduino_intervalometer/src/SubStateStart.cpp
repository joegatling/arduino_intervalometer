#include "SubStateStart.h"
#include "Controller.h"
#include "IntervalInfo.h"
#include "StateRunning.h"
#include <time.h>

char SubStateStart::_displayString[] = "\0";

SubStateStart::SubStateStart()
{}

void SubStateStart::Enter()
{
  auto config = Controller::GetInstance()->GetConfig();
  RTCZero* rtc = Controller::GetInstance()->GetRTC();

  // Reset the date
  rtc->setDate(1,1,1);

  if(config->GetSessionStartStyle() == IntervalInfo::AT_TIME)
  {
    rtc->setAlarmTime(config->GetStartTimeHours(), config->GetStartTimeMinutes(), 0);
	  rtc->enableAlarm(rtc->MATCH_HHMMSS);
  }
  else
  {
    struct tm t;
    t.tm_sec = rtc->getSeconds() + config->GetStartDelaySeconds()-1;
    t.tm_min = rtc->getMinutes() + config->GetStartDelayMinutes();
    t.tm_hour = rtc->getHours() + config->GetStartDelayHours();
    t.tm_mday = rtc->getDay();
    t.tm_mon = rtc->getMonth();
    t.tm_year = rtc->getYear();

    mktime(&t);

    rtc->setAlarmTime(t.tm_hour, t.tm_min, t.tm_sec);
    rtc->setAlarmDate(t.tm_mday, t.tm_mon, t.tm_year);
	  rtc->enableAlarm(rtc->MATCH_YYMMDDHHMMSS);
  }

  rtc->attachInterrupt(OnAlarm);

  _isComplete = false;
}

void SubStateStart::Update()
{
  if(_isComplete)
  {
    StateRunning::GetInstance()->SetSubState(StateRunning::SHUTTER);
  }
}

void SubStateStart::Exit()
{
  auto config = Controller::GetInstance()->GetConfig();
  config->GenerateStartDelayString(_displayString);

  RTCZero* rtc = Controller::GetInstance()->GetRTC();
  rtc->detachInterrupt();
  rtc->disableAlarm();
}

char* SubStateStart::GetDisplayString(bool force)
{
  if(force || StateRunning::GetInstance()->GetCurrentSubState() == this)
  {
    auto config = Controller::GetInstance()->GetConfig();

    if(StateRunning::GetInstance()->GetCurrentSubState() != this)
    {
      config->GenerateStartDelayString(_displayString);
    }
    else
    {
      if(config->GetSessionStartStyle() == IntervalInfo::AFTER_DELAY)
      {
        if(GetTimeInState() > config->GetTotalStartDelayMillis())
        {
          sprintf(_displayString, "0s");
        }
        else
        { 
          long seconds = ceil((config->GetTotalStartDelayMillis() - GetTimeInState()) / 1000.0f);
      
          uint8_t minutes = (seconds / 60) % 60;
          uint8_t hours = (seconds / 3600);

          config->GenerateTimeString(_displayString, hours, minutes, (uint8_t)seconds % 60, true);    
        }
      }
      else
      {
        config->GenerateStartDelayString(_displayString);
      }    
    }
  }

  return _displayString;
}

void SubStateStart::OnAlarm()
{
  Controller::GetInstance()->WakeUp(false);

  RTCZero* rtc = Controller::GetInstance()->GetRTC();
  rtc->detachInterrupt();
  rtc->disableAlarm();

  SubStateStart* startState = (SubStateStart*)StateRunning::GetInstance()->GetSubState(StateRunning::START);
  startState->SetIsComplete(true);
}
