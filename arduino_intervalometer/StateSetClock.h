#ifndef STATE_SET_CLOCK_H
#define STATE_SET_CLOCK_H

#include <RTCZero.h>
#include <ctime>
#include <Adafruit_GFX.h>
#include <EncoderButton.h>

#include "State.h"
#include "Selectable.h"

#define SAVE_MESSAGE F("SPIN TO SAVE")
#define SPIN_TO_SAVE_DECAY 0.05f
#define SPIN_TO_SAVE_SPEED 0.1f
#define SPIN_TO_SAVE_TIMEOUT 500



class StateSetClock : public State
{  
  enum SetClockComponent
  {
    HOURS = 0,
    MINUTES,
    DONE,
    MAX_STATE = DONE
  };

  public:
    StateSetClock();
    static StateSetClock* GetInstance() { return instance; }

    void Enter();
    void Update();
    void Exit();

    void HandleEncoder(EncoderButton& eb);
    void HandleClick(EncoderButton& eb);

    void ShowTimeFormatOption(bool show);

    uint8_t GetHours() { return (uint8_t)_hours; }
    uint8_t GetMinutes() { return (uint8_t)_minutes; }

    void SetTime(uint8_t hours, uint8_t minutes);
    void SetTitle(const char* title);

    void SetCompleteCallback(stateCompleteCallback callback);

    void SetCanCancel(bool canCancel);
    bool GetCanCancel();

  private:
    static StateSetClock* instance;

    Selectable* _hourSelectable;
    Selectable* _minuteSelectable;
    Selectable* _amPmSelectable;
    Selectable* _timeFormatSelectable;
    Selectable* _saveSelectable;
    Selectable* _cancelSelectable;

    const char* _titleText;

    char _clockHoursText[3];
    char _clockMinutesText[3];

    char _amPmText[3];

    int _hours = 0;
    int _minutes = 0;

    int _originalHours = 0;
    int _originalMinutes = 0;

    //bool _canCancel = true;

    char* GetClockHoursText();
    char* GetClockMinutesText();
    char* GetAmPmText();

    stateCompleteCallback _onComplete;
};

#endif