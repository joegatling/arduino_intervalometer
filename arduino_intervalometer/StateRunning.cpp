#include "StateRunning.h"
#include "Controller.h"

#include "SubStateStart.h"
#include "SubStateShutter.h"
#include "SubStateInterval.h"
#include "SubStateFinished.h"
#include "StateMessage.h"

#define Y_OFFSET 32

#define MAX_LOCK_SLIDE_SPEED 6
#define UNLOCK_CANCEL_TIMEOUT 2000
#define UNLOCK_RETURN_TIMEOUT 500
#define UNLOCK_HINT_TIME 1000

StateRunning* StateRunning::__instance = 0;
bool StateRunning::__refreshAllText = false;

char StateRunning::__clockString[] = "\0";
char StateRunning::__exposureRemainingString[] = "\0";
char StateRunning::__startDoneString[] = "-\0";

StateRunning::StateRunning()
{
  for(int i = 0; i < StateRunning::STATE_COUNT; i++)
  {
    _subStates[i] = NULL;
    _selectablesForStates[i] = NULL;
  }

  _subStates[START] = new SubStateStart();
  _subStates[SHUTTER] = new SubStateShutter();
  _subStates[INTERVAL] = new SubStateInterval();
  _subStates[FINISHED] = new SubStateFinished();

  _currentSubState = StateRunning::NONE;
  _nextSubState = StateRunning::UNSET;

  _startSelectable = new Selectable(ICON_WIDTH + 1, Y_OFFSET, GetDelayString);
  _startSelectable->SetAlignment(Selectable::TOP_LEFT);

  _shutterSelectable = new Selectable(ICON_WIDTH + 1, Y_OFFSET + SELECTABLE_SPACING_1 * 1, GetShutterString);
  _shutterSelectable->SetAlignment(Selectable::TOP_LEFT);

  _intervalSelectable = new Selectable(ICON_WIDTH + 1, Y_OFFSET + SELECTABLE_SPACING_1 * 2, GetIntervalString);
  _intervalSelectable->SetAlignment(Selectable::TOP_LEFT);

  _endSelectable = new Selectable(ICON_WIDTH + 1, Y_OFFSET + SELECTABLE_SPACING_1 * 3, GetExposuresRemainingString);
  _endSelectable->SetAlignment(Selectable::TOP_LEFT);

  _clockSelectable = new Selectable(SCREEN_WIDTH-1, 1, GetClockString);
  _clockSelectable->SetAlignment(Selectable::TOP_RIGHT);
  //_clockSelectable->SetAlignment(Selectable::TOP_RIGHT);

  AddSelectable(_startSelectable);
  AddSelectable(_shutterSelectable);
  AddSelectable(_intervalSelectable);
  AddSelectable(_endSelectable);
  AddSelectable(_clockSelectable);

  _selectablesForStates[START] = _startSelectable;
  _selectablesForStates[SHUTTER] = _shutterSelectable;
  _selectablesForStates[INTERVAL] = _intervalSelectable;
  _selectablesForStates[FINISHED] = _endSelectable;

  //SetCurrentSelectable(_startSelectable);  

  __instance = this;
}

void StateRunning::Enter()
{
  auto config = Controller::GetInstance()->GetConfig();

  if(config->GetSessionStartStyle() == IntervalInfo::AFTER_DELAY && config->GetTotalStartDelayMillis() == 0)
  {
    SetSubState(StateRunning::SHUTTER);
  }
  else
  {
    SetSubState(StateRunning::START);
  }

  _currentIconX = 0;
  _targetIconX = _currentIconX;
  _isUnlocking = false;

  _exposureCount = 0;
  __refreshAllText = true;

  //digitalWrite(LED_RED_PIN, HIGH);
}

