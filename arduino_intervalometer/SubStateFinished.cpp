#include "SubStateFinished.h"
#include "Controller.h"
#include "IntervalInfo.h"
#include "StateRunning.h"


SubStateFinished::SubStateFinished()
{}

void SubStateFinished::Enter()
{
  Controller::GetInstance()->SetState(Controller::IDLE);
}

void SubStateFinished::Update()
{}

void SubStateFinished::Exit()
{}
