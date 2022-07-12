#include <Arduino.h>
#include <FlashStorage.h>

#include "Controller.h"
#include "IntervalInfo.h"

FlashStorage(save_data_storage, IntervalometerSaveData);

void setup() 
{  
  Controller::Initialize();

  IntervalometerSaveData data = save_data_storage.read();
  if(data.isValid)
  {
    Controller::GetInstance()->GetConfig()->SetSaveData(data);
  }

  delay(2000);
}

void loop() 
{
  //if(millis() > 2000)
  {
    Controller::GetInstance()->Update();

    if(Controller::GetInstance()->GetConfig()->GetIsSaveDataDirty())
    {
      save_data_storage.write(*Controller::GetInstance()->GetConfig()->GetSaveData());
      Controller::GetInstance()->GetConfig()->SetIsSaveDataDirty(false);
    }
  }
}
