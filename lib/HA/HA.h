#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Config.h>
#include <ArduinoHA.h>

void mqttsetup();
void mqttloop();
void sensorSetup(HABinarySensor sensor, bool state, String sensor_name, String sensor_device, String sensor_icon);