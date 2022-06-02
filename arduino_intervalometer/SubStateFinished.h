#ifndef FINISHED_SUB_STATE_H
#define FINISHED_SUB_STATE_H

#include "SubState.h"

class SubStateFinished: public SubState
{
  public:
    SubStateFinished();

    virtual void Enter();
    virtual void Update();
    virtual void Exit();

  private:
};

#endif