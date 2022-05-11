#ifndef SELECTABLE_H
#define SELECTABLE_H

extern "C" {
typedef char* (*dynamicLabelCallback)(void);
}
#include <Adafruit_GFX.h>

#define MAX_LABEL_LENGTH 32

class Selectable
{
  public:
    enum State
    {
      NONE,
      FOCUSED,
      SELECTED
    };

    Selectable(int x, int y, const char* staticLabel);
    Selectable(int x, int y, dynamicLabelCallback labelCallback);

    void Update(Adafruit_GFX* display);

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

  private: 
    unsigned int _textScale;
    int16_t _x, _y;
    uint16_t _width, _height;

    Selectable* _previousSelectable;
    Selectable* _nextSelectable;

    char _staticLabel[MAX_LABEL_LENGTH];
    dynamicLabelCallback _dynamicLabelCallback;

    State _state;


};

#endif