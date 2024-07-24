#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <interactive_Elements.h>
#include <oled_Display.h>
#include <Ha.h>

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
#define buzzer 16

//Buttons
// ON Button
#define onButton 15

// ALARM Button
#define alarmButton  23
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
#define onLEDgreen 19
bool onLEDstate = HIGH;
#define alarmLEDred 4
#define alarmLEDblue 17

//Deep Sleep Mode
RTC_DATA_ATTR bool sleepMode = false;
unsigned long currenttime;
int waittime = 3000;
unsigned long buttonPressDuration;
bool entered = false;
bool resetentered = false;

//Sensors
#define sensor_1 5
bool window_1 = false;
bool lastInputState_1;
#define sensor_2 18
bool window_2 = false;
bool lastInputState_2;

//Wake up on Tap
#define tap_sensor 2
bool tapped = true;
unsigned long awaketime;
int awaketimeduration = 10000;
bool resettapped = true;
bool lightsleepmodeActivated = false;

//State Maschine
int alarmtype = 0;


//MQTT Sensors
HABinarySensor windowSensor_1 ("windowSensor_1");
String name_window_1 = "window_1";
String device_window_1 = "window";
String icon_window_1 = "mdi:fire";

HABinarySensor windowSensor_2 ("windowSensor_2");
String name_window_2 = "window_2";
String device_window_2 = "window";
String icon_window_2 = "mdi:fire";

void setup() {
  oledSetup();
  Serial.begin(115200);
  sleepMode = false;
  pinMode(buzzer, OUTPUT);
  pinMode(onLEDgreen, OUTPUT);
  pinMode(alarmLEDred, OUTPUT); 
  pinMode(alarmLEDblue, OUTPUT);
  pinMode(onButton, INPUT_PULLDOWN);
  pinMode(alarmButton, INPUT_PULLDOWN);
  pinMode(sensor_1, INPUT_PULLDOWN);
  pinMode(sensor_2, INPUT_PULLDOWN);
  pinMode(tap_sensor, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(sensor_1), sensor1_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(sensor_2), sensor2_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(tap_sensor), tap_sensor_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(onButton), onButtonPressed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(alarmButton), alarmButtonPressed, CHANGE);
  Serial.print("Started...");
  digitalWrite(onLEDgreen, onLEDstate);
  analogWrite(buzzer, 0);
  mqttsetup();
  sensorSetup(windowSensor_1,sensor_1 , window_1, name_window_1, device_window_1, icon_window_1);
  sensorSetup(windowSensor_2,sensor_2 , window_2, name_window_2, device_window_2, icon_window_2);
  lastInputState_1 = digitalRead(sensor_1);
  lastInputState_2 = digitalRead(sensor_2);
}

void loop() {
  mqttloop();
  deep_sleep();
  buttons(alarmButton, alarmState, InterruptalarmButtonstate, alarmButtonDebouncetime, lastalarmButtonState, alarmButtonlastPress);
  //Serial.println("press:" +  String(digitalRead(alarmButton)));
  //Serial.println("lastpress: " + String(lastonButtonState));
  delay(50);
  window_Sensors(window_1, buzzer, alarmAndNoti, alarmtriggered);
  window_Sensors(window_2, buzzer, alarmAndNoti, alarmtriggered);
  //Serial.println("sensor1: " + String(window_1));
  //Serial.println(digitalRead(sensor_1));
  //Serial.println("sensor2: " + String(window_2));
  //Serial.println("sensor3: " + String(tapped));
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
tapping_sensor();
sensorUpdate(windowSensor_1, sensor_1);
sensorUpdate(windowSensor_2, sensor_2);
}

//Interrupt Functions
void onButtonPressed (){
  sleepMode = true;
  reset_sleep_timer();
  alarmtriggered = false;
}

void alarmButtonPressed (){
  InterruptalarmButtonstate = 1;
  reset_sleep_timer();
  alarmtriggered = false;
}

void sensor1_interrupt() {
window_1 = !window_1; 
}

void sensor2_interrupt() {
window_2 = !window_2;
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
  if(lightsleepmodeActivated){ //---------------------------------- To run light sleep Mode delete Line -----------------------------
  if(tapped && resettapped){
    awaketime = millis();
    resettapped = false;
    turn_on_periph();
  } else if( millis() >= awaketime + awaketimeduration){
      tapped = false;
    Serial.println("Entering light sleep...");
    esp_sleep_enable_ext0_wakeup((gpio_num_t)tap_sensor, HIGH);
    esp_sleep_enable_ext1_wakeup(1ULL << sensor_2, ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_sleep_enable_ext1_wakeup(1ULL << sensor_1, ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_sleep_enable_ext1_wakeup(1ULL << onButton, ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_sleep_enable_ext1_wakeup(1ULL << alarmButton, ESP_EXT1_WAKEUP_ANY_HIGH);
    turn_off_periph();
    delay(100);
    esp_light_sleep_start();
    Serial.println("Woke up from light sleep");
    }
}
}

void reset_sleep_timer(){
  awaketime = millis();
}

