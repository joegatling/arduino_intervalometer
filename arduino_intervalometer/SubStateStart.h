#ifndef WAITING_TO_START_SUB_STATE_H
#define WAITING_TO_START_SUB_STATE_H

#include "SubState.h"
#include "IntervalInfo.h"

class SubStateStart: public SubState
{
  public:
    SubStateStart();

    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    char* GetDisplayString(bool force);

    void SetIsComplete(bool isComplete) { _isComplete = isComplete; }
    bool GetIsComplete() { return _isComplete; };
    
    static void OnAlarm();

  private:
    static char _displayString[MAX_TIME_STRING_LENGTH];

    bool _isComplete = false;
};

#endif