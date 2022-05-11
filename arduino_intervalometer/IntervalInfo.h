#ifndef INVERVAL_INFO_H
#define INTERVAL_INFO_H

#include <ctime>

enum SessionEndStyle
{
  NO_END = 0,
  TOTAL_EXPOSURE_COUNT,
  TOTAL_DURATION,
  DATE_TIME
};

class IntervalInfo
{
  public:  
    float GetInitialDelay() { return _initialDelay; };
    float GetFocusTime() { return _focusTime; };
    float GetShutterTime() { return _shutterTime; };
    float GetExposureInterval() { return _exposureInterval; };
    
    SessionEndStyle GetSessionEndStyle() { return _sessionEndStyle; };
    int GetExposureCount() { _sessionEndStyle == TOTAL_EXPOSURE_COUNT ? _exposureCount : 0; };
    float GetTotalDuration() { _sessionEndStyle == TOTAL_DURATION ? _totalDuration : 0; };
    time_t GetEndDateTime() { return _endDateTime; };




  private:
    float _initialDelay = 0;
    float _focusTime = 0;
    float _shutterTime = 0;
    float _exposureInterval = 1000;

    SessionEndStyle _sessionEndStyle = NO_END;
    int _exposureCount = -1;
    float _totalDuration = -1;
    
    time_t _endDateTime;

};

#endif //INTERVAL_INFO_H