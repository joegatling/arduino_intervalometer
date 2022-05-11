#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <RTCZero.h>
#include <ctime>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EncoderButton.h>

#include "State.h"

class Controller 
{
  public:
    enum ProgramState
    {
      UNSET = -1,
      NONE = 0,
      SET_CLOCK,
      SETTINGS_MENU,
      RUNNING,
      STATE_COUNT
    };

    static Controller* GetInstance();    
    static void Initialize(ProgramState initialState);

    void Update();

    void SetState(ProgramState state);

    ProgramState GetState();
    Adafruit_SSD1306* GetDisplay();
    EncoderButton* GetKnob();
    RTCZero* GetRTC();

  private:
    Controller();

    static Controller* _instance;

    State* _states[STATE_COUNT];
    ProgramState _currentProgramState = NONE;
    ProgramState _nextProgramState = UNSET;

    Adafruit_SSD1306* _display;
    EncoderButton* _knob;
    RTCZero* _rtc;
};


#endif