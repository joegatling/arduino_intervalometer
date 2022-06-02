#ifndef RUNNING_SUB_STATE_H
#define RUNNING_SUB_STATE_H

#include <Arduino.h>

class SubState 
{
  public:
    virtual void Enter() = 0;
    virtual void Update() = 0;
    virtual void Exit() = 0;

    void SetEnterMillis(unsigned long enterMillis) { _enterMillis = enterMillis; }
    void SetLastFrameMillis(unsigned long lastFrameMillis) { _lastFrameMillis = lastFrameMillis; }
    
    unsigned long GetTimeInState() { return millis() - _enterMillis;}
    unsigned long GetTimeSinceLastFrame() { return millis() - _lastFrameMillis;}

  protected:    
    unsigned long _enterMillis = 0;
    unsigned long _lastFrameMillis = 0;
};


#endif