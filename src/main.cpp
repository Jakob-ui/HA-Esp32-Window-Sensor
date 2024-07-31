#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "interactive_Elements.h"
#include "oled_Display.h"
#include "Ha.h"
#include "Config.h"
#include "globals.h"

void deep_sleep();
void onButtonPressed();
void alarmButtonPressed();
void IRAM_ATTR buttonISR();
void sensor1_interrupt();
void sensor2_interrupt();
void tap_sensor_interrupt();
void turn_off_periph();
void tapping_sensor();
void reset_sleep_timer();

//Buzzer
#define buzzer 18

//Buttons
// ON Butto
#define onButton 15

// ALARM Button
#define alarmButton  4
bool alarmState = false;
bool previousalarmState = false;
volatile int InterruptalarmButtonstate = 0;
int alarmButtonDebouncetime = 40;
int lastalarmButtonState = 0;
unsigned long alarmButtonlastPress;
bool alarmtriggered = false;

//Alarm Settings
bool NotiMode = false;
bool alarmAndNoti = false;

//LED's
//Power LED
#define onLEDgreen 19
bool onLEDstate = HIGH;
//Alarm LED
#define alarmLEDred 27
#define alarmLEDblue 13

//Sleep Modes
//Deep Sleep Mode
RTC_DATA_ATTR bool sleepMode = false;
unsigned long currenttime;
int waittime = 3000;
unsigned long buttonPressDuration;
bool entered = false;
bool resetentered = false;

//Light Sleep + Wake up on Tap
#define tap_sensor 32
bool tapped = true;
unsigned long awaketime;
int awaketimeduration = 10000;
bool resettapped = true;
bool lightsleepmodeActivated = true;

//Sensors
//Sensor 1
#define sensor_1 14
bool window_1 = false;
//Sensor 2
#define sensor_2 12
bool window_2 = false;

//state machine
int alarmtype = 0;

//Json Daten
struct Secrets config;

//Battery voltage
#define VOLTAGEPIN 35
int batteryStatus;
bool meassuredVoltage = false;

//MQTT Sensors
//Sensor 1
HABinarySensor windowSensor_1 ("windowSensor_1");
const char* name_window_1 = "window_1";
const char* device_window_1 = "window";
const char* icon_window_1 = "mdi:window-closed-variant";

//Sensor 2
HABinarySensor windowSensor_2 ("windowSensor_2");
const char* name_window_2 = "window_2";
const char* device_window_2 = "window";
const char* icon_window_2 = "mdi:window-closed-variant";

//Alarm Mode
HABinarySensor notification ("Notification");
const char* name_notification = "notification";
const char* device_notification = "None";
const char* icon_notification = "mdi:alert-circle-check-outline";



void setup() {
  oledSetup();
  Serial.begin(115200);
  sleepMode = false;  //reset sleepmode on startup
  pinMode(buzzer, OUTPUT);
  pinMode(onLEDgreen, OUTPUT);
  pinMode(alarmLEDred, OUTPUT); 
  pinMode(alarmLEDblue, OUTPUT);
  pinMode(onButton, INPUT);
  pinMode(alarmButton, INPUT);
  pinMode(sensor_1, INPUT);
  pinMode(sensor_2, INPUT);
  pinMode(tap_sensor, INPUT);
  pinMode(VOLTAGEPIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensor_1), sensor1_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(sensor_2), sensor2_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(tap_sensor), tap_sensor_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(onButton), onButtonPressed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(alarmButton), alarmButtonPressed, CHANGE);
  Serial.print("Started...");
  digitalWrite(onLEDgreen, onLEDstate);
  analogWrite(buzzer, 0);
  deserializeJsondoc(&config);
  mqttsetup(&config);
  sensorSetup(windowSensor_1, window_1, name_window_1, device_window_1, icon_window_1);
  sensorSetup(windowSensor_2, window_2, name_window_2, device_window_2, icon_window_2);
  sensorSetup(notification, NotiMode, name_notification, device_notification, icon_notification);
}


