#ifndef interactive_Elements_H
#define interactive_Elements_H
#include <Arduino.h>

void buttons(int buttonpin, bool &globalstate, volatile int &interruptstate, int debounceTime, int &lastButtonState, unsigned long &lastPress);
void window_Sensors(int someSensor, int buzzer, bool &alarmtype);
void tapping_sensor(int tap_sensor, bool &tapped, bool &reset_tap, unsigned long &onwakeuptime, int awaketimeduration);

#endif