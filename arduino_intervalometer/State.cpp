#include "State.h"

bool State::UpdateAllSelectables()
{
  bool redrawRequired = false;

  for(int i = 0; i < _selectableCount; i++)
  {
    redrawRequired |= _allSelectables[i]->Update();
  }


  return redrawRequired;
}

void State::DrawAllSelectables(Adafruit_GFX* display)
{
  for(int i = 0; i < _selectableCount; i++)
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
  if(s != _currentSelectable)
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
  if(increment > 0)
  {
    if(_currentSelectable->GetNextSelectable() != NULL)
    {
      _currentSelectable->SetState(Selectable::NONE);
      _currentSelectable = _currentSelectable->GetNextSelectable();
      _currentSelectable->SetState(Selectable::FOCUSED);
    }
  }
  else if(increment < 0)
  {
    if(_currentSelectable->GetPreviousSelectable() != NULL)
    {
      _currentSelectable->SetState(Selectable::NONE);
      _currentSelectable = _currentSelectable->GetPreviousSelectable();
      _currentSelectable->SetState(Selectable::FOCUSED);
  
    }      
  }  

  return _currentSelectable;
}
