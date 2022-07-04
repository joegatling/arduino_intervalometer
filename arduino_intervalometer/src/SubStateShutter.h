#ifndef SUB_STATE_SHUTTER_H
#define SUB_STATE_SHUTTER_H

#include "SubState.h"
#include "IntervalInfo.h"

class SubStateShutter: public SubState
{
  public:
    SubStateShutter();

    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    char* GetDisplayString(bool force);
    static void OnAlarm();

    void SetIsComplete(bool isComplete) { _isComplete = isComplete; }
    bool GetIsComplete() { return _isComplete; };


  private:
    static char _displayString[MAX_TIME_STRING_LENGTH];
    bool _isComplete = false;
    bool _isBlinking = false;

};

#endif