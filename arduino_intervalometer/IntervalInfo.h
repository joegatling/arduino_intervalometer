#ifndef INTERVAL_INFO_H
#define INTERVAL_INFO_H

#include <ctime>
#include <Arduino.h>

#define MAX_TIME_STRING_LENGTH 12

// Terms:
// Start Delay - The amount of time from the start of the session to the first exposure
// Shutter - The length of time of the shutter pulse
// Interval - The length of time between two exposures in the session
// Duration - the length of time of the session before it end

class IntervalInfo
{
  public: 
    enum SessionStartStyle
    {
      AT_TIME,
      AFTER_DELAY
    };

    enum SessionEndStyle
    {
      NO_END = 0,
      TOTAL_EXPOSURE_COUNT,
      TOTAL_DURATION,
      DATE_TIME
    };

    IntervalInfo();

    unsigned long GetTotalStartDelayMillis() { return ((_startDelayHours * 3600) + (_startDelayMinutes * 60) + _startDelaySeconds) * 1000; };

    uint8_t GetStartDelaySeconds() { return _startDelaySeconds; };
    uint8_t GetStartDelayMinutes() { return _startDelayMinutes; };
    uint8_t GetStartDelayHours() { return _startDelayHours; };
    void SetStartDelay(uint8_t hours, uint8_t minutes, uint8_t seconds);

    uint8_t GetStartTimeHours() { return _startTimeHours; };
    uint8_t GetStartTimeMinutes() { return _startTimeMinutes; };
    void SetStartTime(uint8_t hours, uint8_t minutes);
    
    unsigned long  GetTotalShutterMillis() { return ((_shutterHours * 3600) + (_shutterMinutes * 60) + _shutterSeconds) * 1000; };

    uint8_t GetShutterSeconds() { return _shutterSeconds; };
    uint8_t GetShutterMinutes() { return _shutterMinutes; };
    uint8_t GetShutterHours() { return _shutterHours; };
    void SetShutter(uint8_t hours, uint8_t minutes, uint8_t seconds);
    
    unsigned long  GetTotalIntervalMillis() { return ((_intervalHours * 3600) + (_intervalMinutes * 60) + _intervalSeconds) * 1000; };

    uint8_t GetIntervalSeconds() { return _intervalSeconds; };
    uint8_t GetIntervalMinutes() { return _intervalMinutes; };
    uint8_t GetIntervalHours() { return _intervalHours; };
    void SetInterval(uint8_t hours, uint8_t minutes, uint8_t seconds);

    unsigned long  GetTotalDurationMillis() { return ((_durationHours * 3600) + (_durationMinutes * 60) + _durationSeconds) * 1000; };

    uint8_t GetDurationSeconds() { return _durationSeconds; };
    uint8_t GetDurationMinutes() { return _durationMinutes; };
    uint8_t GetDurationHours() { return _durationHours; };   
    void SetDuration(uint8_t hours, uint8_t minutes, uint8_t seconds); 

    // float GetShutterTime() { return _shutterTime; };
    // float GetInterval() { return _exposureInterval; };

    SessionStartStyle GetSessionStartStyle() { return _sessionStartStyle; };
    void SetSessionStartStyle(SessionStartStyle startStyle) { _sessionStartStyle = startStyle; };
    
    SessionEndStyle GetSessionEndStyle() { return _sessionEndStyle; };
    void SetSessionEndStyle(SessionEndStyle endStyle) { _sessionEndStyle = endStyle; };

    
    int GetExposureCount() { return _sessionEndStyle == TOTAL_EXPOSURE_COUNT ? _exposureCount : 0; };
    void SetExposureCount(int count);
    //float GetTotalDuration() { return _sessionEndStyle == TOTAL_DURATION ? _totalDuration : 0; };
    time_t GetEndDateTime() { return _endDateTime; };

    void GenerateStartDelayString(char* destination, bool showZero = false);
    void GenerateShutterString(char* destination);
    void GenerateIntervalString(char* destination);
    void GenerateDurationString(char* destination);

    void GenerateTimeString(char* destination, uint8_t hours, uint8_t minutes, uint8_t seconds, bool showZero = false);

  private:

    uint8_t _startTimeHours = 0;
    uint8_t _startTimeMinutes = 0;

    uint8_t _startDelaySeconds = 0;
    uint8_t _startDelayMinutes = 0;
    uint8_t _startDelayHours = 0;
    
    uint8_t _shutterSeconds = 0;
    uint8_t _shutterMinutes = 0;
    uint8_t _shutterHours = 0;

    uint8_t _intervalSeconds = 0;
    uint8_t _intervalMinutes = 0;
    uint8_t _intervalHours = 0;

    SessionStartStyle _sessionStartStyle = AFTER_DELAY;
    SessionEndStyle _sessionEndStyle = TOTAL_EXPOSURE_COUNT;

    int _exposureCount = 10;

    uint8_t _durationSeconds = 0;
    uint8_t _durationMinutes = 0;
    uint8_t _durationHours = 0;
    
    time_t _endDateTime;

};

#endif //INTERVAL_INFO_H