#ifndef STATE_SET_START_STYLE_H
#define STATE_SET_START_STYLE_H

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

class StateSetStartStyle : public State
{
  public:
    StateSetStartStyle();

    void Enter();
    void Update();
    void Exit();

    void HandleEncoder(EncoderButton& eb);
    void HandleClick(EncoderButton& eb);

  private:
    Selectable* _immediateSelectable;
    Selectable* _timeSelectable;
    Selectable* _delaySelectable;

    Selectable* _cancelSelectable;
    
    bool _redrawRequired;
};

#endif