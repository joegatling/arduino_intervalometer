#include "Controller.h"

#include "State.h"
#include "StateSetClock.h"
#include "StateRunning.h"
#include "StateIdle.h"
#include "StateSetTimeInterval.h"
#include "StateSetDuration.h"
#include "StateSetStartStyle.h"
#include "StateMessage.h"
//#include <ArduinoLowPower.h>

#define SCREEN_ADDRESS 0x3C
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define ENCODER_A_PIN 6
#define ENCODER_B_PIN 5
#define ENCODER_BUTTON_PIN 10

#define SHUTTER_PIN 11
#define FOCUS_PIN 12

#define WAKE_UP_INPUT_DELAY 200

volatile bool Controller::_discardInterrupts = false;

Controller* Controller::_instance = NULL;
Controller* Controller::GetInstance()
{
  if(_instance == NULL)
  {
    _instance = new Controller();
  }

  return _instance;
}

Controller::Controller()
{
  #ifdef ROTATE_DISPLAY
    _display = new Adafruit_SSD1306(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire, OLED_RESET);
  #else
    _display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  #endif
  
  if(!_display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    while(true) {}; // Don't proceed, loop forever
  }

  GetDisplay()->ssd1306_command(SSD1306_DISPLAYON);    
  

  #ifdef ROTATE_DISPLAY         
    _display->setRotation(3); 
  #endif

  _display->clearDisplay();
  _display->drawBitmap(SCREEN_WIDTH / 2 - BOOT_LOGO_WIDTH/2, SCREEN_HEIGHT / 2 - BOOT_LOGO_HEIGHT / 2,  boot_logo, BOOT_LOGO_WIDTH, BOOT_LOGO_HEIGHT, SSD1306_WHITE);
  _display->display();

  
  _knob = new EncoderButton(ENCODER_A_PIN, ENCODER_B_PIN, ENCODER_BUTTON_PIN);
  _knob->setEncoderHandler(HandleEncoder);
  _knob->setClickHandler(HandleClick);    

  _rtc = new RTCZero();
  _rtc->begin();

  // Initialise _states
  for(int i = 0; i < Controller::STATE_COUNT; i++)
  {
    _states[i] = NULL;
  }

  _states[Controller::SET_CLOCK] = new StateSetClock();
  _states[Controller::RUNNING] = new StateRunning();
  _states[Controller::IDLE] = new StateIdle();
  _states[Controller::SET_TIME_INTERVAL] = new StateSetTimeInterval();
  _states[Controller::SET_DURATION] = new StateSetDuration();
  _states[Controller::SET_START_STYLE] = new StateSetStartStyle();
  _states[Controller::MESSAGE] = new StateMessage();


  // _display->println("States OK...");
  // _display->display();

  pinMode(SHUTTER_PIN, OUTPUT);
  digitalWrite(SHUTTER_PIN, HIGH);

  pinMode(FOCUS_PIN, INPUT_PULLUP);
  digitalWrite(FOCUS_PIN, HIGH);

  pinMode(LED_RED_PIN, OUTPUT);

  _nextProgramState = UNSET;
  _currentProgramState = NONE;


  _isDisplayOn = true;
  _isAsleep = false;
  _redrawRequired = false;

  UpdateLedState();
}

void Controller::Initialize()
{
  GetInstance();

  StateSetClock::GetInstance()->SetCompleteCallback([](bool didCancel)
  {
    Controller::GetInstance()->GetRTC()->setHours(StateSetClock::GetInstance()->GetHours());
    Controller::GetInstance()->GetRTC()->setMinutes(StateSetClock::GetInstance()->GetMinutes());

    Controller::GetInstance()->SetState(Controller::IDLE);                                             
  });

  StateSetClock::GetInstance()->SetTitle("CLOCK");
  StateSetClock::GetInstance()->SetTime(Controller::GetInstance()->GetRTC()->getHours(), 
                                        Controller::GetInstance()->GetRTC()->getMinutes());
  StateSetClock::GetInstance()->SetCanCancel(false);

  Controller::GetInstance()->SetState(Controller::SET_CLOCK);  

  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
}

void Controller::Update()
{
  if(_knob != NULL)
  {    
    _knob->update();
  }

  if(_nextProgramState != Controller::UNSET)
  {
    if(_states[_currentProgramState] != NULL)
    {
      _states[_currentProgramState]->Exit();
    }

    _currentProgramState = _nextProgramState;
    _nextProgramState = UNSET;
    _stateEnterTime = millis();

    if(_states[_currentProgramState] != NULL)
    {
      _states[_currentProgramState]->Enter(); 
    }
  }

  if(_states[_currentProgramState] != NULL)
  {
    _states[_currentProgramState]->Update(_redrawRequired);
  }

  _redrawRequired = false; 

  if(min(GetMillisSinceWakeUp(), GetMillisSinceLastInput()) > SLEEP_TIMEOUT)
  {
    Sleep();
  }

  // This will be the first line to execute after sleep
  SetDisplayState(GetMillisSinceLastInput() < SLEEP_TIMEOUT);    
}

