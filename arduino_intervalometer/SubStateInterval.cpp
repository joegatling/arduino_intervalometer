#include "SubStateInterval.h"
#include "Controller.h"
#include "StateRunning.h"

char SubStateInterval::_displayString[] = "\0";

SubStateInterval::SubStateInterval()
{}

void SubStateInterval::Enter()
{}

void SubStateInterval::Update()
{
  auto config = Controller::GetInstance()->GetConfig();
  if(GetTimeInState() > config->GetTotalIntervalMillis())
  {
    StateRunning::GetInstance()->SetSubState(StateRunning::SHUTTER);
  }
}

void SubStateInterval::Exit()
{
  auto config = Controller::GetInstance()->GetConfig();
  config->GenerateIntervalString(_displayString);
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