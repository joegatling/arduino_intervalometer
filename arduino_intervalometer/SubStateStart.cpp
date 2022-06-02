#include "SubStateStart.h"
#include "Controller.h"
#include "IntervalInfo.h"
#include "StateRunning.h"

char SubStateStart::_displayString[] = "\0";

SubStateStart::SubStateStart()
{}

void SubStateStart::Enter()
{
  auto config = Controller::GetInstance()->GetConfig();

  if(config->GetSessionStartStyle() == IntervalInfo::AT_TIME)
  {
    RTCZero* rtc = Controller::GetInstance()->GetRTC();

    rtc->setAlarmTime(config->GetStartTimeHours(), config->GetStartTimeMinutes(), 0);
	  rtc->enableAlarm(rtc->MATCH_HHMMSS);
	  rtc->attachInterrupt(StartAlarm);
  }

  _isComplete = false;
}

void SubStateStart::Update()
{
  auto config = Controller::GetInstance()->GetConfig();
  if(config->GetSessionStartStyle() == IntervalInfo::AFTER_DELAY)
  {
    _isComplete = GetTimeInState() > config->GetTotalStartDelayMillis();
  }

  if(_isComplete)
  {
    StateRunning::GetInstance()->SetSubState(StateRunning::SHUTTER);
  }
}

void SubStateStart::Exit()
{
  auto config = Controller::GetInstance()->GetConfig();
  config->GenerateStartDelayString(_displayString);
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

void SubStateStart::StartAlarm()
{
  RTCZero* rtc = Controller::GetInstance()->GetRTC();
  rtc->detachInterrupt();
  rtc->disableAlarm();

  SubStateStart* startState = (SubStateStart*)StateRunning::GetInstance()->GetSubState(StateRunning::START);
  startState->SetIsComplete(true);
}
