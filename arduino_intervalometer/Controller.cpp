#include "Controller.h"

#include "StateSetClock.h"
#include "StateRunning.h"
#include "StateIdle.h"
#include "StateSetTimeInterval.h"
#include "StateSetDuration.h"

#define SCREEN_ADDRESS 0x3C
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define ENCODER_A_PIN 5
#define ENCODER_B_PIN 6
#define ENCODER_BUTTON_PIN 10

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

  // _display->ssd1306_command(SSD1306_SETCONTRAST);
  // _display->ssd1306_command(32);
  
  
  if(!_display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    while(true) {}; // Don't proceed, loop forever
  }
  
  _display->clearDisplay();
  _display->setCursor(0,0);            
  _display->setTextColor(SSD1306_WHITE); 
  _display->setTextSize(1); 

  #ifdef ROTATE_DISPLAY        
    _display->setRotation(1); 
  #endif

  _display->println("LCD OK...");
  _display->display();
  
  _knob = new EncoderButton(ENCODER_A_PIN, ENCODER_B_PIN, ENCODER_BUTTON_PIN);
  _knob->setEncoderHandler(HandleEncoder);
  _knob->setClickHandler(HandleClick);    

  _display->println("Knob OK...");
  _display->display();


  _rtc = new RTCZero();
  _rtc->begin();

  _display->println("RTC OK...");
  _display->display();

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


  _display->println("States OK...");
  _display->display();

  _nextProgramState = UNSET;
  _currentProgramState = NONE;
}

void Controller::Initialize(Controller::ProgramState initialState)
{
  GetInstance()->SetState(initialState);
}

void Controller::Update()
{
  _knob->update();

  if(_nextProgramState != Controller::UNSET)
  {
    if(_states[_currentProgramState] != NULL)
    {
      _states[_currentProgramState]->Exit();
    }

    _currentProgramState = _nextProgramState;
    _nextProgramState = UNSET;

    if(_states[_currentProgramState] != NULL)
    {
      _states[_currentProgramState]->Enter(); 
    }
  }

  if(_states[_currentProgramState] != NULL)
  {
    _states[_currentProgramState]->Update();
  }  
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

  if(currentState != NULL)
  {
    currentState->HandleEncoder(eb);
  }
}

void Controller::HandleClick(EncoderButton& eb)
{
  State* currentState = Controller::GetInstance()->GetCurrentState();

  if(currentState != NULL)
  {
    currentState->HandleClick(eb);
  }
}

Adafruit_SSD1306* Controller::GetDisplay() { return _display; }

EncoderButton* Controller::GetKnob() { return _knob; }

RTCZero*  Controller::GetRTC() { return _rtc; }

IntervalInfo*  Controller::GetConfig() { return &_config; }
