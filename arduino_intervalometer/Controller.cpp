#include "Controller.h"

#include "State.h"
#include "StateSetClock.h"
#include "StateRunning.h"
#include "StateIdle.h"
#include "StateSetTimeInterval.h"
#include "StateSetDuration.h"
#include "StateSetStartStyle.h"

#define SCREEN_ADDRESS 0x3C
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define ENCODER_A_PIN 5
#define ENCODER_B_PIN 6
#define ENCODER_BUTTON_PIN 10

#define SHUTTER_PIN 11
#define FOCUS_PIN 12

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
  

  // _display->setCursor(0,0);            
  // _display->setTextColor(SSD1306_WHITE); 
  // _display->setTextSize(1); 

  #ifdef ROTATE_DISPLAY        
    _display->setRotation(3); 
  #endif

  _display->clearDisplay();
  _display->drawBitmap(SCREEN_WIDTH / 2 - BOOT_LOGO_WIDTH/2, SCREEN_HEIGHT / 2 - BOOT_LOGO_HEIGHT / 2,  boot_logo, BOOT_LOGO_WIDTH, BOOT_LOGO_HEIGHT, SSD1306_WHITE);
  _display->display();

  // _display->println("LCD OK...");
  // _display->display();
  
  _knob = new EncoderButton(ENCODER_A_PIN, ENCODER_B_PIN, ENCODER_BUTTON_PIN);
  _knob->setEncoderHandler(HandleEncoder);
  _knob->setClickHandler(HandleClick);    

  // _display->println("Knob OK...");
  // _display->display();


  _rtc = new RTCZero();
  _rtc->begin();

  // _display->println("RTC OK...");
  // _display->display();

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


  // _display->println("States OK...");
  // _display->display();

  pinMode(SHUTTER_PIN, OUTPUT);
  digitalWrite(SHUTTER_PIN, HIGH);

  pinMode(FOCUS_PIN, INPUT_PULLUP);
  digitalWrite(FOCUS_PIN, HIGH);

  _nextProgramState = UNSET;
  _currentProgramState = NONE;
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

  Controller::GetInstance()->SetState(Controller::SET_TIME_INTERVAL);  

  GetInstance()->SetState(Controller::SET_CLOCK);
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

void Controller::SetShutter(bool isPressed)
{
  digitalWrite(SHUTTER_PIN, isPressed ? LOW : HIGH);
}

void Controller::SetFocus(bool isPressed)
{
  digitalWrite(FOCUS_PIN, isPressed ? LOW : HIGH);
}



void Controller::GenerateTimeString(char* destination, uint8_t hours, uint8_t minutes)
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
  GenerateTimeString(destination, GetRTC()->getHours(), GetRTC()->getMinutes());
}
