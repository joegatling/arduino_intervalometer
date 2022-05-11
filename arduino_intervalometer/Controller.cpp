#include "Controller.h"

#include "StateSetClock.h"
#include "StateRunning.h"

#define SCREEN_ADDRESS 0x3C
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define ENCODER_A_PIN 5
#define ENCODER_B_PIN 6
#define ENCODER_BUTTON_PIN 10

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

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
  _display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  if(!_display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    while(true) {}; // Don't proceed, loop forever
  }
  
  _display->clearDisplay();
  _display->setCursor(0,0);            
  _display->setTextColor(SSD1306_WHITE); 
  _display->setTextSize(1);          

  _display->println("LCD OK...");
  _display->display();
  
  _knob = new EncoderButton(ENCODER_A_PIN, ENCODER_B_PIN, ENCODER_BUTTON_PIN);

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

Controller::ProgramState Controller::GetState()
{
  return _currentProgramState;
}

Adafruit_SSD1306* Controller::GetDisplay() { return _display; }

EncoderButton* Controller::GetKnob() { return _knob; }

RTCZero*  Controller::GetRTC() { return _rtc; }
