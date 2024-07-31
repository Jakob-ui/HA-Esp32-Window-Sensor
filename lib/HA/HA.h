#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <ArduinoHA.h>
#include "Config.h"
#include "oled_Display.h"

void mqttsetup(Secrets* config);
void mqttloop();
void sensorSetup(HABinarySensor sensor, bool state, const char *sensor_name, const char *sensor_device, const char *sensor_icon);
void sensorUpdate(HABinarySensor sensor, bool state);