#ifndef STATE_SET_DURATION_H
#define STATE_SET_DURATION_H

#include <RTCZero.h>
#include <ctime>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EncoderButton.h>

#include "State.h"
#include "Selectable.h"
#include "IntervalInfo.h"

class StateSetDuration : public State
{
  public:
    StateSetDuration();

    void Enter();
    void Update();
    void Exit();

    void HandleEncoder(EncoderButton& eb);
    void HandleClick(EncoderButton& eb);

  private:
    Selectable* _exposureCountSelectable;
    Selectable* _totalDurationSelectable;
    Selectable* _endTimeSelectable;
    Selectable* _noEndSelectable;

    Selectable* _currentValueSelectable;

    Selectable* _saveSelectable;
    Selectable* _cancelSelectable;

    bool _redrawRequired;

    void RearrangeSelectables();

    static char __durationString[MAX_TIME_STRING_LENGTH];
    static char* GetDurationString();


};

#endif