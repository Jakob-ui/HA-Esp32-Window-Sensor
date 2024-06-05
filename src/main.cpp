#include <Arduino.h>
#include <spi.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <interactive_Elements.h>
#include <oled_Display.h>

void sleep();
void onButtonPressed();
void alarmButtonPressed();
void IRAM_ATTR buttonISR();
void sensor1_interrupt();
void sensor2_interrupt();
void sensor3_interrupt();


//Buzzer
int buzzer = 16;

//Buttons
// ON Button
int onButton = 15;

// ALARM Button
bool alarmState = false;
bool previousalarmState = false;
int alarmButton = 23;
volatile int InterruptalarmButtonstate = 0;
int alarmButtonDebouncetime = 40;
int lastalarmButtonState = 0;
unsigned long alarmButtonlastPress;

//Alarm Settings
bool NotiMode = false;
bool alarmAndNoti = false;

//LED's
int onLEDgreen = 2;
bool onLEDstate = HIGH;
int alarmLEDred = 4;
int alarmLEDblue = 17;

//Sleep Mode
RTC_DATA_ATTR bool sleepMode = false;
unsigned long currenttime;
int waittime = 3000;
unsigned long buttonPressDuration;
bool entered = false;
bool resetentered = false;

//Sensors
int sensor_1 = 5;
bool window_1 = false;
int sensor_2 = 18;
bool window_2 = false;
int sensor_3 = 19;
bool window_3 = false;

//State Maschine
int alarmtype = 0;

void setup() {
  sleepMode = false;
  pinMode(buzzer, OUTPUT);
  pinMode(onLEDgreen, OUTPUT);
  pinMode(alarmLEDred, OUTPUT); 
  pinMode(alarmLEDblue, OUTPUT);
  pinMode(onButton, INPUT_PULLDOWN);
  pinMode(alarmButton, INPUT_PULLDOWN);
  pinMode(sensor_1, INPUT_PULLDOWN);
  pinMode(sensor_2, INPUT_PULLDOWN);
  pinMode(sensor_3, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(sensor_1), sensor1_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(sensor_2), sensor2_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(sensor_3), sensor3_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(onButton), onButtonPressed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(alarmButton), alarmButtonPressed, CHANGE);
  Serial.begin(115200);
  Serial.print("Started...");
  digitalWrite(onLEDgreen, onLEDstate);
  oledSetup();
}

void loop() {
  sleep();
  buttons(alarmButton, alarmState, InterruptalarmButtonstate, alarmButtonDebouncetime, lastalarmButtonState, alarmButtonlastPress);
  //Serial.println("press:" +  String(digitalRead(alarmButton)));
  //Serial.println("lastpress: " + String(lastonButtonState));
  delay(50);
  window_Sensors(window_1, buzzer, alarmAndNoti);
  window_Sensors(window_2, buzzer, alarmAndNoti);
  window_Sensors(window_3, buzzer, alarmAndNoti);
  Serial.println("sensor1: " + String(window_1));
  Serial.println("sensor2: " + String(window_2));
  Serial.println("sensor3: " + String(window_3));

  switch (alarmtype)
  {
  case 0:
    digitalWrite(alarmLEDred, LOW);
    digitalWrite(alarmLEDblue, LOW);
    NotiMode = false;
    alarmAndNoti = false;
    displayCenteredText("Disarmed!", 2);
    break;
  case 1:
    digitalWrite(alarmLEDred, LOW);
    digitalWrite(alarmLEDblue, HIGH);
    NotiMode = true;
    alarmAndNoti = false;
    displayCenteredText("Alarm!", 2);
    break;
  case 2:
    digitalWrite(alarmLEDred, HIGH);
    digitalWrite(alarmLEDblue, LOW);
    NotiMode = true;
    alarmAndNoti = true;
    displayCenteredText("Alarm and Alert", 1);
    break;
}
if(alarmState != previousalarmState && alarmtype < 2){
  alarmtype ++;
  previousalarmState = alarmState;
} 
if(alarmState != previousalarmState && alarmtype == 2){
  alarmtype = 0;
  previousalarmState = alarmState;
}
}


//Interrupt Functions
void onButtonPressed (){
  sleepMode = true;
}

void alarmButtonPressed (){
  InterruptalarmButtonstate = 1;
}

void sensor1_interrupt() {
window_1 = !window_1; 
}

void sensor2_interrupt() {
window_2 = !window_2;
}

void sensor3_interrupt() {
window_3 = !window_3;
}


void sleep(){
if (sleepMode && digitalRead(onButton) == HIGH) {
  if(!entered){
  currenttime = millis();
  resetentered = false;
  entered = true;
  }
  buttonPressDuration = millis() - currenttime;
  //Serial.println("buttonPressDuration: " + String(buttonPressDuration));
  //Serial.println("currenttime: " + String(currenttime));
  //Serial.println("LED: " + String(digitalRead(onLED)));
  if (buttonPressDuration >= 2400){
    digitalWrite(onLEDgreen, HIGH);
  } else if(buttonPressDuration >= 1800){
    digitalWrite(onLEDgreen, LOW);
  } else if(buttonPressDuration >= 1200){
    digitalWrite(onLEDgreen, HIGH);
  } else if(buttonPressDuration >= 600){
    digitalWrite(onLEDgreen, LOW);
  }
  }
  if (buttonPressDuration >= waittime)
  {
    Serial.println("Going to sleep...");
    turnOffDisplay();
    esp_sleep_enable_ext0_wakeup((gpio_num_t)onButton, HIGH); 
    analogWrite(buzzer, 0);
    digitalWrite(onLEDgreen, LOW);
    delay(2000);                                 
    esp_deep_sleep_start(); 
  } else if(digitalRead(onButton) == LOW && !resetentered){
    digitalWrite(onLEDgreen, HIGH);
    entered = false;
    sleepMode = false;
    resetentered = true;
  }
}