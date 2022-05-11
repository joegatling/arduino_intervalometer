#ifndef STATE_SET_CLOCK_H
#define STATE_SET_CLOCK_H

#include <RTCZero.h>
#include <ctime>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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

    void Enter();
    void Update();
    void Exit();

    void HandleEncoder(EncoderButton& eb);
    void HandleClick(EncoderButton& eb);

  private:
    static StateSetClock* instance;
    
    bool _redrawRequired = false;

    Selectable* _currentSelectable;

    Selectable* _hourSelectable;
    Selectable* _minuteSelectable;
    Selectable* _saveSelectable;
    Selectable* _cancelSelectable;

    char _clockHoursText[3];
    char _clockMinutesText[3];

    uint8_t _hours = 0;
    uint8_t _minutes = 0;

    char* GetClockHoursText();
    char* GetClockMinutesText();
};

#endif