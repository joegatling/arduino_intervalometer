#ifndef SELECTABLE_H
#define SELECTABLE_H

extern "C" {
typedef char* (*dynamicLabelCallback)(void);
}
#include <Adafruit_GFX.h>

#define MAX_LABEL_LENGTH 32

#define SELECTABLE_SPACING_1 13
#define SELECTABLE_SPACING_2 21
#define SELECTABLE_SPACING_3 29
// #define SELECTABLE_SPACING_1 11
// #define SELECTABLE_SPACING_2 19
// #define SELECTABLE_SPACING_3 27

#define FLASH_LENGTH 100

class Selectable
{
  public:
    enum State
    {
      NONE,
      FOCUSED,
      SELECTED
    };

    enum Alignment
    {
      TOP_LEFT,
      TOP_RIGHT,
      BOTTOM_LEFT,
      BOTTOM_RIGHT
    };

    Selectable(int x, int y, const char* staticLabel);
    Selectable(int x, int y, dynamicLabelCallback labelCallback);

    bool Update();
    void Draw(Adafruit_GFX* display);

    void SetTextScale(unsigned int);
    unsigned int GetTextScale();

    void SetPosition(int16_t x, int16_t y);
    int16_t GetPositionX();
    int16_t GetPositionY();

    uint16_t GetWidth();
    uint16_t GetHeight();

    char* GetStaticLabel();
    void SetStaticLabel(const char* staticLabel);
    
    Selectable* GetNextSelectable();
    void SetNextSelectable(Selectable* s);

    Selectable *GetPreviousSelectable();
    void SetPreviousSelectable(Selectable* s);

    void SetLinkedSelectables(Selectable* previous, Selectable* next);

    void SetDynamicLabelCallback(dynamicLabelCallback dynamicLabelCallback);

    State GetState();
    void SetState(State newState);

    bool GetRedrawRequired();
    void RequireRedraw() { _redrawRequired = true; } 

    Alignment GetAlignment();
    void SetAlignment(Alignment alignment);

    bool GetEnabled() { return _enabled; }
    void SetEnabled(bool enabled) { _enabled = enabled; _redrawRequired = true; }

    void Ping() { _flashStartTime = millis();}

    static void LinkInSequence(Selectable* selectable);
    static void EndSequence();
    static void LoopLinkSequence();


  private: 
    unsigned int _textScale;
    int16_t _x, _y;
    uint16_t _width, _height;

    bool _redrawRequired;

    bool _enabled;

    Alignment _alignment = TOP_LEFT;

    Selectable* _previousSelectable;
    Selectable* _nextSelectable;

    char _label[MAX_LABEL_LENGTH];
    dynamicLabelCallback _dynamicLabelCallback;

    State _state;

    static Selectable* __sequenceStart;
    static Selectable* __sequencePrevious;

    static char __tempLabel[MAX_LABEL_LENGTH];

    unsigned long _flashStartTime = 0;

};

#endif