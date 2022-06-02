#include <cstdio>

#include "IntervalInfo.h"
#include "Controller.h"

#define MAX_EXPOSURE_COUNT 10000

IntervalInfo::IntervalInfo() 
{
  _intervalSeconds = 10;
}

void IntervalInfo::SetStartDelay(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  _startDelayHours = hours;
  _startDelayMinutes = minutes;
  _startDelaySeconds = seconds;
}

void IntervalInfo::SetStartTime(uint8_t hours, uint8_t minutes)
{
  _startTimeHours = hours;
  _startTimeMinutes = minutes;
}

void IntervalInfo::SetShutter(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  _shutterHours = hours;
  _shutterMinutes = minutes;
  _shutterSeconds = seconds;
}

void IntervalInfo::SetInterval(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  _intervalHours = hours;
  _intervalMinutes = minutes;
  _intervalSeconds = seconds;

  if(GetTotalIntervalMillis() == 0)
  {
    _intervalSeconds = 1;
  }
}

void IntervalInfo::SetDuration(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  _durationHours = hours;
  _durationMinutes = minutes;
  _durationSeconds = seconds;

  _sessionEndStyle = IntervalInfo::TOTAL_DURATION;
}

void IntervalInfo::SetExposureCount(int count) 
{ 
  _sessionEndStyle = TOTAL_EXPOSURE_COUNT; 

  _exposureCount = count;

  while(_exposureCount < 0)
  {
    _exposureCount += MAX_EXPOSURE_COUNT;
  }

  _exposureCount = _exposureCount % MAX_EXPOSURE_COUNT;
}
    
void IntervalInfo::GenerateStartDelayString(char* destination, bool showZero)
{
  if(_sessionStartStyle == IntervalInfo::AFTER_DELAY)
  {
    if(showZero == false && GetTotalStartDelayMillis() == 0)
    {
      sprintf(destination, "-");
    }
    else
    {
      GenerateTimeString(destination, _startDelayHours, _startDelayMinutes, _startDelaySeconds, showZero);
    }
  }
  else
  {
    Controller::GetInstance()->GenerateTimeString(destination, _startTimeHours, _startTimeMinutes);
  }
}

void IntervalInfo::GenerateShutterString(char* destination)
{
  GenerateTimeString(destination, _shutterHours, _shutterMinutes, _shutterSeconds);
}

void IntervalInfo::GenerateIntervalString(char* destination)
{
  GenerateTimeString(destination, _intervalHours, _intervalMinutes, _intervalSeconds);  
}

void IntervalInfo::GenerateDurationString(char* destination)
{
  auto endStyle = GetSessionEndStyle();

  if(endStyle == IntervalInfo::TOTAL_DURATION)
  {
    GenerateTimeString(destination, _durationHours, _durationMinutes, _durationSeconds);  
  }
  else if(endStyle == IntervalInfo::TOTAL_EXPOSURE_COUNT)
  {
    sprintf(destination, "%d", _exposureCount);
  }
  else if(endStyle == IntervalInfo::NO_END)
  {
    sprintf(destination, "-");
  }  
}

void IntervalInfo::GenerateTimeString(char* destination, uint8_t hours, uint8_t minutes, uint8_t seconds, bool showZero)
{
  if(hours > 0)
  {
    if(showZero || seconds > 0)
    {
      sprintf(destination, "%dh%02dm%02ds", hours, minutes, seconds);
    }
    else if(minutes > 0)
    {
      sprintf(destination, "%dh%02dm", hours, minutes);
    }
    else
    {
      sprintf(destination, "%dh", hours);
    }
  }
  else if(minutes > 0)
  {
    if(showZero || seconds > 0)
    {
      sprintf(destination, "%dm%02ds", minutes, seconds);
    }
    else
    {
      sprintf(destination, "%dm", minutes);
    }
  } 
  else if(showZero || seconds > 0)
  {
      sprintf(destination, "%ds", seconds);
  } 
  else
  {
    sprintf(destination, "-");
  }
}