void Controller::SetState(Controller::ProgramState state)
{
  _nextProgramState = state;
}

State* Controller::GetCurrentState()
{
  return _states[_currentProgramState];
}

void Controller::HandleEncoder(EncoderButton& eb)
{
  State* currentState = Controller::GetInstance()->GetCurrentState();

  if(eb.increment() != 0)
  {
    Controller::GetInstance()->ResetLastInputMillis();
  }

  if(Controller::GetInstance()->GetMillisSinceWakeUp() > WAKE_UP_INPUT_DELAY)
  {  
    if(currentState != NULL)
    {
      currentState->HandleEncoder(eb);
    }
  }
}

void Controller::HandleClick(EncoderButton& eb)
{
  Controller::GetInstance()->ResetLastInputMillis();

  if(Controller::GetInstance()->GetMillisSinceWakeUp() > WAKE_UP_INPUT_DELAY)
  {
    State* currentState = Controller::GetInstance()->GetCurrentState();

    if(currentState != NULL)
    {
      currentState->HandleClick(eb);
    }
  }
}

Adafruit_SSD1306* Controller::GetDisplay() { return _display; }

EncoderButton* Controller::GetKnob() { return _knob; }

RTCZero*  Controller::GetRTC() { return _rtc; }

IntervalInfo*  Controller::GetConfig() { return &_config; }

void Controller::SetShutter(bool isPressed)
{
  digitalWrite(SHUTTER_PIN, isPressed ? LOW : HIGH);
}

void Controller::SetFocus(bool isPressed)
{
  digitalWrite(FOCUS_PIN, isPressed ? LOW : HIGH);
}



void Controller::GenerateTimeString(char* destination, uint8_t hours, uint8_t minutes, uint8_t seconds)
{

  if(!_use24HourFormat)
  {
    int hoursToShow = hours;
    if(hours == 0 || hours == 12)
    {
      hoursToShow = 12;
    }
    else
    {
      hoursToShow = hours % 12;
    }

    sprintf(destination, "%d:%02d%s", hoursToShow, minutes, hours < 12 ? "am" : "pm");
  }
  else
  {
    sprintf(destination, "%02d:%02d", hours, minutes);
  }   
}

void Controller::GenerateTimeString(char* destination)
{
  GenerateTimeString(destination, GetRTC()->getHours(), GetRTC()->getMinutes(), GetRTC()->getSeconds());
}

void Controller::Sleep()
{
  if(!_isAsleep)
  {
    _isAsleep = true;

    SetDisplayState(false);

    // This is a bit of a hack.
    // For some reason, the interrupts below fire immediately if that pin state ever changed.
    // This bool causes that extra interrupt to be disarded
    _discardInterrupts = true;

    // A1 attaches to an encoder, so it could be high or low
    attachInterrupt(digitalPinToInterrupt(A1), WakeInterrupt, digitalRead(A1) == HIGH ? LOW : HIGH);
    attachInterrupt(digitalPinToInterrupt(A2), WakeInterrupt, LOW);
    
    _discardInterrupts = false;

    UpdateLedState();

    //LowPower.deepSleep();
    GetRTC()->standbyMode();
  }
}

void Controller::WakeUp(bool isUserInput)
{  
  if(_isAsleep)
  {
    detachInterrupt(digitalPinToInterrupt(A1));
    detachInterrupt(digitalPinToInterrupt(A2));

    _wakeUpTime = millis();
      
    _isAsleep = false;
  }

  if(isUserInput)
  {
    ResetLastInputMillis();
  }  
}

void Controller::SetDisplayState(bool isOn)
{  
  if(_isDisplayOn != isOn)
  {
    if(isOn)
    {
      GetDisplay()->ssd1306_command(SSD1306_DISPLAYON);    
      _redrawRequired = true;
    }
    else
    {
      GetDisplay()->ssd1306_command(SSD1306_DISPLAYOFF);    
    }
    _isDisplayOn = isOn;

    UpdateLedState();
  }
}

void Controller::WakeInterrupt()
{
  if(!_discardInterrupts)
  {
    Controller::GetInstance()->WakeUp(true);
  }
}

void Controller::SetOverrideLedState(bool isOn)
{
  _ledOverride = isOn ? OVERRIDE_ON : OVERRIDE_OFF;
  UpdateLedState();
}

void Controller::ClearOverrideLedState()
{
  _ledOverride = NO_OVERRIDE;
  UpdateLedState();
}

void Controller::UpdateLedState()
{
  if(_ledOverride == OVERRIDE_ON)
  {
    digitalWrite(LED_RED_PIN, HIGH);    
  }
  else if(_ledOverride == OVERRIDE_OFF)
  {
    digitalWrite(LED_RED_PIN, LOW);    
  }
  else
  {
    digitalWrite(LED_RED_PIN, _isDisplayOn ? LOW : HIGH);
  }

}

