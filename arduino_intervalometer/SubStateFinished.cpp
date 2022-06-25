#include "SubStateFinished.h"
#include "Controller.h"
#include "IntervalInfo.h"
#include "StateRunning.h"
#include "StateMessage.h"


SubStateFinished::SubStateFinished()
{}

void SubStateFinished::Enter()
{
  Controller::GetInstance()->WakeUp(true);
}

void SubStateFinished::Update()
{
  if(Controller::GetInstance()->GetMillisInCurrentState() > 1000)
  {
    auto messageState = StateMessage::GetInstance();

    if(StateRunning::GetInstance()->DidAbandonSession())
    {
      messageState->SetMessage("Session Cancelled", 2000);
    }
    else
    {
      messageState->SetMessage("Session Complete", 2000);
    }

    messageState->SetCompleteCallback([](bool didCancel)
    {      
      Controller::GetInstance()->SetState(Controller::IDLE);                                             
    });

    Controller::GetInstance()->SetState(Controller::MESSAGE);
  }
}

void SubStateFinished::Exit()
{}
