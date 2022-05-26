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

// // 'end', 9x11px
// const unsigned char icon_end [] PROGMEM = {
// 	0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x08, 0x80, 0x04, 0x80, 0xfe, 0x80, 0x04, 0x80, 0x08, 0x80, 
// 	0x01, 0x80, 0x00, 0x00, 0x00, 0x00
// };
// // 'start', 9x11px
// const unsigned char icon_start [] PROGMEM = {
// 	0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x82, 0x00, 0x81, 0x00, 0xbf, 0x80, 0x81, 0x00, 0x82, 0x00, 
// 	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00
// };
// // 'shutter', 9x11px
// const unsigned char icon_shutter [] PROGMEM = {
// 	0x08, 0x00, 0x08, 0x00, 0x2a, 0x00, 0x1c, 0x00, 0x08, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x22, 0x00, 
// 	0x7f, 0x00, 0x00, 0x00, 0x00, 0x00
// };
// // 'interval', 9x11px
// const unsigned char icon_interval [] PROGMEM = {
// 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0xaa, 0x80, 0x80, 0x80, 0x80, 0x80, 
// 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
// };

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
      SET_CLOCK,
      SET_TIME_INTERVAL,
      SET_DURATION,
      STATE_COUNT
    };

    static Controller* GetInstance();    
    static void Initialize(ProgramState initialState);

    void Update();

    void SetState(ProgramState state);
    State* GetCurrentState();

    ProgramState GetState();
    Adafruit_SSD1306* GetDisplay();
    EncoderButton* GetKnob();
    RTCZero* GetRTC();
    IntervalInfo* GetConfig();


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
};


#endif