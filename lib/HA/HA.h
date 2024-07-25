#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Config.h>
#include <ArduinoHA.h>
#include <Config.h>
#include <oled_Display.h>

void mqttsetup();
void mqttloop();
void sensorSetup(HABinarySensor sensor, bool state, const char *sensor_name, const char *sensor_device, const char *sensor_icon);
void sensorUpdate(HABinarySensor sensor, bool state);
/*
class HA
{
  public:
    HA(Secrets &config);

    private:
    
    Secrets &config;
};
*/