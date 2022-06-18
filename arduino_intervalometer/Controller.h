#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <RTCZero.h>
#include <ctime>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EncoderButton.h>

#include "IntervalInfo.h"
#include "State.h"
#include "Icons.h"
#include "CircularBuffer.h"

#define SCREEN_WIDTH 64 // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels
#define ROTATE_DISPLAY

#define LED_RED_PIN 13

#define SLEEP_TIMEOUT 300000

class Controller 
{
  public:
    enum ProgramState
    {
      UNSET = -1,
      NONE = 0,
      IDLE,
      RUNNING,
      SET_START_STYLE,
      SET_CLOCK,
      SET_TIME_INTERVAL,
      SET_DURATION,
      MESSAGE,
      STATE_COUNT
    };

    static Controller* GetInstance();    
    static void Initialize();

    static void WakeInterrupt();

    void Update();

    void SetState(ProgramState state);
    void SetState(ProgramState state, unsigned long delay);
    State* GetCurrentState();

    ProgramState GetState();
    Adafruit_SSD1306* GetDisplay();
    EncoderButton* GetKnob();
    RTCZero* GetRTC();
    IntervalInfo* GetConfig();

    void SetShutter(bool isPressed);
    void SetFocus(bool isPressed);

    bool Get24TimeFormat() { return _use24HourFormat; };
    void Set24TimeFormat(bool use24HourFormat ) { _use24HourFormat = use24HourFormat; };

    void GenerateTimeString(char* destination, uint8_t hours, uint8_t minutes, uint8_t seconds = 0);
    void GenerateTimeString(char* destination);

    unsigned long GetMillisSinceLastInput() { return millis() - _lastInputTime; };
    void ResetLastInputMillis() { _lastInputTime = millis(); }; 

    unsigned long GetMillisSinceWakeUp() { return millis() - _wakeUpTime; };
    unsigned long GetMillisInCurrentState() { return millis() - _stateEnterTime; };

    unsigned long GetStateChangeDelay() { return millis() < _nextProgramStateTime ? 0 : _nextProgramStateTime - millis(); };

    bool GetIsAsleep() { return _isAsleep; };
    void WakeUp(bool isUserInput);

    void SetSleepDisabled(bool isSleepDisabled) { _sleepDisabled = isSleepDisabled; };
    bool GetSleepDisabled() { return _sleepDisabled; };

    void SetDisplayState(bool isOn);
    bool GetDisplayState() { return _isDisplayOn; };

    //void SetLedToSleepState() { digitalWrite(LED_RED_PIN, _isDisplayOn ? LOW : HIGH); };

    void SetOverrideLedState(bool isOn);
    void ClearOverrideLedState();

    float GetBatteryVoltage();
    const unsigned char* GetBatteryIconForCurrentVoltage();

  private:
    enum LedOverrideState
    {
      NO_OVERRIDE,
      OVERRIDE_ON,
      OVERRIDE_OFF
    };

    Controller();

    static void HandleEncoder(EncoderButton& eb);
    static void HandleClick(EncoderButton& eb); 

    static Controller* _instance;
    static volatile bool _discardInterrupts;

    void UpdateBatteryVoltage();

    void SleepInternal();

    IntervalInfo _config;

    State* _states[STATE_COUNT];
    ProgramState _currentProgramState = NONE;
    ProgramState _nextProgramState = UNSET;
    unsigned long _nextProgramStateTime = 0;

    Adafruit_SSD1306* _display;
    EncoderButton* _knob;
    RTCZero* _rtc;

    volatile unsigned long _lastInputTime = 0;
    volatile unsigned long _wakeUpTime = 0;
    volatile bool _isAsleep = false;
    volatile bool _isDisplayOn = true;
    bool _sleepDisabled = false;

    unsigned long _stateEnterTime = 0;
    
    bool _redrawRequired = false;
    bool _use24HourFormat = true;


    LedOverrideState _ledOverride = NO_OVERRIDE;

    void UpdateLedState();

    unsigned long _lastBatteryReadTime = 0;
    CircularBuffer _batteryVoltage = CircularBuffer(32);
};



#endif