#include <cstdio>

#include "IntervalInfo.h"
#include "Controller.h"

#define MAX_EXPOSURE_COUNT 10000

IntervalInfo::IntervalInfo() 
{
  _data.isValid = true;

  _data.startTimeHours = 5;
  _data.startTimeMinutes = 0;

  _data.startDelaySeconds = 0;
  _data.startDelayMinutes = 0;
  _data.startDelayHours = 0;
  
  _data.shutterSeconds = 0;
  _data.shutterMinutes = 0;
  _data.shutterHours = 0;

  _data.intervalSeconds = 5;
  _data.intervalMinutes = 0;
  _data.intervalHours = 0;

  _data.sessionStartStyle = (int)IntervalInfo::AFTER_DELAY;

  _data.exposureCount = 10;
}

void IntervalInfo::SetStartDelay(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  _data.startDelayHours = hours;
  _data.startDelayMinutes = minutes;
  _data.startDelaySeconds = seconds;

  _isSaveDataDirty = true;
}

void IntervalInfo::SetStartTime(uint8_t hours, uint8_t minutes)
{
  _data.startTimeHours = hours;
  _data.startTimeMinutes = minutes;

  _isSaveDataDirty = true;
}

void IntervalInfo::SetShutter(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  _data.shutterHours = hours;
  _data.shutterMinutes = minutes;
  _data.shutterSeconds = seconds;

  _isSaveDataDirty = true;
}

void IntervalInfo::SetInterval(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
  _data.intervalHours = hours;
  _data.intervalMinutes = minutes;
  _data.intervalSeconds = seconds;

  if(GetTotalIntervalMillis() == 0)
  {
    _data.intervalSeconds = 1;
  }

  _isSaveDataDirty = true;
}

// void IntervalInfo::SetDuration(uint8_t hours, uint8_t minutes, uint8_t seconds)
// {
//   _durationHours = hours;
//   _durationMinutes = minutes;
//   _durationSeconds = seconds;

//   _sessionEndStyle = IntervalInfo::TOTAL_DURATION;
// }

void IntervalInfo::SetExposureCount(int16_t count) 
{ 
  //_sessionEndStyle = TOTAL_EXPOSURE_COUNT; 

  _data.exposureCount = count;

  while(_data.exposureCount < 0)
  {
    _data.exposureCount += MAX_EXPOSURE_COUNT;
  }

  _data.exposureCount = _data.exposureCount % MAX_EXPOSURE_COUNT;

  _isSaveDataDirty = true;
}
    
void IntervalInfo::GenerateStartDelayString(char* destination, bool showZero)
{
  if(_data.sessionStartStyle == IntervalInfo::AFTER_DELAY)
  {
    if(showZero == false && GetTotalStartDelayMillis() == 0)
    {
      sprintf(destination, "-");
    }
    else
    {
      GenerateTimeString(destination, _data.startDelayHours, _data.startDelayMinutes, _data.startDelaySeconds, showZero);
    }
  }
  else
  {
    Controller::GetInstance()->GenerateTimeString(destination, _data.startTimeHours, _data.startTimeMinutes);
  }
}

void IntervalInfo::GenerateShutterString(char* destination)
{
  GenerateTimeString(destination, _data.shutterHours, _data.shutterMinutes, _data.shutterSeconds);
}

void IntervalInfo::GenerateIntervalString(char* destination)
{
  GenerateTimeString(destination, _data.intervalHours, _data.intervalMinutes, _data.intervalSeconds);  
}

void IntervalInfo::GenerateDurationString(char* destination)
{
  sprintf(destination, "%d", _data.exposureCount);

  // //auto endStyle = GetSessionEndStyle();

  // if(endStyle == IntervalInfo::TOTAL_DURATION)
  // {
  //   GenerateTimeString(destination, _durationHours, _durationMinutes, _durationSeconds);  
  // }
  // else if(endStyle == IntervalInfo::TOTAL_EXPOSURE_COUNT)
  // {
  //   sprintf(destination, "%d", _exposureCount);
  // }
  // else if(endStyle == IntervalInfo::NO_END)
  // {
  //   sprintf(destination, "-");
  // }  
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

void IntervalInfo::SetSaveData(IntervalometerSaveData data)
{
  _data.startTimeHours = data.startTimeHours;
  _data.startTimeMinutes = data.startTimeMinutes;

  _data.startDelaySeconds = data.startDelaySeconds;
  _data.startDelayMinutes = data.startDelayMinutes;
  _data.startDelayHours = data.startDelayHours;
  
  _data.shutterSeconds = data.shutterSeconds;
  _data.shutterMinutes = data.shutterMinutes;
  _data.shutterHours = data.shutterHours;

  _data.intervalSeconds = data.intervalSeconds;
  _data.intervalMinutes = data.intervalMinutes;
  _data.intervalHours = data.intervalHours;

  _data.sessionStartStyle = data.sessionStartStyle;

  _data.exposureCount = data.exposureCount;  
}
