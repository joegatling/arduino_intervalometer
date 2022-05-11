#ifndef STATE_H
#define STATE_H

class State 
{
  public:
    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;
};


#endif