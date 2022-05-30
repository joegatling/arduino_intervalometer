#ifndef STATE_H
#define STATE_H

#include <Adafruit_GFX.h>
#include <EncoderButton.h>

#include "Selectable.h"


#define MAX_SELECTABLES 16

  typedef void (*stateCompleteCallback)(bool);


class State 
{
  public:
    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;

    virtual void HandleEncoder(EncoderButton &eb) = 0;
    virtual void HandleClick(EncoderButton &eb) = 0;

  protected:
    bool UpdateAllSelectables();
    void DrawAllSelectables(Adafruit_GFX* display);
    void AddSelectable(Selectable* s);

    Selectable* GetCurrentSelectable();
    void SetCurrentSelectable(Selectable* s, bool isSelected = false);
    Selectable* IncrementCurrentSelectable(int increment);
    

  private:
    Selectable* _allSelectables[MAX_SELECTABLES];
    unsigned int _selectableCount = 0;

    Selectable* _currentSelectable = NULL;
};


#endif