void loop() {
  mqttloop();
  deep_sleep();
  buttons(alarmButton, alarmState, InterruptalarmButtonstate, alarmButtonDebouncetime, lastalarmButtonState, alarmButtonlastPress);
  window_Sensors(window_1, buzzer, alarmAndNoti, alarmtriggered);
  window_Sensors(window_2, buzzer, alarmAndNoti, alarmtriggered);
  switch (alarmtype)
  {
  case 0: //Changes Mode to Disarmed
    digitalWrite(alarmLEDred, LOW);
    digitalWrite(alarmLEDblue, LOW);
    NotiMode = false;
    alarmAndNoti = false; 
    displayCenteredText("Disarmed!", 2, alarmAndNoti);
    break;
  case 1: //Changes Mode to Alarm!
    digitalWrite(alarmLEDred, LOW);
    digitalWrite(alarmLEDblue, HIGH);
    NotiMode = true; 
    alarmAndNoti = false; 
    displayCenteredText("Alarm!", 2, alarmAndNoti);
    break;
  case 2: //Changes Mode to Alarm and Alert
    digitalWrite(alarmLEDred, HIGH);
    digitalWrite(alarmLEDblue, LOW);
    NotiMode = true;
    alarmAndNoti = true;
    displayCenteredText("Alarm and Alert", 1, alarmAndNoti);
    break;
}

//Handling changes in state machine
if(alarmState != previousalarmState && alarmtype < 2){
  alarmtype ++;
  previousalarmState = alarmState;
} 
if(alarmState != previousalarmState && alarmtype == 2){
  alarmtype = 0;
  previousalarmState = alarmState;
}

tapping_sensor(); //Run the light sleep function + wake up
sensorUpdate(windowSensor_1, window_1);
sensorUpdate(windowSensor_2, window_2);
sensorUpdate(notification, NotiMode);

if (meassuredVoltage == false) {
  batteryStatus = analogRead(VOLTAGEPIN); // Update the battery status
  meassuredVoltage = true;
}

//---------------------------Debug-----------------------------
  //Serial.println("press:" +  String(digitalRead(alarmButton)));
  //Serial.println("ONpress:" +  String(digitalRead(onButton)));
  //Serial.println("sensor1: " + String(digitalRead(sensor_1)));
  //Serial.println("sensor2: " + String(digitalRead(sensor_2)));
  Serial.println("tap_sensor: " + String(digitalRead(tap_sensor)));
  //Serial.println("BAttery: " + String(analogRead(VOLTAGEPIN)));
  //Serial.println("sensor2: " + String(window_2));
  //Serial.println("sensor1: " + String(window_1));
  //Serial.println("buttonPressDuration: " + String(buttonPressDuration));
  //Serial.println("currenttime: " + String(currenttime));
  //Serial.println("LED: " + String(digitalRead(onLED)));
  //Serial.println("lastpress: " + String(lastonButtonState));
}

//Interrupt functions
void onButtonPressed (){
  sleepMode = true;
  reset_sleep_timer();
  alarmtriggered = false;
  tapped = true;
  resettapped = true;
  reset_sleep_timer();
}

void alarmButtonPressed (){
  InterruptalarmButtonstate = 1;
  reset_sleep_timer();
  alarmtriggered = false;
  tapped = true;
  resettapped = true;
  reset_sleep_timer();
}

void sensor1_interrupt() {
window_1 = !window_1;
tapped = true;
  resettapped = true;
  reset_sleep_timer();
}

void sensor2_interrupt() {
window_2 = !window_2;
tapped = true;
  resettapped = true;
  reset_sleep_timer();
}

void tap_sensor_interrupt() {
  alarmtriggered = false;
  tapped = true;
  resettapped = true;
  reset_sleep_timer();
}


void deep_sleep(){
if (sleepMode && digitalRead(onButton) == HIGH) {
  if(!entered){
  currenttime = millis();
  resetentered = false;
  entered = true;
  }
  buttonPressDuration = millis() - currenttime;
  //Timer to go into deep sleep
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
    delay(1000);                                 
    esp_deep_sleep_start(); 
  } else if(digitalRead(onButton) == LOW && !resetentered){
    digitalWrite(onLEDgreen, HIGH);
    entered = false;
    sleepMode = false;
    resetentered = true;
  }
}



void turn_off_periph(){
    digitalWrite(onLEDgreen, LOW);
    digitalWrite(alarmLEDred, LOW);
    digitalWrite(alarmLEDblue, LOW);
    analogWrite(buzzer, 0);
    turnOffDisplay();
}

void turn_on_periph(){
  digitalWrite(onLEDgreen, HIGH);
  turnONDisplay();
}


void tapping_sensor(){
  bool entered = false;
  if(lightsleepmodeActivated){ //---------------------------------- To run light sleep Mode delete Line -----------------------------
  if(tapped && resettapped){
    awaketime = millis();
    resettapped = false;
    entered = false;
    Serial.println("Woke up from light sleep");
    turn_on_periph();
  } else if( millis() >= awaketime + awaketimeduration && entered == false){
      tapped = false;
      entered = true;
      Serial.println("Entering light sleep...");
      turn_off_periph();
      meassuredVoltage = false;
      /*// Enable wakeup from external signals
        esp_sleep_enable_ext0_wakeup((gpio_num_t)tap_sensor, CHANGE);
        esp_sleep_enable_ext0_wakeup((gpio_num_t)sensor_2, CHANGE);
        esp_sleep_enable_ext0_wakeup((gpio_num_t)sensor_1, CHANGE);

        // Enable wakeup from button presses
        esp_sleep_enable_ext1_wakeup((1ULL << onButton) | (1ULL << alarmButton), ESP_EXT1_WAKEUP_ANY_HIGH);

      turn_off_periph();
      delay(100);
      esp_light_sleep_start();
      Serial.println("Woke up from light sleep");
      meassuredVoltage = false;
      turn_on_periph();
      */
    }
}
}

void reset_sleep_timer(){
  awaketime = millis();
}

