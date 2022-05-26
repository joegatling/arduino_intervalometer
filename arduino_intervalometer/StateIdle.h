#ifndef STATE_IDLE_H
#define STATE_IDLE_H

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

class StateIdle : public State
{
  public:
    StateIdle();

    void Enter();
    void Update();
    void Exit();

    void HandleEncoder(EncoderButton& eb);
    void HandleClick(EncoderButton& eb);

  private:
    static char __clockString[8];

    static char __delayString[MAX_TIME_STRING_LENGTH];
    static char __shutterString[MAX_TIME_STRING_LENGTH];
    static char __intervalString[MAX_TIME_STRING_LENGTH];
    static char __durationString[MAX_TIME_STRING_LENGTH];

    Selectable* _beginSelectable;
    Selectable* _setClockSelectable;
    Selectable* _intervalSettingsSelectable;
    Selectable* _startSettingsSelectable;
    Selectable* _endSettingsSelectable;
    Selectable* _shutterSettingsSelectable;

    static char* GetDelayString();
    static char* GetShutterString();
    static char* GetIntervalString();
    static char* GetDurationString();

    static char* GetClockString();

    bool _redrawRequired;

};

#endif