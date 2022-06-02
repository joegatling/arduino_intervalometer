#include "SubStateShutter.h"
#include "Controller.h"
#include "StateRunning.h"

#define SIMPLE_SHUTTER_MILLIS 250

char SubStateShutter::_displayString[] = "\0";

SubStateShutter::SubStateShutter()
{}

void SubStateShutter::Enter()
{
  //TODO Trigger Shutter Pin
}

void SubStateShutter::Update()
{
  bool isDone = false;
  auto config = Controller::GetInstance()->GetConfig();
  
  if(config->GetTotalShutterMillis() == 0)
  {
    isDone = (GetTimeInState() > SIMPLE_SHUTTER_MILLIS);
  }
  else
  {
    isDone = (GetTimeInState() > config->GetTotalShutterMillis());
  }

  if(isDone)
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

  //TODO Untrigger Shutter Pin
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

