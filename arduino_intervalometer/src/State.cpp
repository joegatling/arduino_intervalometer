#include "State.h"

bool State::UpdateAllSelectables()
{
  bool redrawRequired = false;

  for(unsigned int i = 0; i < _selectableCount; i++)
  {
    redrawRequired |= _allSelectables[i]->Update();
  }


  return redrawRequired;
}

void State::DrawAllSelectables(Adafruit_GFX* display)
{
  for(unsigned int i = 0; i < _selectableCount; i++)
  {
    _allSelectables[i]->Draw(display);
  }
}

void State::AddSelectable(Selectable* s)
{
  if(_selectableCount < MAX_SELECTABLES)
  {
    _allSelectables[_selectableCount] = s;
    _selectableCount++;
  }
}

Selectable* State::GetCurrentSelectable()
{
  return _currentSelectable;
}

void State::SetCurrentSelectable(Selectable* s, bool isSelected)
{
  if(s != _currentSelectable && s->GetEnabled() == true)
  {
    if(_currentSelectable != NULL)
    {
      _currentSelectable->SetState(Selectable::NONE);
    }

    _currentSelectable = s;

    if(_currentSelectable != NULL)
    {
      if(isSelected)
      {
        _currentSelectable->SetState(Selectable::SELECTED);
      }
      else
      {
        _currentSelectable->SetState(Selectable::FOCUSED);
      }
    }
  }
}

Selectable* State::IncrementCurrentSelectable(int increment)
{
  Selectable* newSelectable = NULL;
  
  if(increment > 0)
  {  
    newSelectable = _currentSelectable->GetNextSelectable();

    while(newSelectable != NULL && newSelectable->GetEnabled() == false && newSelectable != _currentSelectable)
    {
      newSelectable = newSelectable->GetNextSelectable();
    }             
  }
  else if(increment < 0)
  {
    newSelectable = _currentSelectable->GetPreviousSelectable();

    while(newSelectable != NULL && newSelectable->GetEnabled() == false && newSelectable != _currentSelectable)
    {
      newSelectable = newSelectable->GetPreviousSelectable();
    } 
  }  

  if(newSelectable != NULL)
  {
    
    _currentSelectable->SetState(Selectable::NONE);
    _currentSelectable = newSelectable;
    _currentSelectable->SetState(Selectable::FOCUSED);
  }

  return _currentSelectable;
}
