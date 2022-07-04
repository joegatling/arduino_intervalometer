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

typedef struct
{
  bool isValid = false;

  uint8_t startTimeHours;
  uint8_t startTimeMinutes;

  uint8_t startDelaySeconds;
  uint8_t startDelayMinutes;
  uint8_t startDelayHours;
  
  uint8_t shutterSeconds;
  uint8_t shutterMinutes;
  uint8_t shutterHours;

  uint8_t intervalSeconds;
  uint8_t intervalMinutes;
  uint8_t intervalHours;

  uint8_t sessionStartStyle;

  int16_t exposureCount;
} IntervalometerSaveData;

class IntervalInfo
{
  public: 
    enum SessionStartStyle
    {
      AT_TIME,
      AFTER_DELAY
    };

    // enum SessionEndStyle
    // {
    //   NO_END = 0,
    //   TOTAL_EXPOSURE_COUNT,
    //   TOTAL_DURATION,
    //   DATE_TIME
    // };

    IntervalInfo();

    unsigned long GetTotalStartDelayMillis() { return ((_data.startDelayHours * 3600) + (_data.startDelayMinutes * 60) + _data.startDelaySeconds) * 1000; };

    uint8_t GetStartDelaySeconds() { return _data.startDelaySeconds; };
    uint8_t GetStartDelayMinutes() { return _data.startDelayMinutes; };
    uint8_t GetStartDelayHours() { return _data.startDelayHours; };
    void SetStartDelay(uint8_t hours, uint8_t minutes, uint8_t seconds);

    uint8_t GetStartTimeHours() { return _data.startTimeHours; };
    uint8_t GetStartTimeMinutes() { return _data.startTimeMinutes; };
    void SetStartTime(uint8_t hours, uint8_t minutes);
    
    unsigned long  GetTotalShutterMillis() { return ((_data.shutterHours * 3600) + (_data.shutterMinutes * 60) + _data.shutterSeconds) * 1000; };

    uint8_t GetShutterSeconds() { return _data.shutterSeconds; };
    uint8_t GetShutterMinutes() { return _data.shutterMinutes; };
    uint8_t GetShutterHours() { return _data.shutterHours; };
    void SetShutter(uint8_t hours, uint8_t minutes, uint8_t seconds);
    
    unsigned long  GetTotalIntervalMillis() { return ((_data.intervalHours * 3600) + (_data.intervalMinutes * 60) + _data.intervalSeconds) * 1000; };

    uint8_t GetIntervalSeconds() { return _data.intervalSeconds; };
    uint8_t GetIntervalMinutes() { return _data.intervalMinutes; };
    uint8_t GetIntervalHours() { return _data.intervalHours; };
    void SetInterval(uint8_t hours, uint8_t minutes, uint8_t seconds);

    // unsigned long  GetTotalDurationMillis() { return ((_durationHours * 3600) + (_durationMinutes * 60) + _durationSeconds) * 1000; };

    // uint8_t GetDurationSeconds() { return _durationSeconds; };
    // uint8_t GetDurationMinutes() { return _durationMinutes; };
    // uint8_t GetDurationHours() { return _durationHours; };   
    // void SetDuration(uint8_t hours, uint8_t minutes, uint8_t seconds); 

    // float GetShutterTime() { return _shutterTime; };
    // float GetInterval() { return _exposureInterval; };

    SessionStartStyle GetSessionStartStyle() { return (SessionStartStyle)(_data.sessionStartStyle); };
    void SetSessionStartStyle(SessionStartStyle startStyle) { _data.sessionStartStyle = (int)startStyle; };
    
    // SessionEndStyle GetSessionEndStyle() { return _sessionEndStyle; };
    // void SetSessionEndStyle(SessionEndStyle endStyle) { _sessionEndStyle = endStyle; };

    
    uint16_t GetExposureCount() { return  _data.exposureCount; };
    void SetExposureCount(uint16_t count);
    
    //float GetTotalDuration() { return _sessionEndStyle == TOTAL_DURATION ? _totalDuration : 0; };
    //time_t GetEndDateTime() { return _endDateTime; };

    void GenerateStartDelayString(char* destination, bool showZero = false);
    void GenerateShutterString(char* destination);
    void GenerateIntervalString(char* destination);
    void GenerateDurationString(char* destination);

    void GenerateTimeString(char* destination, uint8_t hours, uint8_t minutes, uint8_t seconds, bool showZero = false);

    IntervalometerSaveData* GetSaveData() { return &_data; };
    void SetSaveData(IntervalometerSaveData data);

    bool GetIsSaveDataDirty() { return _isSaveDataDirty; }
    void SetIsSaveDataDirty(bool isDirty) { _isSaveDataDirty = isDirty; }

  private:

    IntervalometerSaveData _data;
    bool _isSaveDataDirty = false;
};

#endif //INTERVAL_INFO_H