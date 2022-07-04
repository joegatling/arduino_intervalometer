#ifndef STATE_MESSAGE_H
#define STATE_MESSAGE_H

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

class StateMessage : public State
{
  public:
    StateMessage();

    void Enter();
    void Update(bool forceRedraw);
    void Exit();

    void HandleEncoder(EncoderButton& eb);
    void HandleClick(EncoderButton& eb);

    static StateMessage* GetInstance() { return instance; }
    
    void SetMessage(const char* message, unsigned long duration = 0);


    void SetCompleteCallback(stateCompleteCallback callback);

  private:
    static StateMessage* instance;

    char _messageText[64];
    unsigned long _messageDuration = 0;

    stateCompleteCallback _onComplete;

};

#endif