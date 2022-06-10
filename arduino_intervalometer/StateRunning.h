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
#include "SubState.h"
#include "Selectable.h"
#include "IntervalInfo.h"

class StateRunning : public State
{

  public:
    enum RunningSubState
    {
      UNSET = -1,
      NONE = 0,
      START,
      SHUTTER,
      INTERVAL,
      FINISHED,
      STATE_COUNT
    };

    StateRunning();

    void Enter();
    void Update(bool forceRedraw);
    void Exit();

    void HandleEncoder(EncoderButton& eb);
    void HandleClick(EncoderButton& eb);

    void SetSubState(RunningSubState state);
    SubState* GetCurrentSubState();
    SubState* GetSubState(RunningSubState state);

    void SetExposureCount(unsigned int count) {_exposureCount = count;};
    unsigned int GetExposureCount() { return _exposureCount; };
    unsigned int IncrementExposureCount() { return ++_exposureCount; };

    static StateRunning* GetInstance() { return __instance; };

  private:

    static StateRunning* __instance;
    static bool __refreshAllText;

    unsigned int _exposureCount = 0;

    SubState* _subStates[STATE_COUNT];
    Selectable* _selectablesForStates[STATE_COUNT];

    RunningSubState _currentSubState = NONE;
    RunningSubState _nextSubState = NONE;

    Selectable* _startSelectable;
    Selectable* _shutterSelectable;
    Selectable* _intervalSelectable;
    Selectable* _endSelectable;

    Selectable* _clockSelectable; 

    static char __clockString[8];
    static char __exposureRemainingString[8];

    bool _isUnlocking = false;
    unsigned long _lastUnlockMillis = 0;
    float _currentIconX = 0;
    float _targetIconX = 0;
    bool _redrawRequired = false;

    int _wobbleX = 0;
    int _wobbleY = 0;

    static char* GetDelayString();
    static char* GetShutterString();
    static char* GetIntervalString();
    static char* GetClockString();
    static char* GetExposuresRemainingString();

    const unsigned char* _currentBatteryIcon = NULL;

    float lerp(float a, float b, float x){  return a + x * (b - a); };

    void UpdateStates();
    void UpdateUnlock(Adafruit_GFX* display);
};

#endif