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


Selectable* Selectable::__sequenceStart = 0;
Selectable* Selectable::__sequencePrevious = 0;

char Selectable::__tempLabel[] = "\0";

void Selectable::LinkInSequence(Selectable* selectable)
{
  if(__sequencePrevious == NULL)
  {
    __sequenceStart = selectable;
    __sequencePrevious = selectable;

    __sequenceStart->SetPreviousSelectable(NULL);
    __sequenceStart->SetNextSelectable(NULL);
  }
  else
  {
    __sequencePrevious->SetNextSelectable(selectable);
    selectable->SetLinkedSelectables(__sequencePrevious, NULL);
    __sequencePrevious = selectable;
  }
}

void Selectable::EndSequence()
{
  __sequencePrevious = NULL;
  __sequenceStart = NULL;  
}

void Selectable::LoopLinkSequence()
{
  if(__sequenceStart != NULL && __sequencePrevious != NULL)
  {
    __sequencePrevious->SetNextSelectable(__sequenceStart);
    __sequenceStart->SetPreviousSelectable(__sequencePrevious);
  }

  __sequencePrevious = NULL;
  __sequenceStart = NULL;  
}


Selectable::Selectable(int x, int y, const char* staticLabel) 
  :_x(x), _y(y), _previousSelectable(NULL), _nextSelectable(NULL), _dynamicLabelCallback(0), 
  _textScale(1), _state(Selectable::NONE), _redrawRequired(true), _enabled(true)
{
  strcpy(_label, staticLabel);
}

Selectable::Selectable(int x, int y, dynamicLabelCallback labelCallback)
  :_x(x), _y(y), _dynamicLabelCallback(labelCallback), _previousSelectable(NULL), _nextSelectable(NULL), 
  _textScale(1), _state(Selectable::NONE), _redrawRequired(true), _enabled(true)
{
  strcpy(_label, "");
}

bool Selectable::Update()
{
  if(_enabled)
  {
    if(_dynamicLabelCallback != 0)
    {
      strcpy(__tempLabel, _dynamicLabelCallback()); 
      if(strcmp(__tempLabel, _label) != 0)
      {
        strcpy(_label, __tempLabel);
        _redrawRequired = true;
      }    
    }
  }

  return _redrawRequired && _enabled;
}

void Selectable::Draw(Adafruit_GFX* display)
{
  if(_enabled)
  {
    int16_t  x1, y1;

    int16_t localX = _x;
    int16_t localY = _y;

    display->setTextColor(SSD1306_WHITE);  
    display->setTextSize(_textScale);
    display->getTextBounds(_label, 0, 0, &x1, &y1, &_width, &_height);

    _width += 4 - _textScale;
    _height += 4 - _textScale;

    if(_alignment == TOP_RIGHT)
    {
      localX -= _width;
    }
    else if(_alignment == BOTTOM_LEFT)
    {
      localY -= _height;
    }
    else if(_alignment == BOTTOM_RIGHT)
    {
      localX -= _width;
      localY -= _height; 
    }

    display->setTextWrap(false);
    display->setCursor(localX+2,localY+2);  
    display->print(_label);

    if(_state == Selectable::FOCUSED)
    {
      display->drawRect(localX,localY,_width,_height,SSD1306_WHITE);
    }
    else if(_state == Selectable::SELECTED)
    {
      display->fillRect(localX,localY,_width,_height,SSD1306_INVERSE);
    }

    _redrawRequired = false;
  }
}

void Selectable::SetTextScale(unsigned int scale) 
{ 
  _textScale = scale; 
  _redrawRequired = true;
}
unsigned int Selectable::GetTextScale(){ return _textScale; }

void Selectable::SetPosition(int16_t x, int16_t y)
{ 
  _x = x; 
  _y = y; 
  _redrawRequired = true;
}
int16_t Selectable::GetPositionX(){ return _x; }
int16_t Selectable::GetPositionY(){ return _y; }

uint16_t Selectable::GetWidth(){ return _width; }
uint16_t Selectable::GetHeight(){ return _height; }

char* Selectable::GetStaticLabel(){ return _label; }
void Selectable::SetStaticLabel(const char* staticLabel)
{
  strcpy(_label, staticLabel);
  _dynamicLabelCallback = 0;
  _redrawRequired = true;
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
void Selectable::SetState(Selectable::State newState) 
{ 
  _state = newState; 
  _redrawRequired = true;
}

Selectable::Alignment Selectable::GetAlignment() { return _alignment; }
void Selectable::SetAlignment(Selectable::Alignment alignment) 
{ 
  _alignment = alignment; 
  _redrawRequired = true;
}

void Selectable::SetDynamicLabelCallback(dynamicLabelCallback dynamicLabelCallback) { _dynamicLabelCallback = dynamicLabelCallback; }