#include <RTCZero.h>
#include <ctime>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EncoderButton.h>
#include <stdio.h>
#include <string.h>

#include "Selectable.h"
#include "Controller.h"


Selectable::Selectable(int x, int y, const char* staticLabel) 
  :_x(x), _y(y), _previousSelectable(NULL), _nextSelectable(NULL), _dynamicLabelCallback(0), _textScale(1), _state(Selectable::NONE)
{
  strcpy(_staticLabel, staticLabel);
}

Selectable::Selectable(int x, int y, dynamicLabelCallback labelCallback)
  :_x(x), _y(y), _dynamicLabelCallback(labelCallback), _previousSelectable(NULL), _nextSelectable(NULL), _textScale(1), _state(Selectable::NONE)
{
  strcpy(_staticLabel, "");
}

void Selectable::Update(Adafruit_GFX* display)
{
  if(_dynamicLabelCallback != 0)
  {
    strcpy(_staticLabel, _dynamicLabelCallback());
  }

  int16_t  x1, y1;

  display->setTextColor(SSD1306_WHITE);  
  display->setTextSize(_textScale);
  display->getTextBounds(_staticLabel, 0, 0, &x1, &y1, &_width, &_height);

  _width += 4 - _textScale;
  _height += 4 - _textScale;

  display->setCursor(_x+2,_y+2);  
  display->print(_staticLabel);

  if(_state == Selectable::FOCUSED)
  {
    display->drawRect(_x,_y,_width,_height,SSD1306_WHITE);
  }
  else if(_state == Selectable::SELECTED)
  {
    display->fillRect(_x,_y,_width,_height,SSD1306_INVERSE);
  }
}

void Selectable::SetTextScale(unsigned int scale) { _textScale = scale; }
unsigned int Selectable::GetTextScale(){ return _textScale; }

void Selectable::SetPosition(int16_t x, int16_t y){ _x = x; _y = y; }
int16_t Selectable::GetPositionX(){ return _x; }
int16_t Selectable::GetPositionY(){ return _y; }

uint16_t Selectable::GetWidth(){ return _width; }
uint16_t Selectable::GetHeight(){ return _height; }

char* Selectable::GetStaticLabel(){ return _staticLabel; }
void Selectable::SetStaticLabel(const char* staticLabel)
{
  strcpy(_staticLabel, staticLabel);
  _dynamicLabelCallback = 0;
}

Selectable* Selectable::GetNextSelectable() { return _nextSelectable; }
void Selectable::SetNextSelectable(Selectable* s){ _nextSelectable = s; }

Selectable* Selectable::GetPreviousSelectable(){ return _previousSelectable; }
void Selectable::SetPreviousSelectable(Selectable* s){ _previousSelectable = s; }

void Selectable::SetLinkedSelectables(Selectable* previous, Selectable* next)
{
  SetNextSelectable(next);
  SetPreviousSelectable(previous);
}

Selectable::State Selectable::GetState() { return _state; }
void Selectable::SetState(Selectable::State newState) { _state = newState; }

void Selectable::SetDynamicLabelCallback(dynamicLabelCallback dynamicLabelCallback) { _dynamicLabelCallback = dynamicLabelCallback; }