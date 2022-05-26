#ifndef INTERVAL_INFO_H
#define INTERVAL_INFO_H

#include <ctime>
#include <Arduino.h>

#define MAX_TIME_STRING_LENGTH 12

// Terms:
// Initial Delay - The amount of time from the start of the session to the first exposure
// Shutter - The length of time of the shutter pulse
// Interval - The length of time between two exposures in the session
// Duration - the length of time of the session before it end

class IntervalInfo
{
  public:  
    enum SessionEndStyle
    {
      NO_END = 0,
      TOTAL_EXPOSURE_COUNT,
      TOTAL_DURATION,
      DATE_TIME
    };

    IntervalInfo();

    uint32_t GetTotalInitialDelay() { return (_initialDelayHours * 3600) + (_initialDelayMinutes * 60) + _initialDelaySeconds; };

    uint8_t GetInitialDelaySeconds() { return _initialDelaySeconds; };
    uint8_t GetInitialDelayMinutes() { return _initialDelayMinutes; };
    uint8_t GetInitialDelayHours() { return _initialDelayHours; };
    void SetInitialDelay(uint8_t hours, uint8_t minutes, uint8_t seconds);
    
    uint32_t GetTotalShutter() { return (_shutterHours * 3600) + (_shutterMinutes * 60) + _shutterSeconds; };

    uint8_t GetShutterSeconds() { return _shutterSeconds; };
    uint8_t GetShutterMinutes() { return _shutterMinutes; };
    uint8_t GetShutterHours() { return _shutterHours; };
    void SetShutter(uint8_t hours, uint8_t minutes, uint8_t seconds);
    
    uint32_t GetTotalInterval() { return (_intervalHours * 3600) + (_intervalMinutes * 60) + _intervalSeconds; };

    uint8_t GetIntervalSeconds() { return _intervalSeconds; };
    uint8_t GetIntervalMinutes() { return _intervalMinutes; };
    uint8_t GetIntervalHours() { return _intervalHours; };
    void SetInterval(uint8_t hours, uint8_t minutes, uint8_t seconds);

    uint32_t GetTotalDuration() { return (_durationHours * 3600) + (_durationMinutes * 60) + _durationSeconds; };

    uint8_t GetDurationSeconds() { return _durationSeconds; };
    uint8_t GetDurationMinutes() { return _durationMinutes; };
    uint8_t GetDurationHours() { return _durationHours; };   
    void SetDuration(uint8_t hours, uint8_t minutes, uint8_t seconds); 

    // float GetShutterTime() { return _shutterTime; };
    // float GetInterval() { return _exposureInterval; };
    
    SessionEndStyle GetSessionEndStyle() { return _sessionEndStyle; };
    void SetSessionEndStyle(SessionEndStyle endStyle) { _sessionEndStyle = endStyle; };
    
    int GetExposureCount() { return _sessionEndStyle == TOTAL_EXPOSURE_COUNT ? _exposureCount : 0; };
    void SetExposureCount(int count);
    //float GetTotalDuration() { return _sessionEndStyle == TOTAL_DURATION ? _totalDuration : 0; };
    time_t GetEndDateTime() { return _endDateTime; };

    void GenerateInitialDelayString(char* destination);
    void GenerateShutterString(char* destination);
    void GenerateIntervalString(char* destination);
    void GenerateDurationString(char* destination);

  private:
    void GenerateTimeString(char* destination, uint8_t hours, uint8_t minutes, uint8_t seconds);

    uint8_t _initialDelaySeconds = 0;
    uint8_t _initialDelayMinutes = 0;
    uint8_t _initialDelayHours = 0;
    
    uint8_t _shutterSeconds = 0;
    uint8_t _shutterMinutes = 0;
    uint8_t _shutterHours = 0;

    uint8_t _intervalSeconds = 0;
    uint8_t _intervalMinutes = 0;
    uint8_t _intervalHours = 0;

    SessionEndStyle _sessionEndStyle = TOTAL_EXPOSURE_COUNT;

    int _exposureCount = 10;

    uint8_t _durationSeconds = 0;
    uint8_t _durationMinutes = 0;
    uint8_t _durationHours = 0;
    
    time_t _endDateTime;

};

#endif //INTERVAL_INFO_H