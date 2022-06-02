#ifndef WAITING_TO_START_SUB_STATE_H
#define WAITING_TO_START_SUB_STATE_H

#include "SubState.h"

class StartSubState: public SubState
{
  public:
    StartSubState();

    virtual void Enter();
    virtual void Update();
    virtual void Exit();

};

#endif