void StateRunning::Update(bool forceRedraw)
{
  _redrawRequired |= forceRedraw;

  const unsigned char* batteryIcon = Controller::GetInstance()->GetBatteryIconForCurrentVoltage();

  if(UpdateAllSelectables() || _redrawRequired || _isUnlocking || batteryIcon != _currentBatteryIcon)
  {
    _redrawRequired = false;

    Adafruit_SSD1306* display = Controller::GetInstance()->GetDisplay();

    display->clearDisplay();  
  
    display->drawBitmap(1, Y_OFFSET, icon_start, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(1, Y_OFFSET + SELECTABLE_SPACING_1, icon_shutter, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(1, Y_OFFSET + SELECTABLE_SPACING_1*2, icon_interval, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(1, Y_OFFSET + SELECTABLE_SPACING_1*3, icon_count, ICON_WIDTH, ICON_HEIGHT, SSD1306_WHITE);

    DrawAllSelectables(display);

    if(_selectablesForStates[_currentSubState] != NULL)
    {        
      auto s = _selectablesForStates[_currentSubState];
      display->fillRect(0, s->GetPositionY(), s->GetPositionX(), s->GetHeight(), SSD1306_INVERSE);
    }

    UpdateUnlock(display);

    _currentBatteryIcon = batteryIcon;
    display->drawBitmap(1,2,_currentBatteryIcon, BATTERY_ICON_WIDTH, BATTERY_ICON_HEIGHT, SSD1306_WHITE);

    display->fillRect(0, 0, SCREEN_WIDTH, SELECTABLE_SPACING_1, SSD1306_INVERSE);
    display->display();

    __refreshAllText = false;
  }

  UpdateStates();
}

void StateRunning::Exit()
{
  //digitalWrite(LED_RED_PIN, LOW);
}

void StateRunning::HandleEncoder(EncoderButton& eb)
{
  if(_isUnlocking)
  {
    if(eb.increment() > 0)
    {
      _lastUnlockMillis = millis();
      _targetIconX = min(max(_currentIconX, _targetIconX + eb.increment() * 3), SCREEN_WIDTH - UNLOCK_ICON_WIDTH);
    }
  }
  else
  {
    if(eb.increment() != 0)
    {
      _lastUnlockMillis = millis();

      _redrawRequired = true;
    }    
  }
}

void StateRunning::HandleClick(EncoderButton& eb)
{
  if(!_isUnlocking)
  {
    _lastUnlockMillis = millis();
    _startUnlockMillis = millis();
    _isUnlocking = true;
    _targetIconX = 0;
  }
}

float EaseInOutExpo(float t)
{
  if(0 < t && t < 1)
  {
    t = (t < 0.5f) ? pow(2, 20 * t - 10) / 2 : (2 - pow(2, -20 * t + 10)) / 2;
  }
  return t;
}

void StateRunning::UpdateUnlock(Adafruit_GFX* display)
{
  if(!_isUnlocking)
  {
    if(millis() - _lastUnlockMillis > 500)
    {
      _wobbleX = 0;
      _wobbleY = 0;
      _redrawRequired = true;
    }
    else if(millis() - _lastUnlockMillis < 250)
    {    
      _wobbleX = random(-2, 2);
      _wobbleY = random(-2, 2);
      _redrawRequired = true;
    }

    if(_wobbleX != 0 || _wobbleY != 0)
    {
      display->setCursor(UNLOCK_ICON_WIDTH+4 + _wobbleX,SCREEN_HEIGHT - 8 + _wobbleY);    
      display->print("CLICK!");
    }

    _targetIconX = 0;

    _currentIconX += constrain(_targetIconX - _currentIconX, -MAX_LOCK_SLIDE_SPEED, MAX_LOCK_SLIDE_SPEED);//  lerp(_iconX, targetX, (deltaTime / 1000.0f) * 10);
    
    display->drawBitmap(_currentIconX + _wobbleX, SCREEN_HEIGHT - UNLOCK_ICON_HEIGHT + _wobbleY, icon_locked, UNLOCK_ICON_WIDTH, UNLOCK_ICON_HEIGHT, SSD1306_WHITE);

  }
  else
  {
    _redrawRequired = true;

    if(millis() - _lastUnlockMillis > UNLOCK_CANCEL_TIMEOUT)
    {
      _isUnlocking = false;    
    }
    else if(millis() - _lastUnlockMillis > UNLOCK_RETURN_TIMEOUT)
    {
      _targetIconX = 0;
    }

    _currentIconX += constrain(_targetIconX - _currentIconX, -MAX_LOCK_SLIDE_SPEED, MAX_LOCK_SLIDE_SPEED);

    if(_currentIconX >= SCREEN_WIDTH - UNLOCK_ICON_WIDTH)
    {    
      auto messageState = StateMessage::GetInstance();
      messageState->SetMessage("Session Cancelled", 2000);
      messageState->SetCompleteCallback([](bool didCancel)
      {      
        Controller::GetInstance()->SetState(Controller::IDLE);                                             
      });

      Controller::GetInstance()->SetState(Controller::MESSAGE);
    }

    float t = (((millis() - _startUnlockMillis) % UNLOCK_HINT_TIME) / (float)UNLOCK_HINT_TIME);
    float x0 = EaseInOutExpo(max(0, t - 0.03f)) * (SCREEN_WIDTH - 4);
    float x1 = EaseInOutExpo(min(1, t + 0.03f)) * (SCREEN_WIDTH - 4);

    display->fillRect(2 + x0, SCREEN_HEIGHT - 7, x1 - x0, 3, SSD1306_WHITE );
    display->drawRect(2, SCREEN_HEIGHT - 9, SCREEN_WIDTH - 4, 7, SSD1306_WHITE);
    display->fillRect(3, SCREEN_HEIGHT - 8, _currentIconX - 3, 5, SSD1306_WHITE);
    display->fillRect(_currentIconX, SCREEN_HEIGHT - UNLOCK_ICON_HEIGHT, UNLOCK_ICON_WIDTH, UNLOCK_ICON_HEIGHT, SSD1306_BLACK);
    display->drawBitmap(_currentIconX, SCREEN_HEIGHT - UNLOCK_ICON_HEIGHT, icon_unlocking, UNLOCK_ICON_WIDTH, UNLOCK_ICON_HEIGHT, SSD1306_WHITE);
    display->drawRect(_currentIconX - 1, SCREEN_HEIGHT - UNLOCK_ICON_HEIGHT - 1, UNLOCK_ICON_WIDTH + 2, UNLOCK_ICON_HEIGHT + 2, SSD1306_BLACK);
  }
}

void StateRunning::UpdateStates()
{
 if(_nextSubState != StateRunning::UNSET)
  {
    if(_subStates[_currentSubState] != NULL)
    {
      _subStates[_currentSubState]->Exit();

      if(_selectablesForStates[_currentSubState] != NULL)
      {
        _selectablesForStates[_currentSubState]->SetState(Selectable::NONE);
      }
    }

    _currentSubState = _nextSubState;
    _nextSubState = UNSET;

    if(_subStates[_currentSubState] != NULL)
    {
      _subStates[_currentSubState]->SetEnterMillis(millis());
      _subStates[_currentSubState]->SetLastFrameMillis(millis());

      _subStates[_currentSubState]->Enter(); 

      if(_selectablesForStates[_currentSubState] != NULL)
      {        
        _selectablesForStates[_currentSubState]->SetState(_currentSubState == SHUTTER ? Selectable::SELECTED : Selectable::FOCUSED);        
      }

    }
  }

  if(_subStates[_currentSubState] != NULL)
  {
    _subStates[_currentSubState]->Update();
    _subStates[_currentSubState]->SetLastFrameMillis(millis());
  }  
}

char* StateRunning::GetDelayString()
{
  if(StateRunning::GetInstance()->GetCurrentSubStateType() == StateRunning::START)
  {
    SubStateStart* startState = (SubStateStart*)(StateRunning::GetInstance()->GetSubState(StateRunning::START));
    return startState->GetDisplayString(__refreshAllText);
  }
  else
  {
    return __startDoneString;
  }
}

char* StateRunning::GetShutterString()
{
  SubStateShutter* s = (SubStateShutter*)(StateRunning::GetInstance()->GetSubState(StateRunning::SHUTTER));
  return s->GetDisplayString(__refreshAllText);
}

char* StateRunning::GetIntervalString()
{
  SubStateInterval* s = (SubStateInterval*)(StateRunning::GetInstance()->GetSubState(StateRunning::INTERVAL));
  return s->GetDisplayString(__refreshAllText);
}

char* StateRunning::GetClockString()
{
  Controller::GetInstance()->GenerateTimeString(__clockString);
  return __clockString;
}

char* StateRunning::GetExposuresRemainingString()
{
  auto config = Controller::GetInstance()->GetConfig();

  if(config->GetExposureCount() > 0)
  {
    sprintf(__exposureRemainingString, "%d", config->GetExposureCount() - StateRunning::GetInstance()->GetExposureCount());
  }
  else
  {
    sprintf(__exposureRemainingString, "%d", StateRunning::GetInstance()->GetExposureCount());
  }

  return __exposureRemainingString;      
}

void StateRunning::SetSubState(SubStateType state)
{
  _nextSubState = state;
}

SubState* StateRunning::GetCurrentSubState()
{
  return _subStates[_currentSubState];
}

SubState* StateRunning::GetSubState(SubStateType state)
{
  return _subStates[state];
}
