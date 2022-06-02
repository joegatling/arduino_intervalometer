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
#include "IntervalInfo.h"

#define SCREEN_WIDTH 64 // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels
#define ROTATE_DISPLAY

#define ICON_WIDTH 7
#define ICON_HEIGHT 11

#define UNLOCK_ICON_WIDTH 11
#define UNLOCK_ICON_HEIGHT 11

// 'end', 7x11px
const unsigned char icon_end [] PROGMEM = {
	0x00, 0x00, 0x00, 0x22, 0x12, 0xfa, 0x12, 0x22, 0x00, 0x00, 0x00
};
// 'interval', 7x11px
const unsigned char icon_interval [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x82, 0xfe, 0x82, 0x00, 0x00, 0x00, 0x00
};
// 'shutter', 7x11px
const unsigned char icon_shutter [] PROGMEM = {
	0x10, 0x10, 0x54, 0x38, 0x10, 0x00, 0x7c, 0x44, 0xfe, 0x00, 0x00
};
// 'start', 7x11px
const unsigned char icon_start [] PROGMEM = {
	0x00, 0x00, 0x00, 0x88, 0x84, 0xbe, 0x84, 0x88, 0x00, 0x00, 0x00
};

// 'false', 7x11px
const unsigned char icon_true [] PROGMEM = {
	0x00, 0x00, 0xfe, 0x82, 0xba, 0xba, 0xba, 0x82, 0xfe, 0x00, 0x00
};
// 'true', 7x11px
const unsigned char icon_false [] PROGMEM = {
	0x00, 0x00, 0xfe, 0x82, 0x82, 0x82, 0x82, 0x82, 0xfe, 0x00, 0x00
};

const unsigned char icon_count [] PROGMEM = {
  0x00, 0x00, 0x28, 0x28, 0x7c, 0x28, 0x7c, 0x28, 0x28, 0x00, 0x00
};

// 'sprite_0', 11x11px
const unsigned char icon_unlocking [] PROGMEM = {
	0x7f, 0xc0, 0xf1, 0xe0, 0xee, 0xe0, 0xee, 0xe0, 0xee, 0xe0, 0xc0, 0x60, 0xc0, 0x60, 0xc0, 0x60, 
	0xc0, 0x60, 0xc0, 0x60, 0x7f, 0xc0
};
// 'sprite_1', 11x11px
const unsigned char icon_locked [] PROGMEM = {
	0x00, 0x00, 0x0e, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x3f, 0x80, 0x3f, 0x80, 0x3f, 0x80, 
	0x3f, 0x80, 0x3f, 0x80, 0x00, 0x00
};

// // Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 192)
// const int icon_allArray_LEN = 4;
// const unsigned char* icon_allArray[4] = {
// 	icon_end,
// 	icon_interval,
// 	icon_shutter,
// 	icon_start
// };

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
      STATE_COUNT
    };

    static Controller* GetInstance();    
    static void Initialize();

    void Update();

    void SetState(ProgramState state);
    State* GetCurrentState();

    ProgramState GetState();
    Adafruit_SSD1306* GetDisplay();
    EncoderButton* GetKnob();
    RTCZero* GetRTC();
    IntervalInfo* GetConfig();

    bool Get24TimeFormat() { return _use24HourFormat; };
    void Set24TimeFormat(bool use24HourFormat ) { _use24HourFormat = use24HourFormat; };

    void GenerateTimeString(char* destination, uint8_t hours, uint8_t minutes);
    void GenerateTimeString(char* destination);

  private:
    Controller();

    static void HandleEncoder(EncoderButton& eb);
    static void HandleClick(EncoderButton& eb); 

    static Controller* _instance;

    IntervalInfo _config;

    State* _states[STATE_COUNT];
    ProgramState _currentProgramState = NONE;
    ProgramState _nextProgramState = UNSET;

    Adafruit_SSD1306* _display;
    EncoderButton* _knob;
    RTCZero* _rtc;

    bool _use24HourFormat = true;
};


#endif