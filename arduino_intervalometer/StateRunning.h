#ifndef STATE_RUNNING_H
#define STATE_RUNNING_H

#include <RTCZero.h>
#include <ctime>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EncoderButton.h>

#include "State.h"
#include "Selectable.h"

class StateRunning : public State
{
  public:
    StateRunning();

    void Enter();
    void Update();
    void Exit();

    void HandleEncoder(EncoderButton& eb);
    void HandleClick(EncoderButton& eb);

  private:
    char _clockString[8];
};

#endif