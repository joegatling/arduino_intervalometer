#include <cstdio>

#include "IntervalInfo.h"

#define MAX_EXPOSURE_COUNT 10000

IntervalInfo::IntervalInfo() 
{
  _intervalSeconds = 10;
}

void IntervalInfo::SetInitialDelay(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  _initialDelayHours = hours;
  _initialDelayMinutes = minutes;
  _initialDelaySeconds = seconds;
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

  if(GetTotalInterval() == 0)
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
    

void IntervalInfo::GenerateInitialDelayString(char* destination)
{
  GenerateTimeString(destination, _initialDelayHours, _initialDelayMinutes, _initialDelaySeconds);
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

void IntervalInfo::GenerateTimeString(char* destination, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  if(hours > 0)
  {
    if(seconds > 0)
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
    if(seconds > 0)
    {
      sprintf(destination, "%dm%02ds", minutes, seconds);
    }
    else
    {
      sprintf(destination, "%dm", minutes);
    }
  } 
  else if(seconds > 0)
  {
      sprintf(destination, "%ds", seconds);
  } 
  else
  {
    sprintf(destination, "-");
  }
}
