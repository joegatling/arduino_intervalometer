#ifndef SUB_STATE_INTERVAL_H
#define SUB_STATE_INTERVAL_H

#include "SubState.h"
#include "IntervalInfo.h"

class SubStateInterval: public SubState
{
  public:
    SubStateInterval();

    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    char* GetDisplayString(bool force);

  private:
    static char _displayString[MAX_TIME_STRING_LENGTH];
};

#endif