#include <interactive_Elements.h>

bool flag = true;

void buttons(int buttonpin, bool &globalstate, volatile int &interruptstate, int debounceTime, int &lastButtonState, unsigned long &lastPress){
  if ((millis() - lastPress) > debounceTime && interruptstate == 1)
  {
    lastPress = millis();
    if(digitalRead(buttonpin) == 1 && lastButtonState == 0)
    {
      globalstate = !globalstate;
      lastButtonState = 1;
    }
    else if(digitalRead(buttonpin) == 0 && lastButtonState == 1)
    {
      lastButtonState = 0;
    }
    interruptstate = 0;
  }
}

void window_Sensors(int windowsensor, int buzzer, bool &alarmtype, bool &triggered){
  if(windowsensor == 1 && alarmtype == true){
    if(flag){
      triggered = true;
      flag = false;
    }
    }
  if(triggered == true){
      analogWrite(buzzer, 210);
      flag = true;
    } else if(triggered == false){
      analogWrite(buzzer, 0);
    }
}
