#ifndef STATE_SET_TIME_INTERVAL
#define STATE_SET_TIME_INTERVAL

#include <RTCZero.h>
#include <ctime>
#include <Adafruit_GFX.h>
#include <EncoderButton.h>

#include "State.h"
#include "Selectable.h"

class StateSetTimeInterval : public State
{  
  public:
    static StateSetTimeInterval* GetInstance() { return instance; }

    StateSetTimeInterval();

    void Enter();
    void Update(bool forceRedraw);
    void Exit();

    void HandleEncoder(EncoderButton& eb);
    void HandleClick(EncoderButton& eb);

    uint8_t GetHours() { return _hours; }
    uint8_t GetMinutes() { return _minutes; }
    uint8_t GetSeconds() { return _seconds; }

    void SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
    void SetTitle(const char* title);

    void SetCompleteCallback(stateCompleteCallback callback);

  private:
    static StateSetTimeInterval* instance;

    Selectable* _hourSelectable;
    Selectable* _minuteSelectable;
    Selectable* _secondsSelectable;

    Selectable* _saveSelectable;
    Selectable* _cancelSelectable;

    const char* _titleText;

    char _hoursText[3];
    char _minutesText[3];
    char _secondsText[3];

    uint8_t _hours = 0;
    uint8_t _minutes = 0;
    uint8_t _seconds = 0;

    uint8_t _originalHours;
    uint8_t _originalMinutes;
    uint8_t _originalSeconds;

    char* GetHoursText();
    char* GetMinutesText();
    char* GetSecondsText();

    bool _redrawRequired = false;

    stateCompleteCallback _onComplete;
};

#endif